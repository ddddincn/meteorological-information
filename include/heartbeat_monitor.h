/*
 * @Author: dddd
 * @Date: 2024-07-25 15:27:48
 * @Description: heartbeat头文件，使用共享内存存放进程信息，更新心跳或重启
 */
#ifndef HEARTBEAT_MONITOR_H
#define HEARTBEAT_MONITOR_H

#include <dddd/d_sem.h>
#include <signal.h>
#include <spdlog/spdlog.h>
#include <sys/shm.h>

class HeartbeatMonitor {
public:
    HeartbeatMonitor();
    ~HeartbeatMonitor();

    bool init(size_t capacity = 1000, key_t key = 0x6862);  // 默认key为hb的十六进制
    // 监控启动程序
    void run();
    // 清理共享内存，在监控程序中调用
    void close();

private:
    struct HeartbeatInfo {
        int pid;           // 进程id
        time_t timeout;    // 超时
        time_t last_beat;  // 最后一次心跳时间
    };

    int shmid_;                     // 共享内存id，在构造函数中获取
    struct HeartbeatInfo* shmptr_;  // 共享内存指针，构造函数中获取
    int pos_;                       // 当前进程中，共享内存指针的位置
    size_t capacity_;
    dddd::D_Sem sem_;
};

#endif  // HEARTBEAT_MONITOR_H