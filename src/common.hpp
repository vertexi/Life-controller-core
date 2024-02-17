#pragma once

#include <string>
#include <ctime>

std::string get_time_str(std::time_t *time);
std::string get_duration_str(std::time_t start_time, std::time_t end_time);
std::string get_duration_str(std::time_t duration_time);
#include <algorithm>
#include <cctype>
#include <locale>

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}


class atoi_func
{
public:
    atoi_func(): value_() {}

    inline int64_t value() const { return value_; }

    inline bool operator() (const char *str, size_t len)
    {
        value_ = 0;
        int sign = 1;
        if (str[0] == '-') { // handle negative
            sign = -1;
            ++str;
            --len;
        }

        switch (len) { // handle up to 10 digits, assume we're 32-bit
            case 10:    value_ += (str[len-10] - '0') * 1000000000;
            case  9:    value_ += (str[len- 9] - '0') * 100000000;
            case  8:    value_ += (str[len- 8] - '0') * 10000000;
            case  7:    value_ += (str[len- 7] - '0') * 1000000;
            case  6:    value_ += (str[len- 6] - '0') * 100000;
            case  5:    value_ += (str[len- 5] - '0') * 10000;
            case  4:    value_ += (str[len- 4] - '0') * 1000;
            case  3:    value_ += (str[len- 3] - '0') * 100;
            case  2:    value_ += (str[len- 2] - '0') * 10;
            case  1:    value_ += (str[len- 1] - '0');
                value_ *= sign;
                return value_ > 0;
            default:
                return false;
        }
    }
private:
    int64_t value_;
};

inline bool time_within(std::time_t event_start_time, std::time_t duration_start, std::time_t duration_end)
{
    return (event_start_time >= duration_start) && (event_start_time <= duration_end);
}
