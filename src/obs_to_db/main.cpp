#include <dddd/d_file.h>
#include <dddd/d_signal.h>
#include <dddd/d_string.h>
#include <dddd/d_time.h>
#include <mysql/mysql.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

MYSQL *conn;

void handle_exit(int signal) {
    if (signal == SIGKILL)
        mysql_query(conn, "ROLLBACK;");
    mysql_close(conn);
    exit(signal);
}

struct ObsData {
    int site_id;
    std::string site_name;
    std::string date;
    float temperature;
    float pressure;
    float humidity;
    float wind_direction;
    float wind_speed;
    float rain;
    float visibility;
    std::string update;
};

std::string generate_batch_insert_sql(const std::vector<ObsData> &records) {
    std::string sql = "INSERT INTO obs_data (site_id, site_name, date, temperature, pressure, humidity, wind_direction, wind_speed, rain, visibility, update_time) VALUES ";

    for (size_t i = 0; i < records.size(); ++i) {
        const auto &data = records[i];
        sql += "(" + std::to_string(data.site_id) + ", '" + data.site_name + "', '" + data.date + "', " +
               std::to_string(data.temperature) + ", " + std::to_string(data.pressure) + ", " +
               std::to_string(data.humidity) + ", " + std::to_string(data.wind_direction) + ", " +
               std::to_string(data.wind_speed) + ", " + std::to_string(data.rain) + ", " +
               std::to_string(data.visibility) + ", '" + data.update + "')";
        if (i != records.size() - 1) {
            sql += ", ";
        }
    }

    sql += ";";
    return sql;
}

int main(int argc, char const *argv[]) {
    dddd::d_signal::ignore_all_signal();
    signal(SIGINT, handle_exit);
    signal(SIGTERM, handle_exit);

    // 初始化 MySQL 连接
    conn = mysql_init(NULL);

    auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("./logs/obs_to_db.log", 0, 0);
    auto logger = std::make_shared<spdlog::logger>("observe", sink);

    // 连接到数据库
    if (!mysql_real_connect(conn, "47.76.100.96", "dddd", "S67fi6GPTy6D7NGB", "meteorological_observation", 0, NULL, 0)) {
        logger->error("mysql connect failed");
        exit(1);
    }

    // 查看./data/obs_data/目录下是否有文件
    for (auto &entry : std::filesystem::directory_iterator("./data/obs_data/"))
        if (entry.is_regular_file()) {
            std::fstream file(entry.path().string());
            bool flag = true;
            if (file.is_open()) {
                std::string line;
                std::getline(file, line);
                std::vector<ObsData> records;
                while (std::getline(file, line)) {
                    struct ObsData data;
                    std::vector<std::string> vec = dddd::d_string::split(line, ",");
                    if (vec.size() != 0) {
                        data.site_id = stoi(vec[0]);
                        data.site_name = vec[1];
                        data.date = vec[2];
                        data.temperature = stof(vec[3]);
                        data.pressure = stof(vec[4]);
                        data.humidity = stof(vec[5]);
                        data.wind_direction = stof(vec[6]);
                        data.wind_speed = stof(vec[7]);
                        data.rain = stof(vec[8]);
                        data.visibility = stof(vec[9]);
                        data.update = dddd::d_time::get_time("%Y-%m-%d %H:%M:%S");

                        records.push_back(data);
                    }
                }
                file.close();

                if (!records.empty()) {
                    mysql_query(conn, "START TRANSACTION;");
                    logger->info("正在写入数据库");

                    std::string sql = generate_batch_insert_sql(records);
                    if (mysql_query(conn, sql.c_str())) {
                        flag = false;
                        fprintf(stderr, "%s\n", mysql_error(conn));
                    }

                    if (flag) {  // 插入成功则移动文件到backup
                        dddd::d_file::mkdir("./data/obs_data/backup/");
                        std::filesystem::rename(entry.path().string(), "./data/obs_data/backup/" + std::filesystem::path(entry.path().string()).filename().string());
                        logger->info("数据库写入完成");
                        mysql_query(conn, "COMMIT");
                    } else {  // 失败则重置
                        logger->error("数据库写入错误");
                        mysql_query(conn, "ROLLBACK;");
                    }
                }
            }
        } else {
            logger->info("暂无内容可写");
        }
    return 0;
}
