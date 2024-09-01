#include <dddd/d_ftp.h>
#include <dddd/d_signal.h>
#include <dddd/d_string.h>
#include <spdlog/spdlog.h>

#include <iostream>

void exit_signal(int signum) {
    exit(signum);
}

int main(int argc, char const* argv[]) {
    // ./bin/ftp_get host:port username password /remote/filepath /local/filepath mode
    if (argc != 7) {
        std::cerr << "./bin/ftp_get host:port username password /remote/filepath /local/filepath mode" << std::endl;
        return -1;
    }
    // 处理退出信号
    dddd::d_signal::ignore_all_signal();
    signal(SIGINT, exit_signal);
    signal(SIGTERM, exit_signal);
    // 处理参数
    std::string host, username, password, remote_filepath, local_filepath, mode;
    host = argv[1], username = argv[2], password = argv[3], remote_filepath = argv[4], local_filepath = argv[5], mode = argv[6];
    if (mode != "2")
        mode = "1";
    spdlog::info("host: {}, username: {}, password: {}, remote_filepath: {}, local_filepath: {}, mode: {}",
                 host, username, password, remote_filepath, local_filepath, mode);
    // ftp
    dddd::D_FtpClient ftp_client;
    if (!ftp_client.login(host, username, password, atoi(mode.data()))) {
        spdlog::info("fto login failed");
        return -1;
    }
    std::string list = ftp_client.nlist(remote_filepath);
    if (list == "") {
        spdlog::info("no file to get");
        return -1;
    }
    std::vector<std::string> lists = dddd::d_string::split(list, "\n");
    for (auto file : lists) {
        if (file != "") {
            std::string file_name = file;
            std::vector<std::string> temp_vec = dddd::d_string::split(file_name, "/");
            std::string local_file_name = local_filepath + "/" + temp_vec[temp_vec.size() - 1];
            if (ftp_client.get(file, local_file_name))
                spdlog::info("{} download success", file);
            else
                spdlog::info("{} download failed", file);
        }
    }
    return 0;
}
