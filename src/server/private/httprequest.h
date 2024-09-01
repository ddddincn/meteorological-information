#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <algorithm>
#include <map>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "buffer.h"

class HttpRequest {
public:
    enum PARSE_STATE {  // 记录解析状态
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };
    enum HTTP_CODE {  // 记录请求状态
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

public:
    HttpRequest();
    ~HttpRequest();
    void Init();
    bool Parse(Buffer& buffer);

    std::unordered_map<std::string, std::string> GetParameters();
    std::string GetPath();
    bool IsKeepAlive();

private:
    static int convertHex_(char ch);  // 将十六进制字符转换为十进制数字

    void parseLine_(const std::string& line);            // 解析请求行
    void parseHeader_(const std::string& line);          // 解析请求头
    void parseBody_(const std::string& line);            // 解析请求体
    void parseFromUrlencoded_(const std::string& line);  // 解析表单

private:
    PARSE_STATE state_;                                        // 解析状态
    std::string method_;                                       // 请求方法
    std::string path_;                                         // 请求路由
    std::string version_;                                      // 请求版本
    std::string body_;                                         // 请求体
    std::unordered_map<std::string, std::string> headers_;     // 请求头
    std::unordered_map<std::string, std::string> parameters_;  // 请求参数

    // static const std::unordered_set<std::string> DEFAULT_HTML;              // html路由
    // static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;     // 请求路由
    // static const std::unordered_map<std::string, int> DEFAULT_HTML_METHOD;  // 请求方法
    // static const std::unordered_set<std::string> SUFFIX_TYPE;               // 资源文件后缀
};

#endif  // HTTPREQUEST_H