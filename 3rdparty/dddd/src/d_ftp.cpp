#include <dddd/d_ftp.h>
#include <unistd.h>

#include <iostream>

using namespace dddd;

D_FtpClient::D_FtpClient() {
    ftpconn_ = 0;

    init();

    FtpInit();

    connection_status = false;
    login_status = false;
    option_status = false;
}

D_FtpClient::~D_FtpClient() {
    logout();
}

void D_FtpClient::init() {
    filesize_ = 0;
    modification_date_.clear();
}

bool D_FtpClient::login(const std::string &host, const std::string &username, const std::string &password, const int mode) {
    if (ftpconn_ != nullptr) {  // 如果是登录状态，先退出登录
        FtpQuit(ftpconn_);
        ftpconn_ = nullptr;
    }

    connection_status = login_status = option_status = false;

    if (FtpConnect(host.c_str(), &ftpconn_) == false) {
        connection_status = true;
        return false;
    }

    if (FtpLogin(username.c_str(), password.c_str(), ftpconn_) == false) {
        login_status = true;
        return false;
    }

    if (FtpOptions(FTPLIB_CONNMODE, (long)mode, ftpconn_) == false) {
        option_status = true;
        return false;
    }

    return true;
}

bool D_FtpClient::logout() {
    if (ftpconn_ == nullptr) return false;

    FtpQuit(ftpconn_);

    ftpconn_ = 0;

    return true;
}

bool D_FtpClient::get(const std::string &remote_filename, const std::string &local_filename, const bool check_time) {
    if (ftpconn_ == 0)
        return false;

    // 生成本地文件的临时文件名。
    std::string local_filename_tmp = local_filename + ".tmp";

    // 获取远程服务器的文件的时间。
    if (mod_date(remote_filename) == false)
        return false;

    // 取文件。
    if (FtpGet(local_filename_tmp.c_str(), remote_filename.c_str(), FTPLIB_IMAGE, ftpconn_) == false) return false;

    // 判断文件下载前和下载后的时间，如果时间不同，表示在文件传输的过程中已发生了变化，返回失败。
    if (check_time == true) {
        std::string m_time = modification_date_;

        if (mod_date(remote_filename) == false) return false;

        if (modification_date_ != m_time) return false;
    }

    // 重置文件时间。
    dddd::d_file::set_modification_time(local_filename_tmp, modification_date_);
    // 改为正式的文件。
    if (rename(local_filename_tmp.c_str(), local_filename.c_str()) != 0) {
        std::cout << local_filename_tmp << " " << local_filename << std::endl;
        std::cerr << "rename failed: " << strerror(errno) << std::endl;
        return false;
    }

    // 获取文件的大小。
    filesize_ = dddd::d_file::filesize(local_filename.c_str());

    return true;
}

bool D_FtpClient::mod_date(const std::string &remote_filename) {
    if (ftpconn_ == 0)
        return false;

    modification_date_.clear();

    std::string mod_time;
    mod_time.resize(14);

    if (FtpModDate(remote_filename.c_str(), mod_time.data(), 14, ftpconn_) == false) return false;

    // 把UTC时间转换为本地时间。
    dddd::d_time::utc_transfer(mod_time, 8);

    return true;
}

bool D_FtpClient::size(const std::string &remote_filename) {
    if (ftpconn_ == 0) return false;

    filesize_ = 0;

    if (FtpSize(remote_filename.c_str(), &filesize_, FTPLIB_IMAGE, ftpconn_) == false) return false;

    return true;
}

bool D_FtpClient::chdir(const std::string &remote_dir) {
    if (ftpconn_ == 0) return false;

    if (FtpChdir(remote_dir.c_str(), ftpconn_) == false) return false;

    return true;
}

bool D_FtpClient::mkdir(const std::string &remote_dir) {
    if (ftpconn_ == 0) return false;

    if (FtpMkdir(remote_dir.c_str(), ftpconn_) == false) return false;

    return true;
}

bool D_FtpClient::rmdir(const std::string &remote_dir) {
    if (ftpconn_ == 0) return false;

    if (FtpRmdir(remote_dir.c_str(), ftpconn_) == false) return false;

    return true;
}

std::string D_FtpClient::nlist(const std::string &remote_dir) {
    if (ftpconn_ == 0) return "";
    std::fstream file;
    std::string temp_path = "../temp/ftp_get.temp";
    dddd::d_file::open(file, temp_path);  // 创建临时文件
    file.close();
    if (FtpNlst(temp_path.c_str(), remote_dir.c_str(), ftpconn_) == false) {
        remove(temp_path.c_str());
        return "";
    }
    dddd::d_file::open(file, temp_path);
    std::string line;
    std::string ret;
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            ret += line + "\n";
        }
        file.close();
    }
    remove(temp_path.c_str());
    return ret;
}

bool D_FtpClient::put(const std::string &local_filename, const std::string &remote_filename, const bool check_size) {
    if (ftpconn_ == 0) return false;

    // 生成服务器文件的临时文件名。
    std::string remote_filename_tmp = remote_filename + ".tmp";

    std::string filetime_before, filetime_after;
    filetime_before = dddd::d_file::file_modification_time(local_filename);  // 获取上传文件之前的时间。

    // 发送文件。
    if (FtpPut(local_filename.c_str(), remote_filename_tmp.c_str(), FTPLIB_IMAGE, ftpconn_) == false) return false;

    filetime_after = dddd::d_file::file_modification_time(local_filename);  // 获取上传文件之后的时间。

    // 如果文件上传前后的时间不一致，说明本地有修改文件，放弃本次上传。
    if (filetime_before != filetime_after) {
        delete_file(remote_filename_tmp);
        return false;
    }

    // 重命名文件。
    if (FtpRename(remote_filename_tmp.c_str(), remote_filename.c_str(), ftpconn_) == false) return false;

    // 判断已上传的文件的大小与本地文件是否相同，确保上传成功。
    // 一般来说，不会出现文件大小不一致的情况，如果有，应该是服务器方的原因，不太好处理。
    if (check_size == true) {
        if (size(remote_filename) == false) return false;

        if (filesize_ != dddd::d_file::filesize(local_filename)) {
            delete_file(remote_filename);
            return false;
        }
    }

    return true;
}

bool D_FtpClient::delete_file(const std::string &remote_filename) {
    if (ftpconn_ == nullptr) return false;

    if (FtpDelete(remote_filename.c_str(), ftpconn_) == false) return false;

    return true;
}

bool D_FtpClient::rename_file(const std::string &src_filename, const std::string &target_filename) {
    if (ftpconn_ == nullptr) return false;

    if (FtpRename(src_filename.c_str(), target_filename.c_str(), ftpconn_) == false) return false;

    return true;
}

bool D_FtpClient::site(const std::string &command) {
    if (ftpconn_ == 0) return false;

    if (FtpSite(command.c_str(), ftpconn_) == false) return false;

    return true;
}

char *D_FtpClient::response() {
    if (ftpconn_ == 0)
        return 0;

    return FtpLastResponse(ftpconn_);
}
