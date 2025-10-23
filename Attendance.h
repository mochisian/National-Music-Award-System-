#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Model.h"
#include "DataManager.h"
#include "Validation.h"
#include "OutputManager.h"
#include "TimeUtils.h"
#include <ctime>

namespace AttendanceManager {
    class AttendanceManager {
    public:
        void run(User& user, time_t& currentDateTime, const std::string& regId);

    private:
        User user;
        DataManager dm;
        Validation validation;
        OutputManager output;
        time_t currentDateTime;
        Attendance attendance;
        Registration registration;
        std::vector<Registration> userRegistrations;
        void selectionMenu();
        bool processRegistrationChoice();
        bool takeAttendance(Registration& registration);
        void saveAttendance(const Registration& registration, AttendanceStatus status);
        AttendanceStatus determineAttendanceStatus(const Event& event);
        void displayAllRegistrations();
        void displaySingleRegistration(const Registration& registration);
        void loadAllRegistrationsData(std::vector<std::vector<std::string>>& data);
        void loadRegistrationData(std::vector<std::vector<std::string>>& data,
            const Registration& registration, int index);
        void updateRegistrationStatus(Registration& registration);
    };
}