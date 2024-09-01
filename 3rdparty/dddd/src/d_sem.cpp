#include "dddd/d_sem.h"

using namespace dddd;

D_Sem::D_Sem() : semid_(-1) {}
D_Sem::~D_Sem() {};

bool D_Sem::init(key_t key, unsigned short value, short sem_flg) {
    if (semid_ != -1)  // 已经初始化，不用再操作
        return true;
    sem_flg_ = sem_flg;
    semid_ = semget(key, 1, 0666);                             // 先尝试获取
    if (semid_ < 0 || errno == ENOENT) {                       // 表明不存在该信号量
        semid_ = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);  // 用IPC_EXCL标志确保只有一个进程创建并初始化信号量，其它进程只能获取。
        if (semid_ < 0 || errno == EEXIST) {                   // 如果报存在错误就再次获取
            semid_ = semget(key, 1, 0666);
            if (semid_ < 0) {
                return false;
            }
        }
    }

    union semun sem_union;
    sem_union.val = value;  // 初始化为value
    if (semctl(semid_, 0, SETVAL, sem_union) < 0)
        return false;

    return true;
}

bool D_Sem::post(short val) {
    if (semid_ < 0)
        return false;
    struct sembuf sem_b;
    sem_b.sem_num = 0;  // 第0个信号量，因为semget()中只要求了一个信号量
    sem_b.sem_op = val;
    sem_b.sem_flg = sem_flg_;
    if (semop(semid_, &sem_b, 1) == -1)
        return false;
    return true;
}

bool D_Sem::wait(short val) {
    if (semid_ < 0)
        return false;
    struct sembuf sem_b;
    sem_b.sem_num = 0;  // 第0个信号量，因为semget()中只要求了一个信号量
    sem_b.sem_op = val;
    sem_b.sem_flg = sem_flg_;
    if (semop(semid_, &sem_b, 1) == -1)
        return false;
    return true;
}

bool D_Sem::destroy() {
    if (semid_ == -1)
        return false;
    if (semctl(semid_, 0, IPC_RMID) == -1)
        return false;
    return true;
}