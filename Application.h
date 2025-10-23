#pragma once
#include <ctime>
#include "LoginModule.h"
#include "OutputManager.h"
#include "Validation.h"
#include "EventManagement.h"
#include "Model.h"
#include "EventRegistration.h"
#include "DataManager.h"
#include "OrganizerDashboard.h"
#include "EventRegistration.h"

class Application {
public:
    Application();
    void run();

private:
    LoginModule::LoginSystem loginSystem;
    EventManagement::EventManagement eventManagement;
    OutputManager output;
    Validation validation;
    DataManager dm;

    time_t currentDateTime;
    User loggedInUser;

    void runAdminDashboard();
    void runOrganizerDashboard();
    void runCustomerDashboard();
};