#ifndef D_FTPCLIENT_H
#define D_FTPCLIENT_H

#include <dddd/d_file.h>

#include <string>

#include "ftplib.h"

namespace dddd {
class D_FtpClient {
private:
    netbuf *ftpconn_;  // ftp连接句柄。
public:
    unsigned int filesize_;          // 文件的大小，单位：字节。
    std::string modification_date_;  // 文件的修改时间

    // 以下三个成员变量用于存放login方法登录失败的原因。
    bool connection_status;  // 如果网络连接失败，该成员的值为true。
    bool login_status;       // 如果登录失败，用户名和密码不正确，或没有登录权限，该成员的值为true。
    bool option_status;      // 如果设置传输模式失败，该成员变量的值为true。

    D_FtpClient();   // 类的构造函数。
    ~D_FtpClient();  // 类的析构函数。

    D_FtpClient(const D_FtpClient &) = delete;
    D_FtpClient &operator=(const D_FtpClient) = delete;

    void init();

    // 登录ftp服务器。
    // host：ftp服务器ip地址和端口，ip:port
    // username：登录ftp服务器用户名。
    // password：登录ftp服务器的密码。
    // mode：传输模式，1-FTPLIB_PASSIVE是被动模式，2-FTPLIB_PORT是主动模式，缺省是被动模式。
    bool login(const std::string &host, const std::string &username, const std::string &password, const int mode = FTPLIB_PASSIVE);

    // 注销。
    bool logout();

    // 获取ftp服务器上文件的时间。
    // remote_filename：待获取的文件名。
    // 返回值：false-失败；true-成功，获取到的文件时间存放在modification_time_成员变量中。
    bool mod_date(const std::string &remote_filename);

    // 获取ftp服务器上文件的大小。
    // remote_filename：待获取的文件名。
    // 返回值：false-失败；true-成功，获取到的文件大小存放在filesize_成员变量中。
    bool size(const std::string &remote_filename);

    // 改变ftp服务器的当前工作目录。
    bool chdir(const std::string &remote_dir);

    // 在ftp服务器上创建目录。
    // remote_dir：ftp服务器上待创建的目录名。
    // 返回值：true-成功；false-失败。
    bool mkdir(const std::string &remote_dir);

    // 删除ftp服务器上的目录。
    // remote_dir：ftp服务器上待删除的目录名。
    // 返回值：true-成功；如果权限不足、目录不存在或目录不为空会返回false。
    bool rmdir(const std::string &remote_dir);

    // 发送NLST命令列出ftp服务器目录中的子目录名和文件名。
    // remote_dir：ftp服务器的目录名。
    // list_filename：用于保存从服务器返回的目录和文件名列表。
    // 返回值：true-成功；false-失败。
    // 注意：如果列出的是ftp服务器当前目录，remote_dir用"","*","."都可以，但是，不规范的ftp服务器可能有差别。
    std::string nlist(const std::string &remote_dir);

    // 从ftp服务器上获取文件。
    // remote_filename：待获取ftp服务器上的文件名。
    // local_filename：保存到本地的文件名。
    // check_time：文件传输完成后，是否核对远程文件传输前后的时间，保证文件的完整性。
    // 返回值：true-成功；false-失败。
    // 注意：文件在传输的过程中，采用临时文件命名的方法，即在local_filename后加".tmp"，在传输
    // 完成后才正式改为local_filename。
    bool get(const std::string &remote_filename, const std::string &local_filename, const bool check_time = true);

    // 向ftp服务器发送文件。
    // local_filename：本地待发送的文件名。
    // remote_filename：发送到ftp服务器上的文件名。
    // check_size：文件传输完成后，是否核对本地文件和远程文件的大小，保证文件的完整性。
    // 返回值：true-成功；false-失败。
    // 注意：文件在传输的过程中，采用临时文件命名的方法，即在remote_filename后加".tmp"，在传输
    // 完成后才正式改为remote_filename。
    bool put(const std::string &local_filename, const std::string &remote_filename, const bool check_size = true);

    // 删除ftp服务器上的文件。
    // remote_filename：待删除的ftp服务器上的文件名。
    // 返回值：true-成功；false-失败。
    bool delete_file(const std::string &remote_filename);

    // 重命名ftp服务器上的文件。
    // src_filename：ftp服务器上的原文件名。
    // target_filename：ftp服务器上的目标文件名。
    // 返回值：true-成功；false-失败。
    bool rename_file(const std::string &src_filename, const std::string &target_filename);

    // 向ftp服务器发送site命令。
    // command：命令的内容。
    // 返回值：true-成功；false-失败。
    bool site(const std::string &command);

    // 获取服务器返回信息的最后一条(return a pointer to the last response received)。
    char *response();
};

}  // namespace dddd
#endif  // D_FTPCLIENT_H
