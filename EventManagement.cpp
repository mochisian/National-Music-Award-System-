#include "EventManagement.h"
#include "TimeUtils.h"

namespace EventManagement {
	void EventManagement::getUserDateTimeInput(time_t& currentDateTime) {
		OutputManager output;
		Validation validation;

        std::string date;
        bool dateIsValid = false;
        std::string time;
        bool timeIsValid = false;

        do {
            output.clearScreen();
            output.printHeader("Set Time and Date");
            output.print("Please enter the date (YYYY-MM-DD) or press 'c' to cancel: ");
            std::getline(std::cin, date);

            if (date == "c" || date == "C") {
                output.println("Date input cancelled.", OutputManager::Color::YELLOW);
                return;
            }

            dateIsValid = validation.validateDate(date);
            if (!dateIsValid) {
                output.println("Invalid date format. Please use YYYY-MM-DD format.", OutputManager::Color::RED);
                system("pause");
            }
        } while (!dateIsValid);

        do {
            output.print("Enter time (HH:MM) or press 'c' to cancel: ");
            std::getline(std::cin, time);

            if (time == "c" || time == "C") {
                output.println("Time input cancelled.", OutputManager::Color::YELLOW);
                return;
            }

            timeIsValid = validation.validateTime(time);
            if (!timeIsValid) {
                output.println("Invalid time format. Please use HH:MM format.", OutputManager::Color::RED);
                system("pause");
            }
        } while (!timeIsValid);

        currentDateTime = TimeManagement::convertToTimeT(date, time);
        dm.updateEventStatus(currentDateTime);
        return;
	}
}