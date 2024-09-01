#ifndef D_FILE_H
#define D_FILE_H

#include <dddd/d_time.h>
#include <sys/stat.h>
#include <utime.h>

#include <filesystem>
#include <fstream>

namespace dddd {
namespace d_file {
// 以写方式打开文件，若不存在则创建目录和文件
bool open(std::fstream& fstream, const std::string& file);
// 新建文件夹
bool mkdir(const std::string& path);
// 修改文件修改时间
bool set_modification_time(const std::string& filename, const std::string& time);
// 获取文件大小
size_t filesize(const std::string& filename);
// 获取文件修改时间
std::string file_modification_time(const std::string& filename);
}  // namespace d_file
}  // namespace dddd
#endif  // D_FILE_H