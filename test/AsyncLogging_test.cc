#include <iostream>
#include <thread>
#include <unistd.h>

#include "AsyncLogging.h"
#include "CurrentThread.h"

using namespace ymuduo;

// 一个简单的日志生成函数
void logGenerator(AsyncLogging& logger, int count) {
    for (int i = 0; i < count; ++i) {
        std::string log = "thread id: " + std::to_string(CurrentThread::tid()) +  " to record log message " + std::to_string(i) +"\n";
        logger.append(log.c_str(), log.length());
        usleep(100); 
    }
}

int main() {
    AsyncLogging logger("test_log", 500 * 1024 * 1024);

    logger.start();

    // 创建多个线程来生成日志消息
    std::thread t1(logGenerator, std::ref(logger), 100);
    std::thread t2(logGenerator, std::ref(logger), 100);
    std::thread t3(logGenerator, std::ref(logger), 100);

    t1.join();
    t2.join();
    t3.join();

    logger.stop();

    std::cout << "Log generation completed." << std::endl;
    return 0;
}
