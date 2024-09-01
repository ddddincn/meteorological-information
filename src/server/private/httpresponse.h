#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>  // stat
#include <sys/time.h>
#include <unistd.h>

#include <string>
#include <unordered_map>

#include "buffer.h"

class HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();

    void Init(const std::string& path, bool isKeepAlive, int code);

    void MakeResponse(Buffer& buff);

    void AddHeader(const std::string& key, const std::string& value);
    void SetContent(const std::string& content);
    void SetType(const std::string& type);
    void SetCode(const int& code);

    int Code();

private:
    void initResponse_();
    void cleanResponse_();

    std::string getGMTTime_();

private:
    int code_;          // 状态码
    bool isKeepAlive_;  // 是否保持连接

    std::string path_;  // 请求路由

    std::string status_;   // 状态信息
    std::string headers_;  // 响应头
    std::string type_;     // 响应类型
    std::string content_;  // 响应体

    static const std::unordered_map<int, std::string> CODE_STATUS;
};

#endif  // HTTPRESPONSE_H