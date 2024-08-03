#include <map>
#include <iostream>

#include "Logger.h"
#include "Favicon.h"
#include "EventLoop.h"
#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace ymuduo;

extern char favicon[555];
static bool benchmark = true;

void yTestOnRequest(const HttpRequest& req, HttpResponse* resp) {
    std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;
    if (!benchmark) {
        const std::map<std::string, std::string>& headers = req.headers();
        for (const auto& header : headers) {
        std::cout << header.first << ": " << header.second << std::endl;
        }
    }

    if (req.path() == "/") {
        resp->setStatusCode(HttpResponse::kOk);
        resp->setStatusMessage("OK");
        resp->setContentType("text/html");
        resp->addHeader("Server", "Muduo");
        std::string now = Timestamp::now().toFormattedString();
        resp->setBody("<html><head><title>yichen-http-server</title></head>"
            "<body><h1>Hello</h1>Now is " + now + "</body></html>");
    } else if (req.path() == "/favicon.ico") {
        resp->setStatusCode(HttpResponse::kOk);
        resp->setStatusMessage("OK");
        resp->setContentType("image/png");
        resp->setBody(std::string(favicon, sizeof(favicon)));
    } else if (req.path() == "/hello") {
        resp->setStatusCode(HttpResponse::kOk);
        resp->setStatusMessage("OK");
        resp->setContentType("text/plain");
        resp->addHeader("Server", "yichenMuduo");
        resp->setBody("hello, world from yichen http server!\n");
    } else {
        resp->setStatusCode(HttpResponse::kNotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }
}

int main(int argc, char* argv[]) {
    int numThreads = 0;
    if (argc > 1) {
        benchmark = true;
        numThreads = atoi(argv[1]);
    }
    EventLoop loop;
    std::unique_ptr<HttpServer> server = std::make_unique<HttpServer>(&loop, InetAddress(8000), "yichen-http-server");
    server->setHttpCallback(yTestOnRequest);
    server->setThreadNum(numThreads);
    server->start();
    loop.loop();

    return 0;
}

