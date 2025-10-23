#pragma once
#include "Model.h"
#include "iostream"
#include "iomanip"
#include "Validation.h"
#include "OutputManager.h"
#include "DataManager.h"
#include <sstream>

namespace EventMonitoring {
	class EventMonitoring {
	public:
		void run(const std::string& eventId, const std::vector<std::string>& chosenEvent);
		void adminView(User& loggedInUser);
	private:
		Event event;
		OutputManager output;
		Validation validation;
		DataManager dm;
		void displaySales();
		void showMainMenu(const std::vector<std::string>& chosenEvent);
		void handleMonitoringType(int& choice);
		void displayAttendance();
		void displayAvailableSeat();
		void printAttendanceMenu();
	};
}