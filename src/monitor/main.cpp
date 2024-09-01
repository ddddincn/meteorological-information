#include <dddd/d_signal.h>
#include <spdlog/sinks/daily_file_sink.h>

#include "heartbeat_monitor.h"

HeartbeatMonitor monitor;

void term_handler(int signum) {
    monitor.close();
    exit(signum);
}

int main(int argc, char const *argv[]) {
    auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("./logs/monitor.log", 0, 0);
    auto logger = std::make_shared<spdlog::logger>("monitor", sink);
    spdlog::set_default_logger(logger);
    dddd::d_signal::ignore_all_signal();
    signal(SIGTERM, term_handler);
    signal(SIGINT, term_handler);
    monitor.init();
    monitor.run();
    return 0;
}
