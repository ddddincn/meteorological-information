#include "httprequest.h"

HttpRequest::HttpRequest() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    headers_.clear();
    parameters_.clear();
}

HttpRequest::~HttpRequest() = default;

void HttpRequest::Init() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    headers_.clear();
    parameters_.clear();
}

bool HttpRequest::Parse(Buffer& buff) {
    const char CRLF[3] = "\r\n";
    if (buff.ReadableBytes() <= 0) {
        return false;
    }
    while (buff.ReadableBytes() > 0 && state_ != FINISH) {
        const char* crlf = std::search(buff.Peek(), buff.Peek() + buff.ReadableBytes(), CRLF, CRLF + 2);  // 查找\r\n
        std::string line(buff.Peek(), crlf);                                                              // 将\r\n之间的字符串赋值给line
        switch (state_) {
            case REQUEST_LINE:
                parseLine_(line);
                break;
            case HEADERS:
                parseHeader_(line);
                if (buff.ReadableBytes() <= 2) {
                    state_ = FINISH;
                }
                break;
            case BODY:
                parseBody_(line);
                break;
            default:
                break;
        }
        if (crlf == buff.BeginWrite()) {
            break;
        }
        buff.Retrieve(line.length() + 2);
    }
    return true;
}

void HttpRequest::parseLine_(const std::string& line) {
    std::regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");  // 匹配HTTP请求行
    std::smatch subMatch;                                  // 匹配结果
    if (std::regex_match(line, subMatch, pattern)) {
        method_ = subMatch[1];
        std::string url = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;

        // 提取路径和查询参数
        std::regex pathPattern("([^?]*)(\\?([^#]*))?");
        std::smatch pathMatch;
        if (std::regex_match(url, pathMatch, pathPattern)) {
            path_ = pathMatch[1];                    // 提取路径部分
            std::string queryString = pathMatch[3];  // 提取查询字符串部分

            // 解析查询参数
            if (!queryString.empty()) {
                std::regex queryPattern("([^&=]+)=([^&]*)");
                auto queryBegin = std::sregex_iterator(queryString.begin(), queryString.end(), queryPattern);
                auto queryEnd = std::sregex_iterator();

                for (std::sregex_iterator i = queryBegin; i != queryEnd; ++i) {
                    std::smatch queryMatch = *i;
                    std::string key = queryMatch[1];
                    std::string value = queryMatch[2];
                    parameters_[key] = value;  // 存储参数键值对
                }
            }
        }
    }
}

void HttpRequest::parseHeader_(const std::string& line) {
    std::regex pattern("([^:]*): (.*)");  // 匹配头部信息
    std::smatch subMatch;
    if (std::regex_match(line, subMatch, pattern)) {
        headers_[subMatch[1]] = subMatch[2];  // 将头部信息保存到headers_中
    } else {
        state_ = BODY;  // 当匹配不成功时，说明headers以及匹配完成
    }
}

void HttpRequest::parseBody_(const std::string& line) {
    if (method_ == "POST" && headers_["Content-Type"] == "application/x-www-form-urlencoded") {  // 如果POST请求带body需要解析body
        parseFromUrlencoded_(line);
    }
    body_ = line;
    state_ = FINISH;
}

void HttpRequest::parseFromUrlencoded_(const std::string& line) {
    if (line.size() == 0) {
        return;
    }
    std::string l = line;
    std::string key, value;
    std::string::size_type i = 0, j = 0;
    int num = 0;

    for (; i < l.size(); i++) {
        char ch = l[i];
        switch (ch) {
            case '=':
                key = l.substr(j, i - j);
                j = i + 1;
                break;
            case '+':
                l[i] = ' ';
                break;
            case '%':
                num = convertHex_(l[i + 1]) * 16 + convertHex_(l[i + 2]);
                l[i + 2] = num % 10 + '0';
                l[i + 1] = num / 10 + '0';
                i += 2;
                break;
            case '&':
                value = l.substr(j, i - j);
                j = i + 1;
                parameters_[key] = value;
                break;
            default:
                break;
        }
    }
    assert(j <= i);
    if (parameters_.count(key) == 0 && j < i) {
        value = l.substr(j, i - j);
        parameters_[key] = value;
    }
}

int HttpRequest::convertHex_(char ch) {
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    return ch;
}

std::string HttpRequest::GetPath() {
    return path_;
}

bool HttpRequest::IsKeepAlive() {
    if (headers_.count("Connection") == 1) {
        return headers_["Connection"] == "keep-alive";
    }
    return false;
}

std::unordered_map<std::string, std::string> HttpRequest::GetParameters() {
    return parameters_;
}
