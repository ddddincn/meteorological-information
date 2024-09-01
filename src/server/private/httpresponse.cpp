#include "httpresponse.h"

const std::unordered_map<int, std::string> HttpResponse::CODE_STATUS = {
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {500, "Internal Server Error"},
};

HttpResponse::HttpResponse() {
    code_ = -1;
    path_ = status_ = headers_ = content_ = "";
    isKeepAlive_ = false;
}

HttpResponse::~HttpResponse() {
}

void HttpResponse::Init(const std::string& path, bool isKeepAlive, int code) {
    code_ = code;
    isKeepAlive_ = isKeepAlive;
    path_ = path;
    type_ = "text/html";
}

void HttpResponse::MakeResponse(Buffer& buff) {
    initResponse_();  // 初始化响应报文

    buff.Append(status_);  // 向缓冲区写入响应报文
    buff.Append(headers_ + "\r\n");
    buff.Append(content_);

    cleanResponse_();  // 清空响应报文
}

void HttpResponse::AddHeader(const std::string& key, const std::string& value) {
    headers_ += key + ": " + value + "\r\n";
}

void HttpResponse::SetContent(const std::string& content) {
    content_ += content;
}

void HttpResponse::SetType(const std::string& type) {
    type_ = type;
}

void HttpResponse::SetCode(const int& code) {
    code_ = code;
}

void HttpResponse::initResponse_() {
    std::string status;
    if (CODE_STATUS.count(code_) == 1) {  // 查看是否存在该状态码
        status = CODE_STATUS.find(code_)->second;
    } else {  // 不存在则400
        code_ = 400;
        status = CODE_STATUS.find(code_)->second;
    }
    status_ = "HTTP/1.1 " + std::to_string(code_) + " " + status + "\r\n";
    headers_ = "Date: " + getGMTTime_() + "\r\n" + "Server: DNetworkDisk\r\n";  // 配置默认响应头
    if (code_ == 400) {
        if (isKeepAlive_) {
            AddHeader("Connection", "close");
        }
        AddHeader("Content-Type", type_);
        AddHeader("Content-Length", "0");
        content_ = "";
    } else {
        AddHeader("Content-Type", type_);
        AddHeader("Content-Length", std::to_string(content_.size()));
    }
}

void HttpResponse::cleanResponse_() {
    status_ = "";
    headers_ = "";
    content_ = "";
}

int HttpResponse::Code() {
    return code_;
}

std::string HttpResponse::getGMTTime_() {
    time_t now = time(nullptr);
    struct tm* gmtTime = gmtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtTime);
    return std::string(buffer);
}
