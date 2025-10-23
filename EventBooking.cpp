#include "EventBooking.h"
#include "PaymentCheckout.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace EventBooking {
	void EventBooking::run(User& user, time_t& currentDateTime) {
		this->user = user;
		this->events = dm.getAllScheduledEvent();
		this->currentDateTime = currentDateTime;
		showMenu();
	}

	void EventBooking::showMenu() {
		bool registrationCompleted = false;

		while (!registrationCompleted) {
			displayMenu();
			output.print("Enter the index of the event that you want to register (press c to cancel): ");
			std::string userInput;
			std::getline(std::cin, userInput);

			if (validation.shouldCancel(userInput)) {
				registrationCompleted = true;
				break;
			}

			if (!validation.validateMenuInput(1, events.size(), userInput)) {
				output.println("Invalid Input! Please enter a valid number.", OutputManager::Color::RED);
				system("pause");
				continue;
			}

			int choice = std::stoi(userInput);
			buyingTicket(choice);
			registrationCompleted = true;
		}
	}

	void EventBooking::displayMenu() {
		system("cls");
		output.printHeader("Event Registration");
		std::vector<std::string> eventsHeader = { "No.", "Event Name", "Date", "Time", "Available ticket", "Price" };
		std::vector<std::vector<std::string>> eventData;
		loadDataIntoVector(eventData);
		output.displayTable(eventsHeader, eventData);

		std::string eventName = "";
		if (!event.eventName.empty()) {
			eventName = event.eventName;
		}

		std::vector<std::string> registrationData = {
			"Event Name", eventName,
			"Number of ticket", ticketBuy == 0 ? "" : std::to_string(ticketBuy),
			"Selected Seats", selectedSeats.empty() ? "" : std::to_string(selectedSeats.size()) + " seats selected"
		};

		output.printMenu(registrationData);
	}

	void EventBooking::loadDataIntoVector(std::vector<std::vector<std::string>>& eventsData) {
		int eventNumber = 1;

		for (const auto& event : events) {
			if (event.status != EventStatus::SCHEDULED) {
				continue;
			}

			std::vector<std::string> eventRow;
			eventRow.push_back(std::to_string(eventNumber++));
			eventRow.push_back(event.eventName);
			eventRow.push_back(TimeManagement::formatTimeTToString(event.eventDate, "%Y-%m-%d"));

			std::string timeRange = TimeManagement::formatTimeTToString(event.startTime, "%H:%M") + " - " +
				TimeManagement::formatTimeTToString(event.endTime, "%H:%M");
			eventRow.push_back(timeRange);

			eventRow.push_back(std::to_string(dm.getAvailableSeats(event.eventId)));

			std::stringstream priceStream;
			priceStream << std::fixed << std::setprecision(2) << "RM " << event.ticketPrice;
			eventRow.push_back(priceStream.str());

			eventsData.push_back(eventRow);
		}

		if (eventsData.empty()) {
			eventsData.push_back({ "No events available", "", "", "", "", "" });
		}
	}

	void EventBooking::buyingTicket(int choice) {
		event = events[choice - 1];
		registration.eventId = event.eventId;

		displayMenu();
		output.print("How many tickets do you want to buy: ");
		std::string userInput;
		std::getline(std::cin, userInput);

		int availableSeats = dm.getAvailableSeats(event.eventId);
		if (!validation.validateMenuInput(1, availableSeats, userInput)) {
			output.println("Invalid input! Please enter a number between 1 and " +
				std::to_string(availableSeats), OutputManager::Color::RED);
			system("pause");
			return;
		}

		ticketBuy = std::stoi(userInput);
		selectedSeats.clear();
		selectSeats();
	}

	bool EventBooking::getValidateInput(const std::string& prompt, std::string& result) {
		output.print(prompt + " (press 'c' to cancel): ");
		std::string input;
		std::getline(std::cin, input);

		if (validation.shouldCancel(input)) {
			return false;
		}

		result = input;
		return true;
	}

	void EventBooking::printSeatMenu(const std::vector<std::string>& currentSelection) {
		std::vector<std::vector<bool>> venue_seats(10, std::vector<bool>(10, false));

		std::vector<Seat> occupied_seats = dm.getOccupiedSeatsForEvent(event.eventId);
		for (const auto& seat : occupied_seats) {
			int rowIndex = std::stoi(seat.row) - 1;
			int colIndex = std::stoi(seat.column) - 1;
			if (rowIndex >= 0 && rowIndex < 10 && colIndex >= 0 && colIndex < 10) {
				venue_seats[rowIndex][colIndex] = true;
			}
		}

		for (const auto& seatId : currentSelection) {
			size_t r_pos = seatId.find('R');
			size_t c_pos = seatId.find('C');
			if (r_pos != std::string::npos && c_pos != std::string::npos) {
				std::string rowStr = seatId.substr(r_pos + 1, c_pos - r_pos - 1);
				std::string colStr = seatId.substr(c_pos + 1);
				int rowIndex = std::stoi(rowStr) - 1;
				int colIndex = std::stoi(colStr) - 1;

				if (rowIndex >= 0 && rowIndex < 10 && colIndex >= 0 && colIndex < 10) {
					venue_seats[rowIndex][colIndex] = true;
				}
			}
		}

		output.printSeatingChart(venue_seats);
	}

	void EventBooking::selectSeats() {
		for (int i = 0; i < ticketBuy; i++) {
			displayMenu();
			printSeatMenu(selectedSeats);

			output.println("Select seat " + std::to_string(i + 1) + " of " + std::to_string(ticketBuy),
				OutputManager::Color::CYAN);

			std::string row, col;
			output.print("Enter row number (1-10): ");
			std::getline(std::cin, row);

			if (validation.shouldCancel(row)) {
				return;
			}

			output.print("Enter column number (1-10): ");
			std::getline(std::cin, col);

			if (validation.shouldCancel(col)) {
				return;
			}

			if (!validation.validateMenuInput(1, 10, row) || !validation.validateMenuInput(1, 10, col)) {
				output.println("Invalid row or column! Please enter numbers between 1 and 10.",
					OutputManager::Color::RED);
				system("pause");
				i--;
				continue;
			}

			if (!isSeatAvailable(row, col)) {
				output.println("Seat row " + row + ", column " + col + " is already occupied!",
					OutputManager::Color::RED);
				system("pause");
				i--;
				continue;
			}

			std::string seatId = "R" + row + "C" + col;
			if (std::find(selectedSeats.begin(), selectedSeats.end(), seatId) != selectedSeats.end()) {
				output.println("You've already selected this seat in this transaction!", OutputManager::Color::RED);
				system("pause");
				i--;
				continue;
			}

			selectedSeats.push_back(seatId);
			output.println("Seat " + seatId + " selected successfully!", OutputManager::Color::GREEN);
			system("pause");
		}
		showPaymentOptions();
	}

	bool EventBooking::isSeatAvailable(const std::string& row, const std::string& column) {
		std::vector<Seat> occupied_seats = dm.getOccupiedSeatsForEvent(event.eventId);

		for (const auto& seat : occupied_seats) {
			if (seat.row == row && seat.column == column) {
				return false;
			}
		}

		return true;
	}

	void EventBooking::showPaymentOptions() {
		double totalCost = event.ticketPrice * ticketBuy;

		system("cls");
		output.printHeader("Registration Summary");
		output.println("Event: " + event.eventName);
		output.println("Number of tickets: " + std::to_string(ticketBuy));
		output.println("Selected seats: ");

		for (const auto& seat : selectedSeats) {
			output.println("  - " + seat);
		}

		std::stringstream costStream;
		costStream << std::fixed << std::setprecision(2) << "RM " << totalCost;
		output.println("Total cost: " + costStream.str());
		output.println("----------------------------------------");

		output.println("Payment Options:");
		output.println("1. Pay Now");
		output.println("2. Pay Later");
		output.println("3. Cancel Registration");

		output.print("Enter your choice (1-3): ");
		std::string choice;
		std::getline(std::cin, choice);

		if (choice == "1") {
			Registration newRegistration = completeRegistration(RegistrationStatus::PENDING);
			proceedToPayment(newRegistration);
		}
		else if (choice == "2") {
			completeRegistration(RegistrationStatus::PENDING);
			output.println("Registration saved! You can complete payment later from the Payment System.",
				OutputManager::Color::CYAN);
			system("pause");
		}
		else if (choice == "3") {
			output.println("Registration cancelled.", OutputManager::Color::YELLOW);
			system("pause");
		}
		else {
			output.println("Invalid choice! Registration cancelled.", OutputManager::Color::RED);
			system("pause");
		}

		ticketBuy = 0;
		selectedSeats.clear();
		event = Event();
		registration = Registration();
	}

	void EventBooking::proceedToPayment(Registration& registrationToPay) {
		PaymentCheckout::PaymentCheckout paymentSystem(user, dm, currentDateTime);

		paymentSystem.displayInvoice(registrationToPay, event);

		output.print("Proceed with Payment? (Y/N):");
		std::string confirm;
		std::getline(std::cin, confirm);

		if (confirm == "y" || confirm == "Y") {
			paymentSystem.processPaymentForRegistration(registrationToPay, event);
		}
		else {
			output.println("Payment cancelled. You can pay later from Payment System.", OutputManager::Color::YELLOW);
			system("pause");
		}
	}

	Registration EventBooking::completeRegistration(RegistrationStatus status) {
		auto allRegistrations = dm.loadData<Registration>("registrations.json");

		Registration* target = nullptr;

		auto it = std::find_if(allRegistrations.begin(), allRegistrations.end(),
			[&](const Registration& reg) {
				return reg.customerId == user.userId
					&& reg.eventId == event.eventId
					&& reg.registrationStatus != RegistrationStatus::PENDING;
			});

		if (it == allRegistrations.end() || status == RegistrationStatus::PENDING) {
			Registration newReg;
			newReg.registrationId = dm.generateNewRegistrationId();
			newReg.customerId = user.userId;
			newReg.eventId = event.eventId;
			newReg.registerTime = time(nullptr);
			newReg.registrationStatus = status;
			newReg.seats.clear();
			allRegistrations.push_back(newReg);
			target = &allRegistrations.back();
		}
		else {
			it->registrationStatus = status;
			target = &(*it);
			target->seats.clear();
		}

		for (const auto& seatId : selectedSeats) {
			Seat s;
			s.seatId = seatId;

			size_t r = seatId.find('R');
			size_t c = seatId.find('C');
			if (r != std::string::npos && c != std::string::npos && c > r) {
				s.row = seatId.substr(r + 1, c - (r + 1));
				s.column = seatId.substr(c + 1);
			}

			s.status = (status == RegistrationStatus::CANCELLED)
				? SeatStatus::AVAILABLE
				: SeatStatus::RESERVED;

			s.registrationId = target->registrationId;
			target->seats.push_back(s);
		}

		dm.saveData(allRegistrations, "registrations.json");

		if (status == RegistrationStatus::PENDING) {
			output.println("Registration ID: " + target->registrationId, OutputManager::Color::GREEN);
		}
		return *target;
	}


}