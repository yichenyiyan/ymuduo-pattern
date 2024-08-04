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
    AsyncLogging(const std::string& basename, int rollTime = 1,
        const std::string& out_log_file_name = "async", int flushInterval = 3);
    
    ~AsyncLogging() {
        if (running_)
            stop();
    }

    void setAsyncLogFileName(const std::string& filename) { 
        std::unique_lock<std::mutex>(mutex_);
        basename_ = filename; 
        ::close(log_fd_);
        log_fd_ = -1;
        log_file_name_.clear();
        log_file_name_ = basename_ + ".log";
        log_fd_ = ::open(log_file_name_.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
    }

    void setRollTime(int hours) { roll_every_hours = hours; }

    void append(const std::string& msg);

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
    using BufferPtr = std::unique_ptr<Buffer>;

    int log_fd_;
    std::string log_file_name_;
    const int flushInterval_;
    std::atomic<bool> running_;
    std::string basename_;
    Thread thread_;
    CountDownLatch latch_;
    std::mutex mutex_;
    std::condition_variable cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;

private:
    int roll_every_hours;
    Timestamp nextRollOverTime_;
    std::string currentFileName_;

    std::string getCurrentFileName() const { return currentFileName_; }

    std::string getCurrentTimestamp() const { return Timestamp::now().toFormattedStringWithoutSeconds(); }

    void rollOver();
};

}  // namespace ymuduo

#endif  




