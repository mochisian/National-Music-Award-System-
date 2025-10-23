#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include "Model.h"
using json = nlohmann::json;

class Validation {
public:
    enum class DateValidationStatus {
        VALID,
        INVALID_FORMAT,
        DATE_IN_PAST,
        TOO_SOON
    };
    enum class PerformanceTimeValidationStatus {
        VALID,
        TIME_CONFLICT,
        EXACT_TIME_MATCH
    };
    
    bool isInteger(const std::string& str);
    bool isFloat(const std::string& str);  
    bool validateMenuInput(const int& minValue, const int& maxValue, const std::string& userInput);
    bool shouldCancel(const std::string& str);
    bool shouldSave(const std::string& str);
    DateValidationStatus validateAndParseDate(const std::string& dateStr, time_t& outDate, time_t& currentDateTime);
    Validation::DateValidationStatus validateEventCancellation(time_t& eventDateTime, time_t& currentDateTime);
    bool validateDate(const std::string& dateStr);
    bool validateTime(const std::string& timeStr);
    bool isValidUsername(const std::string& username);
    bool isValidPassword(const std::string& passwordHash);
    bool isValidEmail(const std::string& email);
    bool isValidPhoneNo(const std::string& phoneNumber);
    bool validateUnpaidRegistration(time_t& registrationTime, time_t& currentDateTime);

    PerformanceTimeValidationStatus validatePerformanceTime(
        time_t startTime,
        time_t endTime,
        const std::string& eventId,
        const std::vector<Performance>& existingPerformances,
        const std::string& excludePerformanceId = ""
    );
    
    template <typename T>
    bool isObjectEmpty(const T& object) {
        json j = object;
        for (const auto& item : j.items()) {
            const auto& value = item.value();
            if (value.is_string() && value.get<std::string>().empty()) {
                return true;
            }
            if (value.is_number_integer() && value.get<long long>() == 0) {
                return true;
            }
            if (value.is_number_float() && value.get<double>() == 0.0) {
                return true;
            }
        }
        return false;
    }
};
