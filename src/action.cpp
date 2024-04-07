#include "action.hpp"
#include "config.hpp"

#include "common.hpp"
#include "io.hpp"

namespace life_controller_core
{
    std::string last_append_line = "";

    std::string get_last_append_line() {
        return last_append_line;
    }
}

int append_log(std::string filename, std::string data)
{
    life_controller_core::last_append_line = data;
    life_controller_core::last_append_line.pop_back();
    append_file(filename, data);
    return 0;
}

int do_event_log(std::time_t start_time, std::time_t end_time, std::string event_name, std::string event_data)
{
    std::string start_time_str = get_time_str(&start_time);
    std::string duration_str = get_duration_str(start_time, end_time);

    std::string log_data = "DO      "  ", " +
                           start_time_str + ", " +
                           duration_str + ", " +
                           event_name + ", " +
                           event_data + ", " +
                           std::to_string(start_time) + ", " +
                           std::to_string(end_time) + "\n";

    append_log(LOGFILE_NAME, log_data);

    return 0;
}

int create_event_log(std::string event_name, std::string event_description)
{
    std::time_t current = std::time(0);
    std::string log_data = "CREATE  " ", " +
                           get_time_str(&current) + ", " +
                           "00:00:00, " +
                           event_name + ", " +
                           event_description + ", " +
                           std::to_string(current) + ", " +
                           std::to_string(current) + "\n";
    append_log(LOGFILE_NAME, log_data);
    return 0;
}

int create_goal_log(std::string event_name, bool timesOrduration, int64_t quantity, std::time_t goal_start_time_t, std::time_t goal_end_time_t)
{
    std::string goal_start_time_str = get_time_str(&goal_start_time_t);
    std::string goal_end_time_str = get_time_str(&goal_end_time_t);
    std::string goal_duration_str = get_duration_str(goal_start_time_t, goal_end_time_t);

    std::string goal_description = timesOrduration ? "times: " + std::to_string(quantity) : "duration: " + std::to_string(quantity);

    std::string log_data = "GOAL    " ", " +
                           goal_start_time_str + ", " +
                           goal_duration_str + ", " +
                           event_name + ", " +
                           goal_description + ", " +
                           std::to_string(goal_start_time_t) + ", " +
                           std::to_string(goal_end_time_t) + "\n";
    append_log(LOGFILE_NAME, log_data);
    return 0;
}
