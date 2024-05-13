#pragma once
#include "config.hpp"
#include "io.hpp"
#include <set>
#include <string>

#include "include/rapidjson/document.h"

struct event_do_log
{
    event_do_log() {}
    char* event_name;
    char* event_data;
    std::time_t start_time_t;
    std::string start_time;
    std::time_t end_time_t;
    std::string end_time;
    std::time_t duration_t;
    std::string duration;
};

int eval_log();
int eval_log_init(CSVREADER_CLASS& csv_reader);
int eval_log_line_str(const char* line);
int get_event_names(std::set<std::string>& event_names);
rapidjson::Document& get_log_document();
std::vector<event_do_log> get_all_events();
