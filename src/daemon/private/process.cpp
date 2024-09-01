/*
 * @Author: dddd
 * @Date: 2024-07-23 22:27:26
 * @Description: process类实现
 */
#include "process.h"

Process::Process(struct ProcessInfo& process_info) {
    process_info_ = process_info;
}

Process::Process(const char* pname, time_t timeout, const char* command) {
    strncpy(process_info_.pname_, pname, sizeof(process_info_.pname_));
    process_info_.timeout_ = timeout;
    strncpy(process_info_.command_, command, sizeof(process_info_.command_));
}

Process::Process(time_t timeout, const char* command) {
    process_info_.timeout_ = timeout;
    strncpy(process_info_.command_, command, sizeof(process_info_.command_));
}
Process::~Process() {}

pid_t Process::get_pid() {
    return process_info_.pid_;
}
std::string Process::get_pname() {
    return process_info_.pname_;
}
time_t Process::get_timeout() {
    return process_info_.timeout_;
}
time_t Process::get_last_beat() {
    return process_info_.last_beat_;
}
struct ProcessInfo Process::get_process_info() {
    return process_info_;
}

void Process::set_pid(pid_t pid) {
    process_info_.pid_ = pid;
}
void Process::set_pname(const char* pname) {
    strncpy(process_info_.pname_, pname, sizeof(process_info_.pname_));
}
void Process::set_timeout(time_t timeout) {
    process_info_.timeout_ = timeout;
}
void Process::set_last_beat(time_t last_beat) {
    process_info_.last_beat_ = last_beat;
}

bool Process::is_running() {
    if (process_info_.pid_ != -1 && ::kill(process_info_.pid_, 0) == 0)
        return true;
    else
        return false;
}

void Process::kill() {
    if (is_running()) {
        if (!::kill(process_info_.pid_, SIGKILL))
            process_info_.pid_ == -1;
    }
}
bool Process::restart() {
    if (!is_running()) {
        std::vector<std::string> command;
        std::string path;
        std::vector<const char*> args;
        if (process_info_.command_ != nullptr) {
            command = dddd::d_string::split(process_info_.command_, sizeof(process_info_.command_), " ");
            path = command[0];
            for (const auto& c : command) {
                args.push_back(c.c_str());
            }
            args.push_back(nullptr);
        }

        // 创建新的子进程
        pid_t pid = fork();
        if (pid == 0) {
            execvp(path.c_str(), (char* const*)args.data());
            _exit(1);
        } else if (pid > 0) {
            process_info_.pid_ = pid;                 // 更新进程id
            waitpid(process_info_.pid_, nullptr, 0);  // 等待子进程完成
            return true;
        } else {
            return false;
        }
    }
    return true;
}
