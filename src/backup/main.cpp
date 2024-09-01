#include <dddd/d_signal.h>
#include <dddd/d_time.h>
#include <mysql/mysql.h>
#include <spdlog/spdlog.h>

MYSQL *conn;

void handle_exit(int signal) {
    exit(signal);
}

int main(int argc, char const *argv[]) {
    dddd::d_signal::ignore_all_signal();
    signal(SIGTERM, handle_exit);
    signal(SIGINT, handle_exit);

    std::string now = dddd::d_time::get_time("%Y-%m-%d %H:%M:%S");
    spdlog::info("{}", now[11]);
    if (now[11] == 0 && now[12] == 4) {  // 凌晨四点钟
        // 初始化 MySQL 连接
        conn = mysql_init(NULL);

        // 连接到数据库
        if (!mysql_real_connect(conn, "47.76.100.96", "dddd", "S67fi6GPTy6D7NGB", "meteorological_observation", 0, NULL, 0)) {
            spdlog::error("mysql connect failed");
            exit(1);
        }
        mysql_query(conn, "START TRANSACTION;");
        std::string table = "obs_data_his" + dddd::d_time::get_time("%Y%m");
        // 如果不存在这个表就创建新表
        std::string sql = "CREATE TABLE IF NOT EXISTS " + table + " LIKE obs_data;";
        if (mysql_query(conn, sql.c_str())) {
            spdlog::error("mysql query error: {}", mysql_error(conn));
            mysql_close(conn);
            return -1;
        }
        sql = "INSERT INTO " + table + " SELECT * FROM obs_data WHERE date < '" + now + "'";
        if (mysql_query(conn, sql.c_str())) {
            spdlog::error("mysql query error: {}", mysql_error(conn));
            mysql_close(conn);
            return -1;
        }
        sql = "DELETE FROM obs_data WHERE date < '" + now + "'";
        if (mysql_query(conn, sql.c_str())) {
            spdlog::error("mysql query error: {}", mysql_error(conn));
            mysql_close(conn);
            return -1;
        }
        mysql_query(conn, "COMMIT");
    }
    return 0;
}