
#ifndef YMUDUO_COUNTDOWNLATCH
#define YMUDUO_COUNTDOWNLATCH

#include <mutex>
#include <condition_variable>

#include "noncopyable.h"

namespace ymuduo {

class CountDownLatch : public noncopyable {
public:
    explicit CountDownLatch(int count);  //倒数count次
    void wait();
    void countDown();

    int getCount();

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    int count_;
};

}

#endif