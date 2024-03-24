#include "common.hpp"

std::string get_time_str(std::time_t *time)
{
    std::string time_str = std::ctime(time);
    time_str.erase(time_str.end() - 1);
    return time_str;
}

std::string get_duration_str(std::time_t duration_time)
{
    std::time_t duration_sec = duration_time;
    std::time_t duration_min = duration_sec / 60;
    duration_sec = duration_sec % 60;
    std::time_t duration_hour = duration_min / 60;
    duration_min = duration_min % 60;

    auto duration_str_gen = [](std::time_t duration_time)
    {
        if (duration_time < 10)
        {
            return "0" + std::to_string(duration_time) + ":";
        } else {
            return std::to_string(duration_time) + ":";
        }
    };
    auto duration_str = (duration_str_gen(duration_hour) + duration_str_gen(duration_min) + duration_str_gen(duration_sec));
    duration_str.pop_back();
    return duration_str;
}


std::string get_duration_str(std::time_t start_time, std::time_t end_time)
{
    std::time_t duration_sec = end_time - start_time;
   return get_duration_str(duration_sec);
}
