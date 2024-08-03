#include <memory>
#include <iostream>

#include "Timer.h"
#include "Timestamp.h"

using namespace ymuduo;

static int freq = 0;

void timeoutFunc() {
    std::cout << freq++ << " it's time to go!" << std::endl;
}

static void timerTestFunc() {
    Timestamp t = Timestamp::now();
    TimerCallback cb = std::bind(timeoutFunc);
    std::unique_ptr<Timer> timer_ptr = std::make_unique<Timer>(cb, t, 2.00);
    timer_ptr->run();
    timer_ptr->restart(t);
    timer_ptr->run();
    std::cout << "timer's sequece: " << timer_ptr->sequece() << std::endl;
    auto ret = timer_ptr->expiration();
    ret.invalid();
    std::cout << "--------ret---------------" << std::endl;
    std::cout << ret.kMicroSecondsPerSecond << std::endl;
    std::cout << ret.microSecondsSinceEpoch() << std::endl;
    std::cout << ret.toFormattedString() << std::endl;
    std::cout << ret.toString() << std::endl;
    if (ret.valid()) {
        std::cout << "It is valid!" << std::endl;
    } else {
        std::cout << "It is invalid!" << std::endl;
    }
    ret.invalid();

}

int main() {
    timerTestFunc();
    return 0;
}