#include <stdio.h>

#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
using namespace rapidjson;

#include "include/fast-cpp-csv-parser/csv.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <action.hpp>
#include <log_eval.hpp>
#include "config.hpp"

#include <ctime>

int main() {
    std::time_t start = std::time(0);
    std::time_t end = start + 10 * 60 * 60;
    create_event_log("test", "test event");
    create_goal_log("test", true, 10, start, end);
    create_goal_log("test", true, 99999, start, end);
    create_goal_log("test", false, 9999999, start, end);
    do_event_log(start, end, "test", "test");
    eval_log();
    return 0;
}
