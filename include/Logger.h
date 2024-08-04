#ifndef YMUDUO_LOGGER
#define YMUDUO_LOGGER

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <mutex>
#include <string>
#include <functional>

#include "Timestamp.h"
#include "noncopyable.h"

namespace ymuduo {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

template<int SIZE>
class FixedBuffer : noncopyable {
public:
  FixedBuffer() : cur_(data_) {}

  ~FixedBuffer() {}

  void append(const char* /*restrict*/ buf, size_t len) {
    if (static_cast<size_t>(avail()) > len) {
      ::memcpy(cur_, buf, len);
      cur_ += len;
    }
  }

  const char* data() const { return data_; }
  int length() const { return static_cast<int>(cur_ - data_); }

  // write to data_ directly
  char* current() { return cur_; }
  int avail() const { return static_cast<int>(end() - cur_); }
  void add(size_t len) { cur_ += len; }

  void reset() { cur_ = data_; }
  void bzero() { ::bzero(data_, sizeof data_); }

  std::string toString() const { return std::string(data_, length()); }

private:
  const char* end() const { return data_ + sizeof data_; }
  char data_[SIZE];
  char* cur_;
};



//定义日志的级别   INFO  ERROR   FATAL   DEBUG
enum LogLevel {
    INFO,
    ERROR,
    FATAL,
    DEBUG,
};

//   日志类

class Logger final : noncopyable {
public:

    using LogBuffer = FixedBuffer<kSmallBuffer>;

    //  获取日志单例
    static Logger& GetInstance();
    // 设置日志级别
    void setLogLevel(int level);
    // 写日志
    void log(std::string msg);
    using OutputFunc = std::function<void(const std::string&)>;


    ~Logger() { if (log_file_fd_ != -1) close(log_file_fd_); }

    void closeLoggerRecord() { allowRecord = false; }

    void startLogerRecord() { allowRecord = true; }

    void setLoggerFileName(const std::string& filename) { 
        // std::unique_lock<std::mutex>(mutex_); // 防止多个线程修改log文件名
        if (log_file_fd_ != -1)
            ::close(log_file_fd_);
        log_file_name_ = filename; 
        std::string filename_ = filename +  ".log";
        log_file_fd_ = ::open(filename_.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        //g_output = std::bind(&Logger::defaultOutput, this, std::placeholders::_1);
    }

    // 设置日志回滚时间 hours 小时
    void setRollTime(int hours) { roll_every_hours = hours; }

    // 设置日志输出回调 
    void setOutPut(OutputFunc outfunc) {
        // std::unique_lock<std::mutex>(mutex_); // 防止多个线程修改log文件名
        g_output = outfunc;
    }

private:
    std::mutex mutex_;
    bool allowRecord;

    void defaultOutput(const std::string& msg);

    OutputFunc g_output;
    /* log文件描述符 */
    int log_file_fd_;
    int logLevel_;

    Logger() : allowRecord(true),  log_file_name_("yichen-server"), roll_every_hours(1) {
        log_file_fd_ = open(log_file_name_.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        g_output = std::bind(&Logger::defaultOutput, this, std::placeholders::_1);
    }

    std::string log_file_name_;

private:
    /* update: support log rollling */
    int roll_every_hours;
    Timestamp nextRollOverTime_;
    std::string currentFileName_;

    std::string getCurrentFileName() const { return currentFileName_; }

    std::string getCurrentTimestamp() const { return Timestamp::now().toFormattedStringWithoutSeconds(); }

    void rollOver();
};


#define LOG_INFO(logmsgFormat, ...) \
    do \
    { \
        Logger& logger = Logger::GetInstance(); \
        logger.setLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 

#define LOG_ERROR(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 

#define LOG_FATAL(logmsgFormat, ...) \
    do \
    { \
        Logger& logger = Logger::GetInstance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while(0) 

#define LOG_SET_FILE_NAME(name) \
    do \
    { \
        Logger& logger = Logger::GetInstance(); \
        logger.setLoggerFileName(static_cast<const char*>(name)); \
    } while(0)


#ifdef MYMUDUO_DEBUG
#define LOG_DEBUG(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(DEBUG); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 
#else
    #define LOG_DEBUG(logmsgFormat, ...)
#endif

}

#endif