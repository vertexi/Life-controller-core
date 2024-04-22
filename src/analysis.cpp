#include "include/rapidjson/document.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/stringbuffer.h"

#include "io.hpp"

extern rapidjson::Document d;

void get_all_event_log()
{
    using namespace rapidjson;
    auto& events = d["events"];
    for (Value::ConstMemberIterator itr = events.MemberBegin(); itr != events.MemberEnd(); ++itr)
    {
        auto event_name = itr->name.GetString();
        auto& logs = itr->value["logs"];

        debug_printf("Event: %s\n", event_name);

        for (Value::ConstValueIterator itr = logs.Begin(); itr != logs.End(); ++itr)
        {
            auto& log = *itr;
            debug_printf("\tLog: %s\n", log["log"].GetString());
        }
    }
}
