#pragma once
#include <string>
#include <vector>
#include "Model.h"
#include "EventService.h"
#include "Validation.h"
#include "OutputManager.h"

namespace EventRegistration {

    class EventRegistration {
    public:
        EventRegistration(User& user, time_t& currentDateTime);
        void showMenu(User& user);

    private:
        time_t currentDateTime;
        void displayMenu(const Event& event);
        void editAttribute(Event& event, int choice, Validation& validation, OutputManager& output);
        void editEventName(Event& event, Validation& validation, OutputManager& output);
        void editEventDescription(Event& event, Validation& validation, OutputManager& output);
        void editEventDate(Event& event, Validation& validation, OutputManager& output);
        void editStartTime(Event& event, Validation& validation, OutputManager& output);
        void editEndTime(Event& event, Validation& validation, OutputManager& output);
        void editTicketPrice(Event& event, Validation& validation, OutputManager& output);
        bool getValidateInput(const std::string& prompt, std::string& result, Validation& validation, OutputManager& output);
        std::string generateNewEventId();
        bool saveEvent(const Event& event, Validation& validation);
    };
}