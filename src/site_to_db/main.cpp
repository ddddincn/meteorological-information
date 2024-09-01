#include <dddd/d_file.h>
#include <dddd/d_signal.h>
#include <dddd/d_string.h>
#include <mysql/mysql.h>
#include <spdlog/spdlog.h>

#include <string>

MYSQL *conn;

void handle_exit(int signal) {
    if (signal == SIGKILL)
        mysql_query(conn, "ROLLBACK;");
    mysql_close(conn);
    exit(signal);
}

struct Site_Data {
    std::string province;   // 省份
    int site_id;            // 站号
    std::string site_name;  // 站点名称
    float longitude;        // 经度
    float latitude;         // 纬度
    float elevation;        // 海拔
};

int main(int argc, char const *argv[]) {
    dddd::d_signal::ignore_all_signal();
    signal(SIGTERM, handle_exit);
    signal(SIGINT, handle_exit);
    signal(SIGKILL, handle_exit);
    // 初始化 MySQL 连接
    conn = mysql_init(NULL);

    // 连接到数据库
    if (!mysql_real_connect(conn, "47.76.100.96", "dddd", "S67fi6GPTy6D7NGB", "meteorological_observation", 0, NULL, 0)) {
        spdlog::error("mysql connect failed");
        exit(1);
    }

    std::fstream file;
    dddd::d_file::open(file, "./data/site_data/site/site.csv");
    std::vector<struct Site_Data> data;
    mysql_query(conn, "START TRANSACTION;");
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);  // 先取出第一行，不用
        while (std::getline(file, line)) {
            std::vector<std::string> vec = dddd::d_string::split(line, ",");
            struct Site_Data site;
            if (vec.size() != 0) {  // 获取站点信息
                site.province = vec[0];
                site.site_id = std::stoi(vec[1]);
                site.site_name = vec[2];
                site.latitude = std::stof(vec[3]);
                site.longitude = std::stof(vec[4]);
                site.elevation = std::stof(vec[5]);

                char sql[256];
                snprintf(sql, 255, "INSERT INTO site_data (site_id, province, site_name, longitude, latitude, elevation) VALUES(%d,'%s','%s',%.02f,%.02f,%.02f);",
                         site.site_id, site.province.c_str(), site.site_name.c_str(), site.longitude, site.latitude, site.elevation);
                sql[255] = '\0';
                if (mysql_query(conn, sql)) {
                    // fprintf(stderr, "%s\n", mysql_error(conn));
                    spdlog::error("{} insert failed", sql);
                }
            }
        }
    }
    file.close();
    return 0;
}
