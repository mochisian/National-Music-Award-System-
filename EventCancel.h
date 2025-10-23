#pragma once
#include "Model.h"
#include "OutputManager.h"
#include "DataManager.h"

namespace EventCancellation {
	void cancelEvent(const std::string& eventId, const std::vector<std::string>& chosenEvent, time_t& currentDateTime);
	void updateEventStatus(const std::string& eventId);
}