
#ifndef YMUDUO_TIMERID
#define YMUDUO_TIMERID

namespace ymuduo {

class Timer;

class TimerId {
public:
    
    friend class TimerQueue;

    TimerId() : timer_(nullptr), sequence_(0) {}

    TimerId(Timer* timer, int64_t seq) : timer_(timer), sequence_(seq) {}

private:
    Timer* timer_;
    int64_t sequence_;

};



}

#endif