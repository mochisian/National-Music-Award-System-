#include "EventMonitoring.h"

namespace EventMonitoring {
	void EventMonitoring::run(const std::string& eventId, const std::vector<std::string>& chosenEvent) {
		this->event = dm.getEventFromEventId(eventId);
		showMainMenu(chosenEvent);
	}

	void showScheduledAndOngoingEvent(const std::vector<std::vector<std::string>>& allScheduledEvent) {
		OutputManager output;
		output.clearScreen();
		output.printHeader("My Registered Events");
		output.println("Scheduled & Ongoing Events");
		if (allScheduledEvent.empty() || allScheduledEvent[0].empty()) {
			output.println("\nYou have no registered events.", OutputManager::Color::YELLOW);
		}
		else {
			output.displayTable({ "No.", "Event Id.","Event Name", "Date", "Time", "Status" }, allScheduledEvent);
		}
		output.print("\nPlease enter the index of the event to manage (or 'c' to cancel): ");
	}

	void EventMonitoring::adminView(User& loggedInUser) {
		DataManager dm;
		bool continueAdminView = true;

		while (continueAdminView) {
			std::vector<std::vector<std::string>> allScheduledEvent = dm.loadScheduledAndOngoingDataOfOrganizer(loggedInUser.userId);

			showScheduledAndOngoingEvent(allScheduledEvent);

			std::string userInput;
			std::getline(std::cin, userInput);

			if (validation.shouldCancel(userInput)) {
				continueAdminView = false;
				return;
			}

			if (allScheduledEvent.empty() || allScheduledEvent[0][1].empty()) {
				output.println("No events to select. ", OutputManager::Color::RED);
				system("pause");
				continue;
			}

			if (validation.validateMenuInput(1, allScheduledEvent.size(), userInput)) {
				const auto& chosenEvent = allScheduledEvent.at(std::stoi(userInput) - 1);
				run(chosenEvent[1], chosenEvent);
			}
			else {
				output.println("Invalid input!", OutputManager::Color::RED);
				system("pause");
			}
		}
	}

	void EventMonitoring::showMainMenu(const std::vector<std::string>& chosenEvent) {
		bool continueMonitoring = true;

		while (continueMonitoring) {
			output.clearScreen();
			output.printHeader("Event Monitoring for " + event.eventName);
			output.displayTable({ "No.", "Event Id.","Event Name", "Date", "Time", "Status" }, { chosenEvent });
			output.println("\nSelect the data you want to view:\n");
			output.println("1.  Sales");
			output.println("2.  Attendance");
			output.println("3.  Seat Available");
			output.println("4.  Exit Monitoring");
			output.print("\nEnter your choice (1-4): ");
			std::string userInput;
			std::getline(std::cin, userInput);

			if (userInput == "4") {
				continueMonitoring = false;
				return;
			}

			if (!validation.validateMenuInput(1, 4, userInput)) {
				output.println("Invalid input! Please enter a valid number.", OutputManager::Color::RED);
				system("pause");
				continue;
			}
			int choice = std::stoi(userInput);
			handleMonitoringType(choice);
		}
	}

	void EventMonitoring::handleMonitoringType(int& choice) {
		switch (choice) {
		case 1:
			displaySales();
			break;
		case 2:
			displayAttendance();
			break;
		case 3:
			displayAvailableSeat();
			break;
		}
	}

	void EventMonitoring::displaySales() {
		output.clearScreen();
		output.printHeader("Sales Monitoring");
		std::vector<std::vector<std::string>> completeSalesData;
		double totalSales = 0.0;
		dm.loadCompletedSalesOfEvent(completeSalesData, totalSales, event);
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << totalSales;
		output.println("=== Completed Sales ===\n");
		output.displayTable(
			{ "No.", "Time", "Payment Id", "Amount", "Method", "Status", "Customer" },
			completeSalesData
		);
		output.println("Total Completed Sales: " + ss.str() + "\n");

		output.println("\n=== Refunded Sales ===\n");
		std::vector<std::vector<std::string>> refundedSalesData;
		dm.loadRefundedSalesOfEvent(refundedSalesData, event);
		output.displayTable({"No.","Time", "Refund Id", "Payment Id", "Customer", "Refund Amount", "Reason", "Status"}, refundedSalesData);
;		system("pause");
	}

	void EventMonitoring::displayAttendance() {
		output.clearScreen();
		output.printHeader("Attendance Monitoring");
		printAttendanceMenu();
		std::vector<std::vector<std::string>> attendanceData = dm.loadAttendanceMonitoring(event);
		output.println("\n");
		output.displayTable({"No.", "Attendance Id", "Customer", "Event", "Status", "Time", "Seats Id"}, attendanceData);
		system("pause");
	}

	void EventMonitoring::printAttendanceMenu() {
		std::vector<std::vector<int>> venueAttendance(10, std::vector<int>(10, -1));
		dm.getAttendanceFromEvent(venueAttendance, event);
		output.printAttendanceSeat(venueAttendance);
	}

	void EventMonitoring::displayAvailableSeat() {
		output.clearScreen();
		output.printHeader("Seat Availability Monitoring");
		std::vector<std::vector<bool>> venueSeats(10, std::vector<bool>(10, false));

		std::vector<Seat> occupied_seats = dm.getOccupiedSeatsForEvent(event.eventId);
		for (const auto& seat : occupied_seats) {
			int rowIndex = std::stoi(seat.row) - 1;
			int colIndex = std::stoi(seat.column) - 1;
			if (rowIndex >= 0 && rowIndex < 10 && colIndex >= 0 && colIndex < 10) {
				venueSeats[rowIndex][colIndex] = true;
			}
		}
		output.printSeatingChart(venueSeats);
		output.println("\n");
		std::vector<std::vector<std::string>> seatData = dm.loadSeatMonitoring(event);
		output.displayTable({ "No.", "Seat Id", "Row", "Column", "Customer"}, seatData);

		system("pause");
	}
}