#include <ctime>
#include <regex>
#include <cctype>
#include <iostream>
#include <sstream>
#include "validation.h"
#include <iomanip>
#include <chrono> 
#include <vector>
#include "Model.h"

bool Validation::isInteger(const std::string& str) {
	std::regex integerRegex("^[-+]?\\d+$");
	return std::regex_match(str, integerRegex);
}

bool Validation::isFloat(const std::string& str) {
	std::regex floatRegex(R"(^[-+]?([0-9]*\.[0-9]+|[0-9]+\.?[0-9]*)$)");
	return std::regex_match(str, floatRegex);
}

bool Validation::validateMenuInput(const int& minValue, const int& maxValue, const std::string& userInput) {
	if (isInteger(userInput) && std::stoi(userInput) >= minValue && std::stoi(userInput) <= maxValue) {
		return true;
	}
	return false;
}

bool Validation::shouldCancel(const std::string& str) {
	if (str.length() == 1 && std::tolower(str[0]) == 'c') {
		return true;
	}
	return false;
}

bool Validation::shouldSave(const std::string& str) {
	if (str.length() == 1 && std::tolower(str[0]) == 's') {
		return true;
	}
	return false;
}

bool Validation::validateUnpaidRegistration(time_t& registrationTime, time_t& currentDateTime) {
	const int ONE_DAY = 24 * 60 * 60;
	if (currentDateTime - registrationTime > ONE_DAY) {
		return false;
	}

	return true;
}

Validation::DateValidationStatus Validation::validateAndParseDate(
	const std::string& dateStr,
	time_t& outDate,
	time_t& currentDateTime)
{
	if (!validateDate(dateStr)) return DateValidationStatus::INVALID_FORMAT;

	int year = std::stoi(dateStr.substr(0, 4));
	int month = std::stoi(dateStr.substr(5, 2));
	int day = std::stoi(dateStr.substr(8, 2));

	tm inputTm = {};
	inputTm.tm_year = year - 1900;
	inputTm.tm_mon = month - 1;
	inputTm.tm_mday = day;
	inputTm.tm_hour = 0;
	inputTm.tm_min = 0;
	inputTm.tm_sec = 0;

	time_t tempDate = mktime(&inputTm);
	if (tempDate == -1) {
		return DateValidationStatus::INVALID_FORMAT;
	}

	if (tempDate < currentDateTime) {
		return DateValidationStatus::DATE_IN_PAST;
	}

	const int SEVEN_DAYS = 7 * 24 * 60 * 60;
	if (tempDate < currentDateTime + SEVEN_DAYS) {
		return DateValidationStatus::TOO_SOON;
	}

	outDate = tempDate;
	return DateValidationStatus::VALID;
}


Validation::DateValidationStatus Validation::validateEventCancellation(
	time_t& eventDateTime,
	time_t& currentDateTime)
{

	const int SEVEN_DAYS = 7 * 24 * 60 * 60;
	if (eventDateTime - currentDateTime < SEVEN_DAYS) {
		return DateValidationStatus::TOO_SOON;
	}

	return DateValidationStatus::VALID;
}

bool Validation::validateDate(const std::string& dateStr) {
	std::regex dateRegex("^(\\d{4})-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01])$");

	if (!std::regex_match(dateStr, dateRegex)) {
		return false;
	}

	int year, month, day;
	char dash1, dash2;
	std::istringstream iss(dateStr);
	iss >> year >> dash1 >> month >> dash2 >> day;

	if (dash1 != '-' || dash2 != '-') {
		return false;
	}

	if (month < 1 || month > 12) {
		return false;
	}

	if (day < 1) {
		return false;
	}

	int maxDays;
	switch (month) {
	case 2:
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
			maxDays = 29;
		}
		else {
			maxDays = 28;
		}
		break;
	case 4: case 6: case 9: case 11:
		maxDays = 30;
		break;
	default:
		maxDays = 31;
		break;
	}

	if (day > maxDays) {
		return false;
	}

	return true;
}

bool Validation::validateTime(const std::string& timeStr) {
	std::regex timeRegex("^\\d{2}:\\d{2}$");

	if (!std::regex_match(timeStr, timeRegex)) {
		std::cout << timeStr;
		system("pause");
		return false;
	}

	int hour = std::stoi(timeStr.substr(0, 2));
	int minute = std::stoi(timeStr.substr(3, 2));

	if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
		return false;
	}

	return true;
}

bool Validation::isValidUsername(const std::string& username) {
	std::regex pattern("^[A-Za-z0-9@~#$%^&*_]{5,12}$");
	return std::regex_match(username, pattern);
}

bool Validation::isValidPassword(const std::string& passwordHash) {
	std::regex pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9]).{8,}$");
	return std::regex_match(passwordHash, pattern);
}

bool Validation::isValidEmail(const std::string& email) {
	std::regex pattern("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
	return std::regex_match(email, pattern);
}

bool Validation::isValidPhoneNo(const std::string& phoneNumber) {
	std::regex pattern("^1[0-9]-[0-9]{7,8}$");
	return std::regex_match(phoneNumber, pattern);
}

Validation::PerformanceTimeValidationStatus Validation::validatePerformanceTime(
	time_t startTime,
	time_t endTime,
	const std::string& eventId,
	const std::vector<Performance>& existingPerformances,
	const std::string& excludePerformanceId)
{
	for (const auto& existingPerf : existingPerformances) {
		// Skip if this is the same performance we're updating
		if (!excludePerformanceId.empty() && existingPerf.performanceId == excludePerformanceId) {
			continue;
		}

		// Skip if performance is for a different event or is deleted
		if (existingPerf.eventId != eventId || existingPerf.isDeleted) {
			continue;
		}

		// Check for exact time matches
		if (startTime == existingPerf.startTime && endTime == existingPerf.endTime) {
			return PerformanceTimeValidationStatus::EXACT_TIME_MATCH;
		}

		// Check for time conflicts (overlapping)
		// Two performances conflict if:
		// 1. New start time is between existing start and end time
		// 2. New end time is between existing start and end time  
		// 3. New performance completely encompasses existing performance
		// 4. Existing performance completely encompasses new performance
		bool hasConflict = (startTime >= existingPerf.startTime && startTime < existingPerf.endTime) ||
			(endTime > existingPerf.startTime && endTime <= existingPerf.endTime) ||
			(startTime <= existingPerf.startTime && endTime >= existingPerf.endTime) ||
			(existingPerf.startTime <= startTime && existingPerf.endTime >= endTime);

		if (hasConflict) {
			return PerformanceTimeValidationStatus::TIME_CONFLICT;
		}
	}

	return PerformanceTimeValidationStatus::VALID;
}