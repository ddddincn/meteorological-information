/*
 * @Author: dddd
 * @Date: 2024-07-23 21:17:05
 * @Description: 请填写简介
 */
#include "dddd/d_signal.h"

namespace dddd {
namespace d_signal {

// 忽略所有信号
void ignore_all_signal() {
    std::vector<int> signals = {
        SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT,
        SIGBUS, SIGFPE, SIGKILL, SIGUSR1, SIGSEGV, SIGUSR2,
        SIGPIPE, SIGALRM, SIGTERM, SIGCHLD, SIGCONT, SIGSTOP,
        SIGTSTP, SIGTTIN, SIGTTOU, SIGURG, SIGXCPU, SIGXFSZ,
        SIGVTALRM, SIGPROF, SIGWINCH, SIGIO, SIGPWR, SIGSYS};

    for (int signum : signals) {
        signal(signum, SIG_IGN);
    }
}
}  // namespace d_signal
}  // namespace dddd