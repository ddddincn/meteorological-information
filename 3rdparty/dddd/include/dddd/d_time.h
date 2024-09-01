#ifndef D_TIME_H
#define D_TIME_H

#include <string.h>
#include <time.h>

#include <chrono>
#include <string>

namespace dddd {
namespace d_time {

// 获取当前时间
std::string get_time(const char* fmt);
time_t strtotime(const std::string time);
std::string timetostr(const time_t time);
std::string utc_transfer(std::string& time, int flag);
}  // namespace d_time
}  // namespace dddd

#endif  // D_TIME_H