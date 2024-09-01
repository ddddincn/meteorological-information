#include "heartbeat_monitor.h"

HeartbeatMonitor::HeartbeatMonitor() {
}

HeartbeatMonitor::~HeartbeatMonitor() {
    close();
}

bool HeartbeatMonitor::init(size_t capacity, key_t key) {
    spdlog::info("初始化HeartbeatMonitor");
    // 分配共享内存
    int shmid = shmget(key, capacity * sizeof(struct HeartbeatInfo), 0666 | IPC_CREAT);
    if (shmid < 0) {
        spdlog::error("共享内存分配失败");
        return false;
    }
    shmid_ = shmid;
    shmptr_ = (struct HeartbeatInfo*)shmat(shmid, 0, 0);
    pos_ = 0;
    capacity_ = capacity;
    sem_.init(key);
    return true;
}

// 监控启动程序
void HeartbeatMonitor::run() {
    while (true) {
        for (int i = 0; i < capacity_; i++) {
            if (shmptr_[i].pid == 0)  // 如果遇到没有初始化的信息直接跳过
                continue;
            // 先清理
            // spdlog::info("当前进程 pid = {}, timeout = {}, beat = {}", shmptr_[i].pid, shmptr_[i].timeout, shmptr_[i].last_beat);
            if (kill(shmptr_[i].pid, 0) == -1) {  // 如果该进程已经死掉就先清理
                spdlog::info("清理已结束进程 pid = {}", shmptr_[i].pid);
                sem_.wait();
                memset(&shmptr_[i], 0, sizeof(struct HeartbeatInfo));
                sem_.post();
                continue;
            }

            time_t now = time(nullptr);                            // 获取当前时间
            if (now - shmptr_[i].last_beat <= shmptr_[i].timeout)  // 如果该进程没有超时直接跳过
                continue;
            else {  // 如果已经超时
                spdlog::info("进程 pid = {}，已经超时", shmptr_[i].pid);
                if (kill(shmptr_[i].pid, 0) == -1) {  // 先判断是否存活，如果已经死了就替换信息
                    sem_.wait();
                    memset(&shmptr_[i], 0, sizeof(struct HeartbeatInfo));
                    sem_.post();
                    continue;
                } else {  // 如果存活就先发送信号让他正常停止
                    spdlog::info("尝试停止进程 pid = {}", shmptr_[i].pid);
                    kill(shmptr_[i].pid, 15);
                    if (kill(shmptr_[i].pid, 0) != -1)          // 如果没有立即退出
                        for (int i = 1; i < 5; i++) {           // 给这个进程5秒钟时间正常关闭
                            if (kill(shmptr_[i].pid, 0) == -1)  // 如果进程已经结束了就退出
                                break;
                            sleep(1);
                        }
                    if (kill(shmptr_[i].pid, 0) == -1) {  // 再次判断程序是否正常停止
                        sem_.wait();
                        memset(&shmptr_[i], 0, sizeof(struct HeartbeatInfo));
                        sem_.post();
                    } else {  // 如果没有正常终止就强制杀死程序
                        spdlog::info("强制停止进程 pid = {}", shmptr_[i].pid);
                        kill(shmptr_[i].pid, 9);
                        sem_.wait();
                        memset(&shmptr_[i], 0, sizeof(struct HeartbeatInfo));
                        sem_.post();
                    }
                }
            }
        }
    }
    sleep(100);
}

// 清理共享内存，在监控程序中调用
void HeartbeatMonitor::close() {
    shmdt(shmptr_);                     // 先脱离内存
    shmctl(shmid_, IPC_RMID, nullptr);  // 清理内存
    sem_.destroy();                     // 清理信号量
    spdlog::info("监控程序已退出");
}