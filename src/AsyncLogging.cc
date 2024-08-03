#include <stdio.h>
#include <assert.h>

#include <chrono>

#include "Timestamp.h"
#include "AsyncLogging.h"

using namespace ymuduo;

AsyncLogging::AsyncLogging(const std::string &basename, off_t rollSize, const std::string& out_log_file_name, int flushInterval)
	: flushInterval_(flushInterval), running_(false), basename_(basename), buffers_()
	, rollSize_(rollSize), thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging")
	, latch_(1), mutex_(), cond_(), currentBuffer_(new Buffer), nextBuffer_(new Buffer)
	, log_file_name_(out_log_file_name) {
	currentBuffer_->bzero();
	nextBuffer_->bzero();
	buffers_.reserve(16);
}

void AsyncLogging::append(const char *logline, int len) {

	std::unique_lock<std::mutex> lock(mutex_);
	if (currentBuffer_->avail() > len) {
		currentBuffer_->append(logline, len);
	} else {
		buffers_.push_back(std::move(currentBuffer_));

		if (nextBuffer_) {
			currentBuffer_ = std::move(nextBuffer_);
		} else {
			currentBuffer_.reset(new Buffer); // Rarely happens
		}
		currentBuffer_->append(logline, len);
		cond_.notify_one();
	}
}

void AsyncLogging::threadFunc() {
	latch_.countDown();
	int fd = ::open(log_file_name_.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (fd < 0) {
		std::cerr << "Failed to open log file" << std::endl;
		return;
	}

	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);
	while (running_) {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (buffers_.empty()) {
				cond_.wait_for(lock, std::chrono::seconds(1) * flushInterval_, [&] { return !buffers_.empty(); });
			}
			buffers_.push_back(std::move(currentBuffer_));
			currentBuffer_ = std::move(newBuffer1);
			buffersToWrite.swap(buffers_);
			if (!nextBuffer_) {
				nextBuffer_ = std::move(newBuffer2);
			}
		}

		if (buffersToWrite.size() > 25) {
			char buf[256];
			snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
						Timestamp::now().toFormattedString().c_str(), buffersToWrite.size() - 2);
			fputs(buf, stderr);
			::write(fd, buf, strlen(buf));
			buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
		}

		for (const auto &buffer : buffersToWrite) {
			::write(fd, buffer->data(), buffer->length());
		}

		if (buffersToWrite.size() > 2) {
			buffersToWrite.resize(2);
		}

		if (!newBuffer1) {
			newBuffer1 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}

		if (!newBuffer2) {
			newBuffer2 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}

		buffersToWrite.clear();
	}
	::close(fd);
}