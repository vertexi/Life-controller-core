#pragma once
#include "config.hpp"
#include "include/fast-cpp-csv-parser/csv.h"
#include "io.hpp"
#include <set>
#include <string>


int eval_log();
int eval_log_init(CSVREADER_CLASS& csv_reader);
int eval_log_line_str(char* line);
int get_event_names(std::set<std::string>& event_names);
