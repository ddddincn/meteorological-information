#include "httpconn.h"

std::atomic<int> HttpConn::userCount;
bool HttpConn::isET;

std::unordered_map<std::string, std::function<void(HttpRequest *, HttpResponse *)>> HttpConn::ROUTE_MAP;
std::mutex HttpConn::routeMapMtx_;

void HttpConn::ADD_ROUTE(const std::string route, std::function<void(HttpRequest *, HttpResponse *)> fn) {
    std::lock_guard<std::mutex> locker(routeMapMtx_);
    ROUTE_MAP[route] = fn;
}

HttpConn::HttpConn() {
    fd_ = -1;
    addr_ = {0};
    isClose_ = true;
};

HttpConn::~HttpConn() {
    Close();
};

void HttpConn::init(int fd, const sockaddr_in &addr) {
    assert(fd > 0);
    userCount++;
    addr_ = addr;
    fd_ = fd;
    writeBuff_.RetrieveAll();
    readBuff_.RetrieveAll();
    isClose_ = false;
    // LOG_INFO("Client[%d](%s:%d) in, userCount:%d", fd_, GetIP(), GetPort(), (int)userCount);
}

void HttpConn::Close() {
    if (isClose_ == false) {
        isClose_ = true;
        userCount--;
        close(fd_);
        // LOG_INFO("Client[%d](%s:%d) quit, UserCount:%d", fd_, GetIP(), GetPort(), (int)userCount);
    }
}

int HttpConn::GetFd() const {
    return fd_;
};

struct sockaddr_in HttpConn::GetAddr() const {
    return addr_;
}

const char *HttpConn::GetIP() const {
    return inet_ntoa(addr_.sin_addr);
}

int HttpConn::GetPort() const {
    return addr_.sin_port;
}

ssize_t HttpConn::read(int *saveErrno) {
    ssize_t len = -1;
    do {
        len = readBuff_.ReadFd(fd_, saveErrno);
        if (len <= 0) {
            break;
        }
    } while (isET);
    return len;
}

ssize_t HttpConn::write(int *saveErrno) {
    ssize_t len = -1;
    do {
        len = writeBuff_.WriteFd(fd_, saveErrno);
        if (len <= 0) {
            break;
        }
    } while (isET);
    return len;
}

bool HttpConn::process() {
    request_.Init();
    if (readBuff_.ReadableBytes() <= 0) {
        return false;
    } else if (request_.Parse(readBuff_)) {
        // LOG_DEBUG("%s", request_.path().c_str());
        response_.Init(request_.GetPath(), request_.IsKeepAlive(), 200);
        handleRequest_(request_.GetPath());

    } else {
        response_.Init(request_.GetPath(), false, 400);
    }

    response_.MakeResponse(writeBuff_);
    return true;
}

// void HttpConn::AddResponseHeader(const std::string &key, const std::string &value) {
//     response_.AddHeader(key, value);
// }

// void HttpConn::SetResponseContent(const std::string &content) {
//     response_.SetContent(content);
// }

// void HttpConn::SetResponseType(const std::string &type) {
//     response_.SetType(type);
// }

// void HttpConn::SetResponseCode(const int &code) {
//     response_.SetCode(code);
// }

void HttpConn::handleRequest_(std::string path) {
    if (ROUTE_MAP.count(path) == 1) {
        ROUTE_MAP[path](&this->request_, &this->response_);
    } else {
        response_.Init(request_.GetPath(), false, 404);
    }
}