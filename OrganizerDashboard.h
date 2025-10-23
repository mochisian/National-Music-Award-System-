#pragma once
#include "Model.h"
#include "DataManager.h"
#include "OrganizerDashboard.h"
#include "OutputManager.h"
#include "EventSystemMenu.h"
#include "Validation.h"

namespace OrganizerDashboard {

	void showAndManageOrganizerEvents(User& loggedInUser, time_t& currentDateTime, OutputManager& output, Validation& validation);
}