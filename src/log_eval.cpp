#include "log_eval.hpp"
#include "app_config.hpp"
#include "common.hpp"
#include "include/fast-cpp-csv-parser/csv.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include <ctime>
#include <sstream>


#include "doctest_header.h"

using namespace rapidjson;

int print_json(Document& d)
{
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    d.Accept(writer);

    debug_printf("%s\n", buffer.GetString());
    return 0;
}

int event_eval_total_duration(Document& d)
{
    auto& allocator = d.GetAllocator();
    auto& events = d["events"];
    for (Value::ConstMemberIterator itr = events.MemberBegin(); itr != events.MemberEnd(); ++itr)
    {
        auto event_name = itr->name.GetString();
        auto& logs = itr->value["logs"];
        std::time_t total_duration = 0;
        for (Value::ConstValueIterator itr = logs.Begin(); itr != logs.End(); ++itr)
        {
            auto& log = *itr;
            total_duration += log["duration_t"].GetInt64();
        }
        Value total_duration_v;
        events[event_name]["total_duration_t"].SetInt64(total_duration);
    }

    return 0;
}

int event_eval_goal(std::string event_data, Value& goal_value, Document::AllocatorType& allocator)
{
    std::vector<std::string> goal_types = {"times", "duration"};
    for (auto& goal_type : goal_types)
    {
        if (event_data.find(goal_type) != std::string::npos)
        {
            auto head_size = (goal_type + ":").size();
            event_data =
                event_data.substr(event_data.find(goal_type) + head_size, event_data.size() - head_size);
            trim(event_data);
            atoi_func converter;
            if (converter(event_data.c_str(), event_data.size()))
            {
                auto duration_t = converter.value();
                Value goal_duration_t_v(duration_t);
                Value goal_type_v(goal_type.c_str(), allocator);
                goal_value.AddMember(goal_type_v, goal_duration_t_v, allocator);
                goal_value.AddMember("progress", Value().SetInt64(0), allocator);
                return 0;
            }
            puts(("ERROR: " + goal_type + "not found in event_data " + event_data + "\n").c_str());
            return 1;
        }
    }
    return 1;
}

Value& event_eval_log(Value& action_value,
                      uint64_t start_time_t,
                      uint64_t end_time_t,
                      const std::string& event_log,
                      Document::AllocatorType& allocator)
{
    Value action_start_time_t_v(start_time_t);
    Value action_end_time_t_v(end_time_t);
    auto duration_t = end_time_t - start_time_t;
    Value action_duration_t_v(duration_t);
    Value action_log_v(event_log.c_str(), allocator);

    action_value.AddMember("start_time_t", action_start_time_t_v, allocator);
    action_value.AddMember("end_time_t", action_end_time_t_v, allocator);
    action_value.AddMember("duration_t", action_duration_t_v, allocator);
    action_value.AddMember("log", action_log_v, allocator);

    return action_value;
}

int event_eval_log_with_goal(Value& event, std::time_t start_time_t_, std::time_t end_time_t_)
{
    bool addOrSubtract = true;
    std::time_t start_time_t, end_time_t;
    if (start_time_t_ < end_time_t_)
    {
        start_time_t = start_time_t_;
        end_time_t = end_time_t_;
        addOrSubtract = true;
    } else if (start_time_t_ == end_time_t_)
    {
        return 0;
    }
    else
    {
        start_time_t = end_time_t_;
        end_time_t = start_time_t_;
        addOrSubtract = false;
    }
    auto& goals = event["goals"];
    assert(goals.IsArray());
    for (SizeType i = 0; i < goals.Size(); i++)
    {
        auto& goal = goals[i];
        if (time_within(start_time_t, goal["start_time_t"].GetInt64(), goal["end_time_t"].GetInt64()))
        {
            if (goal.HasMember("times"))
            {
                if (addOrSubtract)
                    goal["progress"].SetInt64(goal["progress"].GetInt64() + 1);
                else
                    goal["progress"].SetInt64(goal["progress"].GetInt64() - 1);
            }
            else if (goal.HasMember("duration"))
            {
                    auto goal_progress_t = goal["progress"].GetInt64();
                    std::time_t goal_progress_t_new = 0;
                    if (addOrSubtract)
                        goal_progress_t_new = goal_progress_t + (end_time_t - start_time_t);
                    else
                        goal_progress_t_new = goal_progress_t - (end_time_t - start_time_t);

                    goal["progress"].SetInt64(goal_progress_t_new);
            }
        }
    }
    return 0;
}

int create_new_event(Document::AllocatorType& allocator,
                     Value& events,
                     std::string event_name,
                     std::string event_desc,
                     std::time_t create_date_time_t,
                     bool duplicate)
{
    if (duplicate)
    {
        auto& event = events[event_name.c_str()];
        auto& event_desc_orig = event["description"];
        auto event_desc_str_orig = event_desc_orig.GetString();
        event_desc_orig.SetString((event_desc_str_orig + std::string("\n") + event_desc).c_str(), allocator);
        event["duplicate"] = true;
        return 0;
    }

    Value event_create_date_time_t_v((int64_t)create_date_time_t);
    Value event_description_v(event_desc.c_str(), allocator);

    Value event_value(kObjectType);
    event_value.AddMember("create_date_time_t", event_create_date_time_t_v, allocator);
    event_value.AddMember("description", event_description_v, allocator);
    event_value.AddMember("duplicate", false, allocator);
    event_value.AddMember("logs", Value().SetArray(), allocator);
    event_value.AddMember("total_duration_t", (int64_t)0, allocator);
    event_value.AddMember("goals", Value().SetArray(), allocator);

    Value event_key;
    event_key.SetString(event_name.c_str(), allocator);

    events.AddMember(event_key, event_value, allocator);
    return 0;
}

Document d;

CSVREADER_CLASS* csv_reader = nullptr;

Document& get_log_document() { return d; }

int eval_log_init(CSVREADER_CLASS& csv_reader_)
{
    csv_reader = &csv_reader_;
    d.SetObject();
    auto& allocator = d.GetAllocator();
    Value events_init(kObjectType);
    d.AddMember("events", events_init, allocator);
    csv_reader->set_header(
        "action", "start_time", "duration", "event_name", "event_data", "start_time_t", "end_time_t");
    return true;
}

int get_event_names(std::set<std::string>& event_names)
{
    auto& events = d["events"];
    for (Value::ConstMemberIterator itr = events.MemberBegin(); itr != events.MemberEnd(); ++itr)
    {
        auto event_name = itr->name.GetString();
        event_names.insert(event_name);
    }
    return 0;
}

std::vector<event_do_log> get_all_events()
{
    auto& events = d["events"];
    std::vector<event_do_log> event_do_log_vec;
    for (Value::ConstMemberIterator itr = events.MemberBegin(); itr != events.MemberEnd(); ++itr)
    {
        auto event_name = itr->name.GetString();
        auto& logs = itr->value["logs"];
        for (Value::ConstValueIterator itr = logs.Begin(); itr != logs.End(); ++itr)
        {
            auto& log = *itr;
            event_do_log event_do_log_;
            event_do_log_.data_ptr = itr;
            event_do_log_.event_name = (char*)event_name;
            event_do_log_.event_data = (char*)log["log"].GetString();
            event_do_log_.start_time_t = log["start_time_t"].GetInt64();
            event_do_log_.end_time_t = log["end_time_t"].GetInt64();
            event_do_log_.duration_t = log["duration_t"].GetInt64();

            std::string start_time_str = get_time_str(&event_do_log_.start_time_t);
            event_do_log_.start_time = std::move(start_time_str);
            std::string end_time_str = get_time_str(&event_do_log_.end_time_t);
            event_do_log_.end_time = std::move(end_time_str);
            std::string duration_str = get_duration_str(event_do_log_.start_time_t, event_do_log_.end_time_t);
            event_do_log_.duration = std::move(duration_str);

            event_do_log_vec.push_back(event_do_log_);
        }
    }
    return event_do_log_vec;
}

int eval_log_line(std::string action,
                  std::string start_time,
                  std::string duration,
                  std::string event_name,
                  std::string event_data,
                  std::time_t start_time_t,
                  std::time_t end_time_t)
{
    auto& allocator = d.GetAllocator();
    if (d.HasMember("events"))
    {
        auto events = d["events"].GetObject();
        if (action == "CREATE")
        {
            create_new_event(allocator,
                             events,
                             event_name,
                             event_data,
                             start_time_t,
                             events.HasMember(event_name.c_str()));
        }
        else if (action == "DO")
        {
            if (!events.HasMember(event_name.c_str()))
            {
                create_new_event(allocator, events, event_name, event_data, start_time_t, false);
            }
            Value action_value(kObjectType);
            event_eval_log(action_value, start_time_t, end_time_t, event_data, allocator);
            events[event_name.c_str()]["logs"].PushBack(action_value, allocator);
            event_eval_log_with_goal(events[event_name.c_str()], start_time_t, end_time_t);
        }
        else if (action == "GOAL")
        {
            if (!events.HasMember(event_name.c_str()))
            {
                create_new_event(allocator, events, event_name, event_data, start_time_t, false);
            }
            auto& event_goals = events[event_name.c_str()]["goals"];
            Value goal_start_time_t_v((int64_t)start_time_t);
            Value goal_end_time_t_v((int64_t)end_time_t);
            Value goal_value(kObjectType);
            goal_value.AddMember("start_time_t", goal_start_time_t_v, allocator);
            goal_value.AddMember("end_time_t", goal_end_time_t_v, allocator);

            event_eval_goal(event_data, goal_value, allocator);
            event_goals.PushBack(goal_value, allocator);
        }
        else if (action == "EDIT")
        {
            if (!events.HasMember(event_name.c_str()))
            {
                create_new_event(allocator, events, event_name, event_data, start_time_t, false);
            }
            Value action_value(kObjectType);
            event_eval_log(action_value, start_time_t, end_time_t, event_data, allocator);
            auto& logs = events[event_name.c_str()]["logs"];
            Value::ConstValueIterator itr = logs.Begin();
            for (; itr != logs.End(); ++itr)
            {
                auto& log = *itr;
                auto start_time_orig_t = log["start_time_t"].GetInt64();
                if (start_time_orig_t == start_time_t) {
                    break;
                }
            }
            if (itr == logs.End()) {
                events[event_name.c_str()]["logs"].PushBack(action_value, allocator);
                event_eval_log_with_goal(events[event_name.c_str()], start_time_t, end_time_t);
            } else {
                auto& log = *itr;
                auto start_time_orig_t = log["start_time_t"].GetInt64();
                auto end_time_orig_t = log["end_time_t"].GetInt64();
                event_eval_log_with_goal(events[event_name.c_str()], end_time_orig_t, start_time_orig_t);
                logs.Erase(itr);
                if (start_time_t != end_time_t)
                {
                    logs.PushBack(action_value, allocator);
                    event_eval_log_with_goal(events[event_name.c_str()], start_time_t, end_time_t);
                }
            }
        }
        else
        {
            debug_printf("UNKNOWN\n");
        }
    }
    return 0;
}

int eval_log()
{
    std::string action;
    std::string start_time;
    std::string duration;
    std::string event_name;
    std::string event_data;
    std::time_t start_time_t;
    std::time_t end_time_t;

    while (
        csv_reader->read_row(action, start_time, duration, event_name, event_data, start_time_t, end_time_t))
    {
        eval_log_line(action, start_time, duration, event_name, event_data, start_time_t, end_time_t);
    }

    event_eval_total_duration(d);

    // print_json(d);

    return 0;
}

int eval_log_line_str(const char* line)
{
    std::string action;
    std::string start_time;
    std::string end_time;
    std::string duration;
    std::string event_name;
    std::string event_data;
    std::time_t start_time_t;
    std::time_t end_time_t;
    csv_reader->read_row_string(
        (char*)line, action, start_time, duration, event_name, event_data, start_time_t, end_time_t);
    eval_log_line(action, start_time, duration, event_name, event_data, start_time_t, end_time_t);

    return 0;
}

TEST_CASE("eval_log")
{
    CSVReader csv_reader_;
    eval_log_init(csv_reader_.csv_reader);

    std::string temp_str =
        R"(CREATE  , Tue Apr 23 01:41:17 2024, 00:00:00, test, test event, 1713807677, 1713807677
GOAL    , Tue Apr 23 01:41:17 2024, 10:00:00, test, times: 10, 1713807677, 1713843677
GOAL    , Tue Apr 23 01:41:17 2024, 10:00:00, test, times: 99999, 1713807677, 1713843677
GOAL    , Tue Apr 23 01:41:17 2024, 10:00:00, test, duration: 9999999, 1713807677, 1713843677
DO      , Tue Apr 23 01:41:17 2024, 10:00:00, test, test19, 1713807677, 1713843677
DO    , Tue Apr 23 01:41:17 2024, 10:00:00, test, test19, 1713807777, 1713807877
EDIT    , Tue Apr 23 01:41:17 2024, 10:00:00, test, test19, 1713807777, 1713807777
EDIT    , Tue Apr 23 01:41:17 2024, 10:00:00, test, test1988, 1713807677, 1713843677
)";
    std::istringstream iss(temp_str);

    for (std::string line; std::getline(iss, line);)
    {
        eval_log_line_str(line.c_str());
    }

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    d.Accept(writer);

    CHECK(std::string(buffer.GetString()) == std::string(
                                                 R"({
    "events": {
        "test": {
            "create_date_time_t": 1713807677,
            "description": "test event",
            "duplicate": false,
            "logs": [
                {
                    "start_time_t": 1713807677,
                    "end_time_t": 1713843677,
                    "duration_t": 36000,
                    "log": "test1988"
                }
            ],
            "total_duration_t": 0,
            "goals": [
                {
                    "start_time_t": 1713807677,
                    "end_time_t": 1713843677,
                    "times": 10,
                    "progress": 1
                },
                {
                    "start_time_t": 1713807677,
                    "end_time_t": 1713843677,
                    "times": 99999,
                    "progress": 1
                },
                {
                    "start_time_t": 1713807677,
                    "end_time_t": 1713843677,
                    "duration": 9999999,
                    "progress": 36000
                }
            ]
        }
    }
})"));
}
