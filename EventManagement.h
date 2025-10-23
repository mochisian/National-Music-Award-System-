#pragma once
#include "iomanip"
#include "iostream"
#include "OutputManager.h"
#include "Validation.h"
#include "Model.h"
#include "DataManager.h"

namespace EventManagement {
	class EventManagement {
	public:
		DataManager dm;
		void getUserDateTimeInput(time_t& currentDateTime);
	};
}