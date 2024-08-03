#ifndef YMUDUO_ASYNCLOGGING
#define YMUDUO_ASYNCLOGGING

#include <atomic>
#include <vector>
#include <memory>
#include <iostream>

#include "Thread.h"
#include "Logger.h"
#include "noncopyable.h"
#include "CountDownLatch.h"

namespace ymuduo {

class AsyncLogging : noncopyable {
public:
    AsyncLogging(const std::string& basename, off_t rollSize, const std::string& out_log_file_name = "async.log", int flushInterval = 3);
    ~AsyncLogging() {
        if (running_)
            stop();
    }

    void append(const char* logline, int len);
    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() {
        running_ = false;
        cond_.notify_one();
        thread_.join();
    }

private:
    void threadFunc();

    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;
    using BufferPtr = BufferVector::value_type;

    std::string log_file_name_;
    const int flushInterval_;
    std::atomic<bool> running_;
    const std::string basename_;
    const off_t rollSize_;
    Thread thread_;
    CountDownLatch latch_;
    std::mutex mutex_;
    std::condition_variable cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};

}  // namespace ymuduo

#endif  




