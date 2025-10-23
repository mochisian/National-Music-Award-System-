#include "TimeUtils.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace TimeManagement {

    bool convertTimeStringToTimeT(const std::string& timeStr, time_t baseDate, time_t& outTime) {
        std::stringstream ss(timeStr);
        int hours = 0, minutes = 0;
        char colon = 0;

        ss >> hours >> colon >> minutes;

        if (ss.fail() || colon != ':' || ss.rdbuf()->in_avail() != 0) {
            return false;
        }

        if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
            return false;
        }

        std::tm tm = {};
        if (localtime_s(&tm, &baseDate) != 0) {
            return false;
        }

        tm.tm_hour = hours;
        tm.tm_min = minutes;
        tm.tm_sec = 0;

        outTime = mktime(&tm);
        return outTime != -1;
    }

    std::string formatTimeTToString(time_t timeValue, const char* format) {
        if (timeValue == 0) {
            return "";
        }

        std::tm tm_buf;

        if (localtime_s(&tm_buf, &timeValue) != 0) {
            return "Invalid Time";
        }

        char buffer[80];
        if (strftime(buffer, sizeof(buffer), format, &tm_buf) == 0) {
            return "Format Error";
        }
        return std::string(buffer);
    }

    time_t convertToTimeT(const std::string& dateStr, const std::string& timeStr) {
        int year, month, day;
        char dash;
        std::istringstream dateStream(dateStr);
        dateStream >> year >> dash >> month >> dash >> day;

        int hour, minute;
        char colon;
        std::istringstream timeStream(timeStr);
        timeStream >> hour >> colon >> minute;

        std::tm timeInfo = {};
        timeInfo.tm_year = year - 1900; 
        timeInfo.tm_mon = month - 1;    
        timeInfo.tm_mday = day;          
        timeInfo.tm_hour = hour;
        timeInfo.tm_min = minute;
        timeInfo.tm_sec = 0;
        timeInfo.tm_isdst = -1;

        return std::mktime(&timeInfo);
    }
}
