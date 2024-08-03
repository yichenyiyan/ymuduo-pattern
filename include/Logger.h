#ifndef YMUDUO_LOGGER
#define YMUDUO_LOGGER

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <string>
#include <functional>

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

class Logger : noncopyable {
public:

    using LogBuffer = FixedBuffer<kSmallBuffer>;

    //  获取日志单例
    static Logger& GetInstance();
    // 设置日志级别
    void setLogLevel(int level);
    // 写日志
    void log(std::string msg);
    using OutputFunc = std::function<void(const std::string&)>;

    // 设置日志输出回调 
    static void setOutPut(OutputFunc);

    ~Logger() { close(log_file_fd_); }

    void closeLoggerRecord() { allowRecord = false; }

    void startLogerRecord() { allowRecord = true; }

private:
    bool allowRecord;

    void defaultOutput(const std::string& msg);

    OutputFunc g_output;
    /* log文件描述符 */
    int log_file_fd_;
    int logLevel_;
    // 构造函数私有化
    Logger() : allowRecord(true) {
        log_file_fd_ = open(log_file_name_, O_WRONLY | O_CREAT | O_APPEND, 0644);
        g_output = std::bind(&Logger::defaultOutput, this, std::placeholders::_1);
    }

    const char* log_file_name_ = "Server.log";
};


#define LOG_INFO(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
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
        Logger &logger = Logger::GetInstance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while(0) 

#ifdef YMUDUO_TODEBUG
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