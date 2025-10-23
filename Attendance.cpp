#include "Attendance.h"
#include "TimeUtils.h"
#include <algorithm>
#include <limits>

namespace AttendanceManager {
    void AttendanceManager::run(User& user, time_t& currentDateTime, const std::string& regId) {
        this->user = user;
        this->currentDateTime = currentDateTime;
        this->registration = dm.getRegistrationFromRegistrationId(regId);
        processRegistrationChoice();
    }

    void AttendanceManager::selectionMenu() {
        processRegistrationChoice();
    }

    bool AttendanceManager::processRegistrationChoice() {
        bool deciding = true;

        while (deciding) {
            displaySingleRegistration(registration);
            output.print("Do you want to mark attendance for this event? (y/n/c to cancel): ");

            std::string userInput;
            std::getline(std::cin, userInput);

            if (userInput.empty()) continue;

            char response = std::tolower(userInput[0]);

            switch (response) {
            case 'y':
                return takeAttendance(registration);
            case 'n':
                output.println("Attendance not taken.", OutputManager::Color::YELLOW);
                system("pause");
                return true;
            case 'c':
                return false;
            default:
                output.println("Invalid input. Please enter 'y', 'n', or 'c'.", OutputManager::Color::RED);
                system("pause");
                break;
            }
        }
        return false;
    }


    bool AttendanceManager::takeAttendance(Registration& registration) {
        try {

            std::vector<Attendance> allAttendances = dm.loadData<Attendance>("attendances.json");
            auto existingAttendance = std::find_if(allAttendances.begin(), allAttendances.end(),
                [&](const Attendance& att) {
                    return att.registrationId == registration.registrationId && att.attendanceStatus == AttendanceStatus::PRESENT;
                });

            if (existingAttendance != allAttendances.end()) {
                output.println("Attendance for this registration has already been marked as PRESENT.", OutputManager::Color::YELLOW);
                system("pause");
                return true;
            }

            Event event = dm.getEventFromEventId(registration.eventId);

            output.clearScreen();
            output.printHeader("Taking Attendance");
            output.println("Current Time: " + TimeManagement::formatTimeTToString(currentDateTime, "%Y-%m-%d %H:%M"));
            output.println("Event Start: " + TimeManagement::formatTimeTToString(event.startTime, "%Y-%m-%d %H:%M"));
            output.println("Event End: " + TimeManagement::formatTimeTToString(event.endTime, "%Y-%m-%d %H:%M"));

            AttendanceStatus status = determineAttendanceStatus(event);

            switch (status) {
            case AttendanceStatus::PRESENT:
                output.println("Attendance marked successfully!", OutputManager::Color::GREEN);
                updateRegistrationStatus(registration);
                saveAttendance(registration, status);
                break;
            case AttendanceStatus::ABSENT:
                output.println("Cannot take attendance - Event time has passed", OutputManager::Color::RED);
                break;
            }

            system("pause");
            return true;

        }
        catch (const std::exception& e) {
            output.println("Error taking attendance: " + std::string(e.what()), OutputManager::Color::RED);
            system("pause");
            return false;
        }
    }

    AttendanceStatus AttendanceManager::determineAttendanceStatus(const Event& event) {
        if (currentDateTime >= event.startTime && currentDateTime <= event.endTime) {
            return AttendanceStatus::PRESENT;
        }
        else if (currentDateTime > event.endTime) {
            return AttendanceStatus::ABSENT;
        }
        else {
            return AttendanceStatus::PRESENT;
        }
    }

    void AttendanceManager::updateRegistrationStatus(Registration& registration) {
        std::vector<Registration> allRegistraion = dm.loadData<Registration>("registrations.json");
        auto it = std::find_if(allRegistraion.begin(), allRegistraion.end(),
            [&](const Registration& reg) {
                return reg.registrationId == registration.registrationId;
            });
        if (it != allRegistraion.end()) {
            it->registrationStatus = RegistrationStatus::COMPLETED;
        }
        dm.saveData(allRegistraion, "registrations.json");
    }

    void AttendanceManager::saveAttendance(const Registration& registration, AttendanceStatus status) {
        try {
            std::vector<Attendance> allAttendances = dm.loadData<Attendance>("attendances.json");

            auto it = std::find_if(allAttendances.begin(), allAttendances.end(),
                [&](const Attendance& att) {
                    return att.registrationId == registration.registrationId;
                });

            if (it != allAttendances.end()) {
                it->attendanceStatus = status;
                output.println("Updated existing attendance record.", OutputManager::Color::BLUE);
            }
            else {
                Attendance newAttendance;
                newAttendance.attendanceId = dm.generateNewAttendanceId();
                newAttendance.registrationId = registration.registrationId;
                newAttendance.attendanceTime = time(nullptr);
                newAttendance.attendanceStatus = status;

                allAttendances.push_back(newAttendance);
                output.println("Created new attendance record.", OutputManager::Color::BLUE);
            }

            dm.saveData(allAttendances, "attendances.json");

        }
        catch (const std::exception& e) {
            output.println("Error saving attendance: " + std::string(e.what()), OutputManager::Color::RED);
        }
    }

    void AttendanceManager::displaySingleRegistration(const Registration& registration) {
        output.clearScreen();
        output.printHeader("Take Attendance");

        std::vector<std::string> headers = { "No.", "Event Name", "Description", "Date", "Time", "Tickets" };
        std::vector<std::vector<std::string>> data;

        loadRegistrationData(data, registration, 1);
        output.displayTable(headers, data);
    }

    void AttendanceManager::displayAllRegistrations() {
        output.clearScreen();
        output.printHeader("Your Upcoming Registrations");

        std::vector<std::string> headers = { "No.", "Event Name", "Description", "Date", "Time", "Tickets" };
        std::vector<std::vector<std::string>> data;

        loadAllRegistrationsData(data);
        output.displayTable(headers, data);
    }

    void AttendanceManager::loadRegistrationData(std::vector<std::vector<std::string>>& data,
        const Registration& registration, int index) {
        try {
            Event event = dm.getEventFromEventId(registration.eventId);

            std::vector<std::string> row;
            row.push_back(std::to_string(index));
            row.push_back(event.eventName);
            row.push_back(event.description);
            row.push_back(TimeManagement::formatTimeTToString(event.eventDate, "%Y-%m-%d"));

            std::string timeRange = TimeManagement::formatTimeTToString(event.startTime, "%H:%M") + " - " +
                TimeManagement::formatTimeTToString(event.endTime, "%H:%M");
            row.push_back(timeRange);
            row.push_back(std::to_string(registration.seats.size()));

            data.push_back(row);

        }
        catch (const std::exception& e) {
            output.println("Error loading registration data: " + std::string(e.what()), OutputManager::Color::RED);
        }
    }

    void AttendanceManager::loadAllRegistrationsData(std::vector<std::vector<std::string>>& data) {
        userRegistrations.clear();

        try {
            std::vector<Registration> allRegistrations = dm.loadData<Registration>("registrations.json");
            int count = 1;

            for (const auto& registration : allRegistrations) {
                if (registration.customerId != user.userId && registration.registrationStatus != RegistrationStatus::PENDING) {
                    continue;
                }

                Event event = dm.getEventFromEventId(registration.eventId);
                if (event.status != EventStatus::SCHEDULED) {
                    continue;
                }

                userRegistrations.push_back(registration);
                loadRegistrationData(data, registration, count++);
            }

            if (data.empty()) {
                data.push_back({ "", "No registrations available", "", "", "", "" });
            }

        }
        catch (const std::exception& e) {
            output.println("Error loading registrations: " + std::string(e.what()), OutputManager::Color::RED);
            data.push_back({ "", "Error loading data", "", "", "", "" });
        }
    }
}