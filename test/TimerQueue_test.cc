#include <stdio.h>
#include <unistd.h>

#include "Logger.h"
#include "Thread.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

using namespace ymuduo;

static int cnt = 0;
EventLoop* g_loop;

void funcPrintTid() {
    printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    printf("toString now:%s\n", Timestamp::now().toString().c_str());
    printf("toFormattedString now:%s\n", Timestamp::now().toFormattedString().c_str());

}

void funcPrint(const char* msg) {
    printf("toString: msg %s %s\n", Timestamp::now().toString().c_str(), msg);
    printf("toFormattedString: msg %s %s\n", Timestamp::now().toFormattedString().c_str(), msg);

    if (++cnt == 20) {
        g_loop->quit();
    }
}

void cancel(TimerId timer) {
    g_loop->cancel(timer);
    printf("toString: cancelled at %s\n", Timestamp::now().toString().c_str());
    printf("toFormattedString: cancelled at %s\n", Timestamp::now().toFormattedString().c_str());

}

int main() {
    Logger& logger_ = Logger::GetInstance();
    logger_.setLoggerFileName("timer-queue-test");
    funcPrintTid();
    sleep(1);
    {
        EventLoop loop;
        g_loop = &loop;

        funcPrint("main");
        loop.runAfter(1, std::bind(funcPrint, "interval 1"));
        loop.runAfter(1.5, std::bind(funcPrint, "interval 1.5"));
        loop.runAfter(2.5, std::bind(funcPrint, "interval 2.5"));
        loop.runAfter(3.5, std::bind(funcPrint, "interval 3.5"));
        TimerId t45 = loop.runAfter(4.5, std::bind(funcPrint, "interval 4.5"));
        loop.runAfter(4.2, std::bind(cancel, t45));
        loop.runAfter(4.8, std::bind(cancel, t45));
        loop.runEvery(2, std::bind(funcPrint, "memfunc runevery 2"));
        TimerId t3 = loop.runEvery(3, std::bind(funcPrint, "memfunc runevery 3"));
        loop.runAfter(9.001, std::bind(cancel, t3));

        loop.loop();
        funcPrint("main loop exits");
    }
    sleep(1);
    {
        EventLoopThread loopThread;
        EventLoop* loop = loopThread.startLoop();
        loop->runAfter(2, funcPrintTid);
        sleep(3);
        funcPrint("thread loop exits");
    }

    return 0;
}
