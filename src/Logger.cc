
#include <iostream>

#include "Logger.h"
#include "Timestamp.h"

using namespace ymuduo;

// LOG_INFO("%s  %d", arg1, arg2)

//  获取日志单例

using namespace ymuduo; 

// 默认输出到Server.log文件中
void Logger::defaultOutput(const std::string& msg) {
    if (!allowRecord)
        return;
    size_t n = write(log_file_fd_, msg.c_str(), msg.size());
}

// void Logger::defaultFlush() {
//     fflush(stdout);
// }



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



