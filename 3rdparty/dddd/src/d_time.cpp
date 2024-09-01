/*
 * @Author: dddd
 * @Date: 2024-07-16 17:29:55
 * @Description: 请填写简介
 */
#include "dddd/d_time.h"
namespace dddd {
namespace d_time {
std::string get_time(const char* fmt) {
    time_t timer = time(nullptr);
    struct tm* local_tm = std::localtime(&timer);

    char tm[80];
    strftime(tm, sizeof(tm), fmt, local_tm);
    return std::string(tm);
}

time_t strtotime(const std::string time) {
    if (time.length() != 14) return -1;  // 如果时间格式不是yyyymmddhh24miss，说明时间格式不正确。

    std::string yyyy, mm, dd, hh, mi, ss;
    yyyy = time.substr(0, 4);
    mm = time.substr(4, 2);
    dd = time.substr(6, 2);
    hh = time.substr(8, 2);
    mi = time.substr(10, 2);
    ss = time.substr(12, 2);
    struct tm t;

    try {
        t.tm_year = stoi(yyyy) - 1900;
        t.tm_mon = stoi(mm) - 1;
        t.tm_mday = stoi(dd);
        t.tm_hour = stoi(hh);
        t.tm_min = stoi(mi);
        t.tm_sec = stoi(ss);
        t.tm_isdst = 0;
    } catch (const std::exception& e) {
        return -1;
    }

    return mktime(&t);
}

std::string timetostr(const time_t time) {
    time_t t = time;
    std::string ret;

    struct tm* tm_info;

    // 将 time_t 转换为 tm 结构体
    tm_info = localtime(&t);

    // 格式化时间为字符串
    strftime(ret.data(), 14, "%Y%m%d%H%M%S", tm_info);
    return ret;
}

std::string utc_transfer(std::string& time, int flag) {
    struct tm tm;
    time_t t;
    char buffer[16];

    // 解析 UTC 时间字符串
    memset(&tm, 0, sizeof(struct tm));
    strptime(time.data(), "%Y%m%d%H%M%S", &tm);

    // 将 tm 转换为 time_t
    t = mktime(&tm);

    // 增加8小时的秒数
    t += 8 * 3600;

    // 将 time_t 转换回 tm 结构体
    struct tm* local_tm = localtime(&t);

    // 格式化时间为字符串
    strftime(time.data(), 15, "%Y%m%d%H%M%S", local_tm);
    return time;
}
}  // namespace d_time
}  // namespace dddd