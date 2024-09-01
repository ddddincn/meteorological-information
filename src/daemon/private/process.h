/*
 * @Author: dddd
 * @Date: 2024-07-23 22:27:20
 * @Description: 进程信息类
 */
#ifndef PROCESS_H
#define PROCESS_H

#include <dddd/d_string.h>
#include <dirent.h>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>
// 进程信息结构体
struct ProcessInfo {
    int pid_ = -1;           // 进程id
    time_t timeout_ = 0;     // 超时
    time_t last_beat_ = -1;  // 最后一次心跳时间
    char pname_[256];        // 进程名
    char command_[256];      // 执行该程序的命令
};

class Process {
public:
    Process(struct ProcessInfo& process_info);
    Process(const char* pname, time_t timeout = 60, const char* command = "");
    Process(time_t timeout = 60, const char* command = "");
    ~Process();

    pid_t get_pid();
    std::string get_pname();
    time_t get_timeout();
    time_t get_last_beat();
    struct ProcessInfo get_process_info();

    void set_pid(pid_t pid);
    void set_pname(const char* pname);
    void set_timeout(time_t timeout);
    void set_last_beat(time_t last_beat);
    // 该进程是否正在运行
    bool is_running();
    // 杀死该进程
    void kill();
    // 通过command_重启该进程
    bool restart();

private:
    struct ProcessInfo process_info_;
};

#endif  // PROCESS_H