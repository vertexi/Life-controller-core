#pragma once
#include <ctime>
#include <string>

namespace life_controller_core
{
std::string get_last_append_line();
}

int do_event_log(std::time_t start_time,
                 std::time_t end_time,
                 std::string event_name,
                 std::string event_data);
int edit_event_log(std::string event_name, std::time_t start_time, std::string event_data, std::time_t end_time);
int create_event_log(std::string event_name, std::string event_description);
int create_goal_log(std::string event_name,
                    bool timesOrduration,
                    int64_t quantity,
                    std::time_t goal_start_time_t,
                    std::time_t goal_end_time_t);
