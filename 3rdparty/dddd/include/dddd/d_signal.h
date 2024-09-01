#ifndef D_SIGNAL_H
#define D_SIGNAL_H

#include <signal.h>

#include <vector>

namespace dddd {
namespace d_signal {

// 忽略所有信号
void ignore_all_signal();

}  // namespace d_signal
}  // namespace dddd

#endif  // D_SIGNAL_H