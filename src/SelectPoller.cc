#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "Logger.h"
#include "Channel.h"
#include "SelectPoller.h"


using namespace ymuduo;

//channel未添加到poller中
const int kNew = -1;    //channel的成员index = -1
//channel已添加
const int kAdded = 1;
//channel从poller中删除
const int kDeleted = 2;

SelectPoller::SelectPoller(EventLoop *loop)
    : Poller(loop), maxfd_(-1) {
    FD_ZERO(&readfds_);
    FD_ZERO(&writefds_);
    FD_ZERO(&exceptfds_);
}

// channel update => EventLoop updateChannel => Poller updateChannel
void SelectPoller::updateChannel(Channel *channel) {
    const int fd = channel->fd();
    int index = channel->index();
    LOG_INFO("func=%s, fd=%d, event=%d, index=%d \n", __FUNCTION__, channel->fd(), channel->events(), index);

    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
    }

    if (channel->events() & Channel::kReadEvent) {
        FD_SET(fd, &readfds_);
    } else {
        FD_CLR(fd, &readfds_);
    }

    if (channel->events() & Channel::kWriteEvent) {
        FD_SET(fd, &writefds_);
    } else {
        FD_CLR(fd, &writefds_);
    }

    FD_SET(fd, &exceptfds_);

    if (fd > maxfd_) {
        maxfd_ = fd;
    }
}

// 从Poller中删除channel
void SelectPoller::removeChannel(Channel *channel) {
    int fd = channel->fd();
    channels_.erase(fd);

    FD_CLR(fd, &readfds_);
    FD_CLR(fd, &writefds_);
    FD_CLR(fd, &exceptfds_);

    channel->set_index(kNew);

    if (fd == maxfd_) {
        // Recalculate maxfd_
        maxfd_ = -1;
        for (const auto& kv : channels_) {
            if (kv.first > maxfd_) {
                maxfd_ = kv.first;
            }
        }
    }
}

Timestamp SelectPoller::poll(int timeoutMs, ChannelList *activateChannels) {
    LOG_INFO("poll called\n");

    struct timeval timeout;
    timeout.tv_sec = timeoutMs / 1000;
    timeout.tv_usec = (timeoutMs % 1000) * 1000;

    fd_set readfds = readfds_;
    fd_set writefds = writefds_;
    fd_set exceptfds = exceptfds_;

    int numEvents = ::select(maxfd_ + 1, &readfds, &writefds, &exceptfds, &timeout);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0) {
        LOG_INFO("%d events happened \n", numEvents);
        fillActiveChannels(numEvents, activateChannels);
    } else if (numEvents == 0) {
        LOG_DEBUG("%s timeout! \n", __FUNCTION__);
    } else {
        if (saveErrno != EINTR) {
            errno = saveErrno;
            LOG_ERROR("SelectPoller poll error\n");
        }
    }

    return now;
}

void SelectPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
    for (const auto& kv : channels_) {
        Channel *channel = kv.second;
        int fd = channel->fd();
        int revents = 0;

        if (FD_ISSET(fd, &readfds_)) {
            revents |= Channel::kReadEvent;
        }
        if (FD_ISSET(fd, &writefds_)) {
            revents |= Channel::kWriteEvent;
        }
        if (FD_ISSET(fd, &exceptfds_)) {
            revents |= Channel::kNoneEvent;
        }

        if (revents != 0) {
            channel->set_revents(revents);
            activeChannels->push_back(channel);
            if (--numEvents == 0) break;
        }
    }
}
