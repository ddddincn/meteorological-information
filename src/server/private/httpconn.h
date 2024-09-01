#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <arpa/inet.h>  // sockaddr_in
#include <errno.h>
#include <stdlib.h>  // atoi()
#include <sys/types.h>
#include <sys/uio.h>  // readv/writev

#include "buffer.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "sqlconnRAII.h"

class HttpConn {
public:
    HttpConn();

    ~HttpConn();

    void init(int sockFd, const sockaddr_in &addr);

    ssize_t read(int *saveErrno);

    ssize_t write(int *saveErrno);

    void Close();

    int GetFd() const;

    int GetPort() const;

    const char *GetIP() const;

    sockaddr_in GetAddr() const;

    bool process();

    int ToWriteBytes() {
        return writeBuff_.ReadableBytes();
    }

    bool IsKeepAlive() {
        return request_.IsKeepAlive();
    }

    // void AddResponseHeader(const std::string &key, const std::string &value);
    // void SetResponseContent(const std::string &content);
    // void SetResponseType(const std::string &type);
    // void SetResponseCode(const int &code);

    static bool isET;

    static std::atomic<int> userCount;

    static void ADD_ROUTE(std::string route, std::function<void(HttpRequest *, HttpResponse *)> fn);

private:
    int fd_;
    struct sockaddr_in addr_;

    bool isClose_;

    Buffer readBuff_;   // 读缓冲区
    Buffer writeBuff_;  // 写缓冲区

    HttpRequest request_;
    HttpResponse response_;

    static std::unordered_map<std::string, std::function<void(HttpRequest *, HttpResponse *)>> ROUTE_MAP;
    static std::mutex routeMapMtx_;
    void handleRequest_(std::string path);
};

#endif  // HTTP_CONN_H