#include "dddd/d_string.h"

namespace dddd {
namespace d_string {
// 除去字符串中前后的空格
char* trim(char* str, size_t len) {
    if (str == nullptr)
        return nullptr;
    if (str[0] == ' ') {
        for (int i = 0; i < len - 2; i++) {
            str[i] = str[i + 1];
        }
        str[len - 1] = '\0';
    }
    if (str[len - 1] == ' ')
        str[len - 1] = '\0';
    return str;
}
std::string& trim(std::string& str) {
    if (!str.empty()) {
        str.erase(0, str.find_first_not_of(" "));
        str.erase(str.find_last_not_of(" ") + 1);
    }
    return str;
}

// 替换字符串中的指定的字符
char* replace(char* str, size_t len, const char* search, const char* replace) {
    if (str == nullptr || search == nullptr || replace == nullptr)
        return str;

    size_t search_len = strlen(search);
    size_t replace_len = strlen(replace);

    // 创建缓存用于存储修改后的字符串
    char* buffer = (char*)malloc(len * (replace_len > search_len ? replace_len : search_len) + 1);
    if (buffer == nullptr)
        return nullptr;

    size_t buffer_index = 0;
    for (size_t i = 0; i < len; i++) {
        if (strncmp(&str[i], search, search_len) == 0) {
            strncpy(&buffer[buffer_index], replace, replace_len);
            buffer_index += replace_len;
            i += search_len - 1;
        } else {
            buffer[buffer_index] = str[i];
            buffer_index++;
        }
    }
    buffer[buffer_index] = '\0';
    strncpy(str, buffer, len + 1);
    free(buffer);

    return str;
}
std::string& replace(std::string& str, const char* search, const char* replace) {
    if (str.length() == 0 || search == nullptr || replace == nullptr)
        return str;

    size_t search_len = strlen(search);
    size_t replace_len = strlen(replace);

    size_t pos = 0;
    while ((pos = str.find(search, pos)) != std::string::npos) {
        str.replace(pos, search_len, replace);
        pos += replace_len;
    }

    return str;
}

// 将字符串分割，返回vector
std::vector<std::string> split(const char* str, size_t len, const char* delim) {
    if (str == nullptr || delim == nullptr || len == 0)
        return {};

    size_t delim_len = strlen(delim);
    std::vector<std::string> ret;
    const char* start = str;
    const char* end = str + len + 1;

    while (start < end) {
        const char* pos = std::search(start, end, delim, delim + delim_len);

        if (pos != end) {
            ret.emplace_back(std::string(start, pos));
            start = pos + delim_len;
        } else {
            ret.emplace_back(std::string(start));
            break;
        }
    }

    return ret;
}
std::vector<std::string> split(std::string& str, const char* delim) {
    if (str.length() == 0 || delim == nullptr)
        return {};
    size_t delim_len = strlen(delim);
    std::vector<std::string> ret;
    const char* start = str.data();
    const char* end = str.data() + str.length() + 1;

    while (start < end) {
        const char* pos = std::search(start, end, delim, delim + delim_len);

        if (pos != end) {
            ret.emplace_back(std::string(start, pos));
            start = pos + delim_len;
        } else {
            ret.emplace_back(std::string(start));
            break;
        }
    }

    return ret;
}

// 将字符串转为大写
char* toupper(char* str) {
    std::transform(str, str + strlen(str), str, ::toupper);
    return str;
}
std::string toupper(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// 将字符串转为小写
char* tolower(char* str) {
    std::transform(str, str + strlen(str), str, ::tolower);
    return str;
}
std::string tolower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

}  // namespace d_string
}  // namespace dddd