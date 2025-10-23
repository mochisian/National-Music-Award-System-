#pragma once
#include "Model.h"
#include "OutputManager.h"
#include "Validation.h"

namespace CustomerDashboard {
    void showAndManageCustomerEvents(User& loggedInUser, time_t& currentDateTime, OutputManager& output, Validation& validation);
}