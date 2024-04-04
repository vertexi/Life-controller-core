#pragma once
#include "include/fast-cpp-csv-parser/csv.h"
#include "io.hpp"
#include "config.hpp"

int eval_log(CSVREADER_CLASS &csv_reader);
int eval_log_init(CSVREADER_CLASS &csv_reader);
int eval_log_line_str(CSVREADER_CLASS &csv_reader, char* line);
