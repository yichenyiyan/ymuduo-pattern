
#include <iostream>

#include "Logger.h"


using namespace ymuduo;

// LOG_INFO("%s  %d", arg1, arg2)

//  获取日志单例

using namespace ymuduo; 

// 默认输出到Server.log文件中
void Logger::defaultOutput(const std::string& msg) {
    if (!allowRecord)
        return;
    
    Timestamp now = Timestamp::now();
    if (now >= nextRollOverTime_) {
        rollOver();
    }
    size_t n = write(log_file_fd_, msg.c_str(), msg.size());
}


Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::setLogLevel(int level) {
    logLevel_ = level;
}

// 写日志
void Logger::log(std::string msg) {
    std::string buf = "";
    switch (logLevel_) {
    case INFO:  buf += "[INFO]" ; break;
    case ERROR: buf += "[ERROR]"; break;
    case FATAL: buf += "[FATAL]"; break;
    case DEBUG: buf += "[DEBUG]"; break;
    default: break;
    }

    buf += "Time: " + Timestamp::now().toFormattedString(false) + " Content: " + msg + "\n";

    g_output(buf);

    if (logLevel_ == FATAL) {
        exit(0);
    }
}

void Logger::rollOver() {
    if (log_file_fd_ != -1) {
        ::close(log_file_fd_);
    }

    currentFileName_.clear();
    currentFileName_ = log_file_name_ + getCurrentTimestamp() + ".log";
    log_file_fd_ = ::open(currentFileName_.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    nextRollOverTime_ = addTime(Timestamp::now(), 3600 * roll_every_hours); 
}



