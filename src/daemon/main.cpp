#include <dddd/d_sem.h>
#include <dddd/d_signal.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include "process.h"

void handle_exit(int signal) {
    exit(signal);
}

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        std::cerr << "example: /daemon timeout started_program" << std::endl;
        std::cerr << "example: /daemon 3 ls -l" << std::endl;
        return -1;
    }
    // 获取参数
    int timeout = atoi(argv[1]);
    // 拼接需要运行程序参数
    std::string command;
    for (int i = 2; i < argc; i++) {
        command += argv[i];
        if (i != argc - 1)
            command += " ";
    }
    Process p(timeout, command.c_str());
    auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("./logs/monitor.log", 0, 0);
    auto logger = std::make_shared<spdlog::logger>("monitor", sink);
    dddd::d_signal::ignore_all_signal();
    signal(SIGILL, handle_exit);
    signal(SIGTERM, handle_exit);

    if (fork() != 0) exit(0);  // 关闭父进程
    dddd::D_Sem sem;
    sem.init(2024);

    while (true) {
        if (!p.is_running()) {
            if (p.restart()) {
                sem.wait();
                logger->info("have restarted program \"{}\" timeout: {}", command, timeout);
                sem.post();
            } else {
                sem.wait();
                logger->error("restart program failed \"{}\" timeout: {}", command, timeout);
                sem.post();
            }
        }
        sleep(p.get_timeout());
    }
}
