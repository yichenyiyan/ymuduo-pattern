#include <stdlib.h>

#include "Poller.h"
#include "EPollPoller.h"

using namespace ymuduo;

/* 默认IO多路复用模型为epoll */
Poller* Poller::newDefaultPoller(EventLoop *loop) {
    if (::getenv("MUDUO_USE_POLL"))
        return nullptr;
    else
        return new EPollPoller(loop);
}
