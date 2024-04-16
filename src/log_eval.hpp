#pragma once
#include <string>
#include <set>
#include "include/fast-cpp-csv-parser/csv.h"
#include "io.hpp"
#include "config.hpp"

int eval_log();
int eval_log_init(CSVREADER_CLASS &csv_reader);
int eval_log_line_str(char* line);
int get_event_names(std::set<std::string>& event_names);
