#ifndef D_SEM_H
#define D_SEM_H
#include <errno.h>
#include <sys/sem.h>

namespace dddd {
class D_Sem {
public:
    D_Sem();
    ~D_Sem();

    D_Sem(const D_Sem &) = delete;             // 禁用拷贝构造
    D_Sem &operator=(const D_Sem &) = delete;  // 禁用赋值

    // 初始化
    bool init(key_t key, unsigned short value = 1, short sem_flg = SEM_UNDO);
    // p操作
    bool post(short val = 1);
    // v操作
    bool wait(short val = -1);

    bool destroy();

private:
    union semun {
        int val;                   /* value for cmd SETVAL */
        struct semid_ds *buf;      /* buffer for IPC_STAT & IPC_SET */
        unsigned short int *array; /* array for GETALL & SETALL */
        struct seminfo *__buf;     /* buffer for IPC_INFO */
    };

    int semid_;      // 信号量id
    short sem_flg_;  // 操作符
};
}  // namespace dddd
#endif  // D_SEM_H
