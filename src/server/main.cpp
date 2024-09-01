#include <spdlog/spdlog.h>
#include <unistd.h>

#include "server.h"

struct Observation_Data {
    std::string site_id;         // 站号
    std::string site_name;       // 站点名
    std::string date;            // 观测时间 yyyymmddHHMMSS
    std::string temperature;     // 温度
    std::string pressure;        // 气压
    std::string humidity;        // 湿度
    std::string wind_direction;  // 风向
    std::string wind_speed;      // 风速
    std::string rain;            // 降雨量
    std::string visibility;      // 能见度
};

std::string url_decode(const std::string& value) {
    std::string result;
    char ch;
    int i, ii;
    for (i = 0; i < value.length(); i++) {
        if (value[i] == '%') {
            sscanf(value.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            result += ch;
            i = i + 2;
        } else if (value[i] == '+') {
            result += ' ';
        } else {
            result += value[i];
        }
    }
    return result;
}

void weather(HttpRequest* request, HttpResponse* response) {
    auto params = request->GetParameters();  // 获取请求中的参数
    if (params.count("token") == 0) {        // 没有token则403
        response->SetCode(403);
        return;
    }
    // 初始化mysql
    MYSQL* conn;
    conn = mysql_init(nullptr);
    SqlConnRAII(&conn, SqlConnPool::Instance());

    // mysql_query(conn, "START TRANSACTION;");
    //  查询该token
    std::string sql = "SELECT * FROM user WHERE token = '" + params["token"] + "'";
    if (mysql_query(conn, sql.c_str())) {
        std::cerr << mysql_error(conn) << "\n";
        return;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (res == nullptr) {
        std::cerr << "mysql_store_result() failed. Error: " << mysql_error(conn) << "\n";
        return;
    }
    if (mysql_num_rows(res) == 0) {  // 判断是否存在该记录
        response->SetCode(403);
    } else if (mysql_num_rows(res) == 1) {  // 如果存在token
        int num_fields = mysql_num_fields(res);
        MYSQL_ROW row;
        row = mysql_fetch_row(res);
        std::string token(row[2] ? row[2] : ""), status(row[3] ? row[3] : "0");
        if (status == "0" || status == "") {  // 判断该token是否被禁用
            response->SetCode(403);
            spdlog::info("{} request /api/weather 403", token);
            mysql_free_result(res);
            return;
        }
        // 满足条件，进行二次查询
        if (params.count("date") == 1) {
            if (params.count("site_name") == 1)
                sql = "SELECT * FROM obs_data WHERE date = '" + params["date"] + "' AND site_name = '" + url_decode(params["site_name"]) + "'";
            else
                sql = "SELECT * FROM obs_data WHERE date = '" + params["date"] + "'";
            if (mysql_query(conn, sql.c_str())) {
                std::cerr << mysql_error(conn) << "\n";
                mysql_free_result(res);
                return;
            }
            res = mysql_store_result(conn);
            num_fields = mysql_num_fields(res);
            std::vector<struct Observation_Data> data;
            struct Observation_Data tmp;
            while (row = mysql_fetch_row(res)) {
                tmp.site_id = row[0] ? row[0] : NULL;
                tmp.site_name = row[1] ? row[1] : NULL;
                tmp.date = row[2] ? row[2] : NULL;
                tmp.temperature = row[3] ? row[3] : NULL;
                tmp.pressure = row[4] ? row[4] : NULL;
                tmp.humidity = row[5] ? row[5] : NULL;
                tmp.wind_direction = row[6] ? row[6] : NULL;
                tmp.wind_speed = row[7] ? row[7] : NULL;
                tmp.rain = row[8] ? row[8] : NULL;
                tmp.visibility = row[9] ? row[9] : NULL;

                data.push_back(tmp);
            }
            std::string content = "{\"data\": [";
            for (int i = 0; i < data.size(); i++) {
                content += "{\"site_id\": " + data[i].site_id + ",\"site_name\": \"" + data[i].site_name + "\",\"date\": " + data[i].date;
                content += ",\"temperature\": " + data[i].temperature + ",\"pressure\": " + data[i].pressure + ",\"humidity\": " + data[i].humidity;
                content += ",\"wind_direction\": " + data[i].wind_direction + ",\"wind_speed\": " + data[i].wind_speed + ",\"rain\": " + data[i].rain + ",\"visibility\": " + data[i].visibility + "}";
                if (i != data.size() - 1)
                    content += ",";
            }
            content += "]}";
            response->SetType("application/json");
            response->SetContent(content);
        }

    } else {
        response->SetCode(500);
    }

    mysql_free_result(res);
}

void initRoute() {
    HttpConn::ADD_ROUTE("/api/weather", std::bind(weather, std::placeholders::_1, std::placeholders::_2));
}

int main() {
    initRoute();
    Server server(
        1316, 3, 60000, false,           /* 端口 ET模式 timeoutMs 优雅退出  */
        "ip", 3306, "user", "pwd", "db", /* Mysql配置 */
        4, 6);                           /* 连接池数量 线程池数量*/
    server.Start();
}
