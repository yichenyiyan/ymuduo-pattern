#ifndef YMUDUO_SELECTPOLLER
#define YMUDUO_SELECTPOLLER

#include <sys/select.h>

#include <vector>

#include "Poller.h"
#include "Timestamp.h"

namespace ymuduo {

class SelectPoller : public Poller {
public:
    SelectPoller(EventLoop *loop);
    ~SelectPoller() override = default;

    Timestamp poll(int timeoutMs, ChannelList *activateChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    
    fd_set readfds_;
    fd_set writefds_;
    fd_set exceptfds_;
    int maxfd_;
};

}

#endif
