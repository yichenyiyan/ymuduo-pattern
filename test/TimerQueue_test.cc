#include <unistd.h>

#include <memory>
#include <iostream>

#include "Timer.h"
#include "EventLoop.h"
#include "TimerQueue.h"

using namespace ymuduo;

static int change_able_number_ = 0;

static void func1() {
    std::cout << "timer func1" << std::endl;
    change_able_number_++;
}


static void func2() {
    std::cout << "timer func2" << std::endl;
    change_able_number_ += 2;
}

static void testFunc() {
    std::unique_ptr<EventLoop> loop = std::make_unique<EventLoop>();
    std::unique_ptr<TimerQueue> tqueue = std::make_unique<TimerQueue>(loop.get());
    TimerCallback cb1 = std::bind(func1);
    TimerCallback cb2 = std::bind(func2);
    for (int i = 0; i < 4; ++i) {
        if (i % 2 == 0)
            tqueue->addTimer(cb1, Timestamp::now(), 2.000);
        else
            tqueue->addTimer(cb2, Timestamp::now(), 2.000);
    }

    sleep(8);
    std::cout << "timer all number: " << Timer::numCreated() << std::endl; 
    std::cout << "after 8 seconds the number value is: " << change_able_number_ << std::endl; 
}

int main() {
    testFunc();
    return 0;
}
