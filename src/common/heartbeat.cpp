#include "heartbeat.h"

#include <dddd/d_sem.h>

Heartbeat::Heartbeat() {
}

Heartbeat::~Heartbeat() {
}

bool Heartbeat::init(time_t timeout, key_t key, size_t capacity) {
    // 分配共享内存
    int shmid = shmget(key, capacity * sizeof(struct HeartbeatInfo), 0666 | IPC_CREAT);
    if (shmid < 0) {
        spdlog::info("共享内存分配失败");
        return false;
    }
    shmid_ = shmid;
    timeout_ = timeout;
    shmptr_ = (struct HeartbeatInfo*)shmat(shmid, 0, 0);
    pos_ = 0;
    capacity_ = capacity;

    sem_.init(key);

    if (shmid_ < 0)  // 如果内存没有分配成功
        return false;
    struct HeartbeatInfo heartbeat_info;
    heartbeat_info.pid = getpid();
    heartbeat_info.timeout = timeout_;
    heartbeat_info.last_beat = time(nullptr);
    sem_.wait();  // 因为访问共享内存要上锁
    for (int i = 0; i < capacity_; i++) {
        if (shmptr_[i].pid == 0) {        // 若pid为0，表明当前位置没有内容
            shmptr_[i] = heartbeat_info;  // 将当前进程信息写入共享内存
            sem_.post();
            return true;
        }
    }
    sem_.post();
    return false;
}

bool Heartbeat::beat() {
    if (shmid_ < 0)  // 如果内存没有分配成功
        return false;
    pid_t pid = getpid();
    sem_.wait();
    for (int i = 0; i < capacity_; i++) {
        if (shmptr_[i].pid == pid) {  // 更新当前进程心跳时间
            shmptr_[i].last_beat = time(nullptr);
            sem_.post();
            return true;
        }
    }
    sem_.post();
    return false;
}
