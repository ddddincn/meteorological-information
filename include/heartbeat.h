/*
 * @Author: dddd
 * @Date: 2024-07-25 17:37:44
 * @Description: heartbeat头文件，该类配合heartbeat_monitor使用，类中使用的信号量和共享内存由heartbeat_monitor回收
 */

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <dddd/d_sem.h>
#include <signal.h>
#include <spdlog/spdlog.h>
#include <sys/shm.h>

class Heartbeat {
public:
    Heartbeat();
    // 需要在共享内存中删除当前进程的信息记录
    ~Heartbeat();
    // 将当前进程信息存入共享内存
    bool init(time_t timeout = 3, key_t key = 0x6862, size_t capacity = 1000);  // 默认key为hb的十六进制
    // 更新共享内存中心跳时间
    bool beat();

private:
    struct HeartbeatInfo {
        int pid;           // 进程id
        time_t timeout;    // 超时
        time_t last_beat;  // 最后一次心跳时间
    };

    int shmid_;                     // 共享内存id，在构造函数中获取
    struct HeartbeatInfo* shmptr_;  // 共享内存指针，构造函数中获取
    int pos_;                       // 当前进程中，共享内存指针的位置
    time_t timeout_;                // 心跳间隔
    size_t capacity_;               // 共享内存大小

    dddd::D_Sem sem_;  // 用于多进程的信号量
};

#endif  // HEARTBEAT_H