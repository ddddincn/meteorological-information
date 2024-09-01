#include <dddd/d_file.h>
#include <dddd/d_string.h>
#include <dddd/d_time.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
// 站点结构体
struct Site_Data {
    std::string province;   // 省份
    int site_id;            // 站号
    std::string site_name;  // 站点名称
    float longitude;        // 经度
    float latitude;         // 纬度
    float elevation;        // 海拔
};
// 观测数据结构体
struct Observation_Data {
    int site_id;            // 站号
    std::string site_name;  // 站点名称
    std::string date;       // 观测时间 yyyymmddHHMMSS
    float temperature;      // 温度
    float pressure;         // 气压
    float humidity;         // 湿度
    float wind_direction;   // 风向
    float wind_speed;       // 风速
    float rain;             // 降雨量
    float visibility;       // 能见度
};

int main(int argc, char const* argv[]) {
    auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("./logs/observe.log", 0, 0);
    auto logger = std::make_shared<spdlog::logger>("observe", sink);

    logger->info("模拟观测气象开始");

    std::fstream file("./data/site_data/raw_site_data.txt");
    if (!file.is_open()) {
        logger->error("模拟观测气象失败");
        return -1;
        // spdlog::error("./data/site_data/raw_site_data.txt不存在或打开失败");
    }
    std::string line;          // 用于存储行数据
    std::getline(file, line);  // 去除第一条没用的数据
    std::vector<Site_Data> sites;
    // spdlog::info("正在读取文件 ./data/site_data/raw_site_data.txt");
    while (std::getline(file, line)) {
        dddd::d_string::replace(line, " ", "");
        std::vector<std::string> site_data_vec = dddd::d_string::split(line, ",");
        struct Site_Data site_data;
        site_data.province = site_data_vec[0];
        site_data.site_id = std::stoi(site_data_vec[1]);
        site_data.site_name = site_data_vec[2];
        site_data.latitude = std::stof(site_data_vec[3]);
        site_data.longitude = std::stof(site_data_vec[4]);
        site_data.elevation = std::stof(site_data_vec[5]);
        sites.push_back(site_data);
    }
    file.close();
    // spdlog::info("./data/site_data/raw_site_data.txt 读取完成");

    // // 写入csv
    // std::fstream site_csv;
    // if (!dddd::d_file::open(site_csv, "../data/site_data/site/site.csv")) {
    //     spdlog::error("打开文件 ../data/site_data/site/site.csv 失败");
    // }
    // spdlog::info("正在写入文件 ../data/site_data/site/site.csv");
    // site_csv << "省份,站号,站名,纬度,经度,海拔\n";
    // for (auto s : sites) {
    //     char info[256];
    //     snprintf(info, 256, "%s,%d,%s,%.02f,%.02f,%.02f\n", s.province.c_str(), s.site_id, s.site_name.c_str(), s.latitude, s.longitude, s.elevation);
    //     site_csv << info;
    // }
    // site_csv.close();
    // spdlog::info("../data/site_data/site/site.csv 写入完成");
    // // 写入xml
    // std::fstream site_xml;
    // if (!dddd::d_file::open(site_xml, "../data/site_data/site/site.xml")) {
    //     spdlog::error("打开文件 ../data/site_data/site/site.xml 失败");
    // }
    // spdlog::info("正在写入文件 ../data/site_data/site/site.xml");
    // site_xml << "<data>";
    // for (auto s : sites) {
    //     char info[256];
    //     snprintf(info, 256, "<province>%s</province><site_id>%d</site_id><site_name>%s</site_name><latitude>%.02f</latitude><longitude>%.02f</longitude><elevation>%.02f</elevation><endl/>",
    //              s.province.c_str(), s.site_id, s.site_name.c_str(), s.latitude, s.longitude, s.elevation);
    //     site_xml << info;
    // }
    // site_xml << "</data>";
    // site_xml.close();
    // spdlog::info("../data/site_data/site/site.xml 写入完成");
    // // 写入json
    // std::fstream site_json;
    // if (!dddd::d_file::open(site_json, "../data/site_data/site/site.json")) {
    //     spdlog::error("打开文件 ../data/site_data/site/site.json 失败");
    // }
    // spdlog::info("正在写入文件 ../data/site_data/site/site.json");
    // site_json << "{\"sites\":[";
    // int cnt = 0;
    // for (auto s : sites) {
    //     char info[256];
    //     snprintf(info, 256, "{\"province\":\"%s\",\"site_id\":%d,\"site_name\":\"%s\",\"latitude\":%.02f,\"longitude\":%.02f,\"elevation\":%.02f}",
    //              s.province.c_str(), s.site_id, s.site_name.c_str(), s.latitude, s.longitude, s.elevation);
    //     site_json << info;
    //     if (cnt == sites.size() - 1)
    //         site_json << "\n";
    //     else {
    //         site_json << ",\n";
    //     }
    //     cnt++;
    // }
    // site_json << "]}";
    // site_json.close();
    // spdlog::info("../data/site_data/site/site.json 写入完成");

    // 生成伪观测数据
    std::vector<struct Observation_Data> obs_data;
    std::srand(std::time(nullptr));  // 随机数种子
    for (auto s : sites) {
        struct Observation_Data tmp;
        tmp.site_id = s.site_id;
        tmp.site_name = s.site_name;
        tmp.date = dddd::d_time::get_time("%Y%m%d%H%M00");
        tmp.temperature = rand() % 350 / 10.0;
        tmp.pressure = 950 + rand() % 1000 / 10.0;
        tmp.humidity = 10 + rand() % 900 / 10.0;
        tmp.wind_direction = rand() % 3600 / 10.0;
        tmp.wind_speed = rand() % 100 / 10.0;
        tmp.rain = rand() % 160 / 10.0;
        tmp.visibility = rand() % 400 / 10.0;
        obs_data.push_back(tmp);
    }

    std::string filename = "./data/obs_data/obs_data" + dddd::d_time::get_time("%Y%m%d%H%M00");
    // 写入csv
    std::fstream obs_csv;
    if (!dddd::d_file::open(obs_csv, filename + ".csv")) {
        // spdlog::error("打开文件 {} 失败", filename + ".csv");
        return -1;
    }
    // spdlog::info("写入文件 {}", filename + ".csv");
    obs_csv << "站号,站点名称,时间,温度,气压,湿度,风向,风速,降水量,能见度\n";
    for (auto o : obs_data) {
        char buff[256];
        snprintf(buff, 256, "%d,%s,%s,%.02f,%.02f,%.02f,%.02f,%.02f,%.02f,%.02f\n",
                 o.site_id, o.site_name.c_str(), o.date.c_str(), o.temperature, o.pressure, o.humidity, o.wind_direction, o.wind_speed, o.rain, o.visibility);
        obs_csv << buff;
    }
    logger->info("模拟观测气象结束");
    //  // 写入xml
    //  std::fstream obs_xml;
    //  if (!dddd::d_file::open(obs_xml, filename + ".xml")) {
    //      spdlog::error("打开文件 {} 失败", filename + ".xml");
    //      return -1;
    //  }
    //  spdlog::info("写入文件 {}", filename + ".xml");
    //  obs_xml << "<data>";
    //  for (auto o : obs_data) {
    //      char buff[256];
    //      snprintf(buff, 256,
    //               "<site_id>%d</site_id><date>%s</date><temperature>%.02f</temperature><pressure>%.02f</pressure><humidity>%.02f</humidity><wind_direction>%.02f</wind_direction><wind_speed>%.02f</wind_speed><rain>%.02f</><visibility>%.02f</visibility>",
    //               o.site_id, o.date.c_str(), o.temperature, o.pressure, o.humidity, o.wind_direction, o.wind_speed, o.rain, o.visibility);
    //      obs_xml << buff;
    //  }
    //  obs_xml << "</data>";
    //  spdlog::info("写入文件 {} 完成", filename + ".xml");

    // // 写入json
    // std::fstream obs_json;
    // if (!dddd::d_file::open(obs_json, filename + ".json")) {
    //     spdlog::error("打开文件 {} 失败", filename + ".json");
    //     return -1;
    // }
    // spdlog::info("写入文件 {}", filename + ".json");
    // int cnt = 0;
    // obs_json << "{\"data\":[";
    // for (auto o : obs_data) {
    //     char buff[256];
    //     snprintf(buff, 256, "{\"site_id\":%d,\"date\":\"%s\",\"temperature\":%.02f,\"pressure\":%.02f,\"humidity\":%.02f,\"wind_direction\":%.02f,\"wind_speed\":%.02f,\"rain\":%.02f,\"visibility\":%.02f}",
    //              o.site_id, o.date.c_str(), o.temperature, o.pressure, o.humidity, o.wind_direction, o.wind_speed, o.rain, o.visibility);
    //     obs_json << buff;
    //     if (cnt == obs_data.size() - 1)
    //         obs_json << "\n";
    //     else {
    //         obs_json << ",\n";
    //     }
    //     cnt++;
    // }
    // obs_json << "]}";
    // spdlog::info("写入文件 {} 完成", filename + ".json");
}
