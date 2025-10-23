#pragma once
#include "Model.h"
#include <vector>
#include <string>
#include "EventMonitoring.h"
#include "EventCancel.h"

class EventSystemMenu {
public:
    void run(User& user, time_t& currentDateTime, const std::vector<std::string>& chosenEvent = {});

private:
    void showMenu(const User& user, const std::vector<std::string>& chosenEvent);
    std::string getChoice(const User& user);
    void handleCustomerChoice(const std::string& choice, User& user, time_t& currentDateTime, const std::vector<std::string>& chosenEvent);
    void handleOrganizerChoice(const std::string& choice, User& user, const std::vector<std::string>& chosenEvent, time_t& currentDateTime);
};