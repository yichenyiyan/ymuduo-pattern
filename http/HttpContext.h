#ifndef YMUDUO_HTTPCONTEXT
#define YMUDUO_HTTPCONTEXT

#include "HttpRequest.h"

namespace ymuduo {

class Buffer;

class HttpContext {
public:
    enum HttpRequestParseState : int {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpContext() : state_(kExpectRequestLine) {}

    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    bool gotAll() const { return state_ == kGotAll; }

    void reset() {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    const HttpRequest& request() const{ return request_; }

    HttpRequest& request() { return request_; }

private:
    bool processRequestLine(const char* begin, const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};

}

#endif 
