/*
 * @Author: dddd
 * @Date: 2024-07-17 10:06:07
 * @Description: 请填写简介
 */
#include "dddd/d_file.h"

#include <iostream>
namespace dddd {
namespace d_file {
// 以写方式打开文件，若不存在则创建目录和文件
bool open(std::fstream& fstream, const std::string& file) {
    size_t pos = file.find_last_of("/");
    std::string path;
    if (pos != std::string::npos) {
        path = file.substr(0, pos + 1);
        if (!std::filesystem::exists(path)) {                  // 判断是否存在该路径
            if (!std::filesystem::create_directories(path)) {  // 不存在则创建目录
                return false;
            } else {  // 存在则打开文件
                fstream.open(file, std::ios::out);
                if (!fstream.is_open()) {
                    return false;
                }
            }
        } else {  // 存在路径直接打开文件
            fstream.open(file, std::ios::out);
            if (!fstream.is_open()) {
                return false;
            }
        }
    } else {
        fstream.open(file, std::ios::out);
        if (!fstream.is_open()) {
            return false;
        }
    }
    return true;
}
// 新建文件夹
bool mkdir(const std::string& path) {
    if (!std::filesystem::exists(path)) {                  // 判断是否存在该路径
        if (!std::filesystem::create_directories(path)) {  // 不存在则创建目录
            return false;
        }
    }
    return true;
}
// 修改文件修改时间
bool set_modification_time(const std::string& filename, const std::string& time) {
    struct utimbuf buf;

    buf.actime = buf.modtime = dddd::d_time::strtotime(time);

    if (utime(filename.c_str(), &buf) != 0) return false;

    return true;
}
// 获取文件大小
size_t filesize(const std::string& filename) {
    struct stat st_filestat;  // 存放文件信息的结构体。

    // 获取文件信息，存放在结构体中。
    if (stat(filename.c_str(), &st_filestat) < 0) return -1;

    return st_filestat.st_size;  // 返回结构体的文件大小成员。
}
// 获取文件修改时间
std::string file_modification_time(const std::string& filename) {
    struct stat st_filestat;  // 存放文件信息的结构体。

    // 获取文件信息，存放在结构体中。
    if (stat(filename.c_str(), &st_filestat) < 0) return "";

    // 把整数表示的时间转换成字符串表示的时间。
    std::string time = dddd::d_time::timetostr(st_filestat.st_mtime);

    return time;
}
}  // namespace d_file
}  // namespace dddd