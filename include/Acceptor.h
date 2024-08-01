#ifndef MUDUO_ACCEPTOR
#define MUDUO_ACCEPTOR

#include "Socket.h"
#include "Channel.h"
#include "noncopyable.h"


namespace ymuduo
{

class EventLoop;
class InetAddress;

class Acceptor : public noncopyable {
public:
    using NewConnectionCallback = std::function<void(int, const InetAddress&)>;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb) { 
        newConnectionCallback_ = cb; 
    }

    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

    EventLoop *loop_; // Acceptor用的loop是用户创建的base loop
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idleFd_; //用来解决文件描述符枯竭的情况
};


}


#endif