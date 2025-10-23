#pragma once
#include <string>
#include <ctime>

namespace TimeManagement {
    bool convertTimeStringToTimeT(const std::string& timeStr, time_t baseDate, time_t& outTime);
    std::string formatTimeTToString(time_t timeValue, const char* format);
    time_t convertToTimeT(const std::string& dateStr, const std::string& timeStr);
}
