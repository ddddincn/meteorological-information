#ifndef D_STRING_H
#define D_STRING_H

#include <string.h>

#include <algorithm>
#include <string>
#include <vector>

namespace dddd {
namespace d_string {

// 除去字符串前后的空格
char* trim(char* str, size_t len);
std::string& trim(std::string& str);

// 替换字符串中的指定的字符
char* replace(char* str, size_t len, const char* search, const char* replace);
std::string& replace(std::string& str, const char* search, const char* replace);

// 将字符串分割，返回vector
std::vector<std::string> split(const char* str, size_t len, const char* delim);
std::vector<std::string> split(std::string& str, const char* delim);

// 将字符串转为大写
char* toupper(char* str);
std::string toupper(std::string& str);

// 将字符串转为小写
char* tolower(char* str);
std::string tolower(std::string& str);

}  // namespace d_string
}  // namespace dddd
#endif  // D_STRING_H
