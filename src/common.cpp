#include "common.hpp"

std::string get_time_str(std::time_t *time)
{
    std::string time_str = std::ctime(time);
    time_str.erase(time_str.end() - 1);
    return time_str;
}

std::string get_duration_str(std::time_t start_time, std::time_t end_time)
{
    std::time_t duration_sec = end_time - start_time;
    std::time_t duration_min = duration_sec / 60;
    duration_sec = duration_sec % 60;
    std::time_t duration_hour = duration_min / 60;
    duration_min = duration_min % 60;
    std::string duration_str = std::to_string(duration_hour) + ":" + std::to_string(duration_min) + ":" + std::to_string(duration_sec);
    return duration_str;
}

std::string get_duration_str(std::time_t duration_time)
{
    std::time_t duration_sec = duration_time;
    std::time_t duration_min = duration_sec / 60;
    duration_sec = duration_sec % 60;
    std::time_t duration_hour = duration_min / 60;
    duration_min = duration_min % 60;
    std::string duration_str = std::to_string(duration_hour) + ":" + std::to_string(duration_min) + ":" + std::to_string(duration_sec);
    return duration_str;
}
