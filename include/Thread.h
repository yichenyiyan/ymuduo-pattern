#ifndef YMUDUO_THREAD
#define YMUDUO_THREAD

#include <unistd.h>

#include <thread>
#include <memory>
#include <atomic>
#include <string>
#include <functional>

#include "noncopyable.h"
#include "CountDownLatch.h"

namespace ymuduo {


class Thread : public noncopyable {
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, const std::string& name = std::string());
    ~Thread();

    void start();
    int join();

    bool started() const { return start_; }

    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }

    static int numCreated() { return numCreated_; }

private:
    void setDefaultName();

    bool start_;
    bool joined_;

    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    ThreadFunc func_;
    
    std::string name_;
    static std::atomic_int32_t numCreated_; // 记录总的创建线程的数量

    CountDownLatch latch_;
};

}
#endif