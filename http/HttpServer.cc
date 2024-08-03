#include <any>

#include "Logger.h"
#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace ymuduo {

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
    resp->setStatusCode(HttpResponse::kNotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}


HttpServer::HttpServer(EventLoop* loop, const InetAddress& listenAddr,
        const std::string& name, TcpServer::Option option)
    : server_(std::make_unique<TcpServer>(loop, listenAddr, name)), httpCallback_(defaultHttpCallback) {
    
    server_->setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_->setMessageCallback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void HttpServer::start() {
    LOG_INFO("HttpServer %s starts listening on %s", server_->name().c_str(), server_->ipPort().c_str());
    server_->start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime) {
    HttpContext* context = std::any_cast<HttpContext>(conn->getMutableContext());

    if (!context->parseRequest(buf, receiveTime)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll()) {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req) {
    const std::string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
        (req.getVersion() == HttpRequest::kHttpV10 && connection != "Keep-Alive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(std::string(buf.peek(), buf.readableBytes()));
    if (response.closeConnection()) {
        conn->shutdown();
    }
}

}