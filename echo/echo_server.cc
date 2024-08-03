#include <string>
#include <memory>
#include <iostream>

#include "Logger.h"
#include "Buffer.h"
#include "TcpServer.h"
#include "AsyncLogging.h"

using namespace ymuduo;

off_t kRollSize = 500 * 1000 * 1000;

AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len) {
  g_asyncLog->append(msg, len);
}

class EchoServer {
public:
    EchoServer(EventLoop* loop, const InetAddress& addr,
                const std::string& name)
        : loop_(loop), server_(std::make_unique<TcpServer>(loop, addr, name)) {
        server_->setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        server_->setMessageCallback(
            std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)    
        );
        server_->setThreadNum(0);
    }
    ~EchoServer() {}

    void setThreadNum(int threadNum) {
        server_->setThreadNum(threadNum);
    }

    void start() {
        server_->start();
    }

private:

    void onConnection(const TcpConnectionPtr &conn) {
        if (conn->connected()) {
            LOG_INFO("conn UP : %s", conn->peerAddress().toIpPort().c_str());
        } else {
            LOG_INFO("conn DOWN : %s", conn->peerAddress().toIpPort().c_str());
            conn->shutdown();
        }

    }

    void onMessage(const TcpConnectionPtr& conn,
                Buffer *buffer, Timestamp time) {
        std::string msg = buffer->retrieveAllAsString();
        conn->send(msg);
        // conn->shutdown();
    }

    EventLoop* loop_;
    std::unique_ptr<TcpServer> server_;
};

int main(int argc, char** argv) {
    EventLoop* loop = new EventLoop();
    InetAddress addr("0.0.0.0", 8000);
    EchoServer* server = new EchoServer(loop, addr, "EchoServer-01");
    server->start();
    loop->loop();

    delete server;
    delete loop;
    return 0;
}   