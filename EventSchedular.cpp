#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <ctime>
#include <fstream>
#include <set>
#include <algorithm>
#include <conio.h>
#include <windows.h>
#include "EventSchedular.h"
#include "OutputManager.h"
#include "Model.h"
#include "Validation.h"
#include "TimeUtils.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

EventSchedular::EventSchedular() : performanceCounter(0), globalSongCounter(0) {
	loadEvents();
	loadPerformances();
	loadGlobalSongCounter();
}

EventSchedular::EventSchedular(const User& user) : performanceCounter(0), globalSongCounter(0), currentUser(user) {
	loadEvents();
	loadPerformances();
	loadGlobalSongCounter();
}

void EventSchedular::run() {
	std::string eventIdInput;

	do {
		displayMenu();
		
		if (currentUser.userType == UserType::CUSTOMER) {
			std::cout << "\nEnter Event ID you want to view performance schedular details (or 'c' to exit): ";
		} else {
			std::cout << "\nEnter Event ID to manage performances (or 'c' to exit): ";
		}
		
		std::getline(std::cin, eventIdInput);

		if (eventIdInput == "c" || eventIdInput == "C") {
			std::cout << "Exit to main menu..." << std::endl;
			break;
		}

		if (selectEvent(eventIdInput)) {
			managePerformancesForEvent();
			if (currentUser.userType == UserType::CUSTOMER) {
				break;
			}
		}
		
		if (eventIdInput != "c" && eventIdInput != "C") {
			pauseScreen();
		}

	} while (eventIdInput != "c" && eventIdInput != "C");

	savePerformancesToFile();
}

void EventSchedular::displayMenu() {
	OutputManager output;
	system("cls");
	
	// Display current events first
	displayCurrentEvents();
}

void EventSchedular::managePerformancesForEvent() {
	if (currentUser.userType == UserType::CUSTOMER) {
		displayPerformanceDetails();
		return;
	}
	else {
		int choice;
		do {
			displayPerformanceManagementMenu();
			choice = getPerformanceMenuChoice();
			handlePerformanceMenuChoice(choice);

			if (choice != 5) {
				pauseScreen();
			}
		} while (choice != 5);
	}
}

void EventSchedular::displayPerformanceManagementMenu() {
	OutputManager output;
	system("cls");
	
	Event selectedEvent = getEventById(selectedEventId);
	std::cout << "\nManaging Performances for: " << selectedEvent.eventName 
		 << " (ID: " << selectedEventId << ")" << std::endl;

	displayPerformanceForEvent();
	
	std::cout << "\nPlease choose an option:" << std::endl;
	std::cout << "1. Add Performance" << std::endl;
	std::cout << "2. Display Performance Details" << std::endl;
	std::cout << "3. Update Performance" << std::endl;
	std::cout << "4. Delete Performance" << std::endl;
	std::cout << "5. Back to Event Selection" << std::endl;
	std::cout << "Enter your choice: ";
	std::cout.flush();
}

int EventSchedular::getPerformanceMenuChoice() {
	OutputManager output;
	Validation validation;
    std::string choice;
    std::getline(std::cin, choice);
	while (true) {
		if (choice.empty()) {
			output.print("Invalid input! Please enter 1 - 5! ", OutputManager::Color::RED);
			std::cout << "\n\nPress any key to continue...";
			_getch();
			displayPerformanceManagementMenu();
			std::getline(std::cin, choice);
			continue;
		}

		if (!validation.isInteger(choice)) {
			output.print("Invalid input! Please enter 1 - 5! ", OutputManager::Color::RED);
			std::cout << "\n\nPress any key to continue...";
			_getch();
			displayPerformanceManagementMenu();
			std::getline(std::cin, choice);
			continue;
		}

		if (!validation.validateMenuInput(1, 5, choice)) {
			output.print("Invalid input! Please enter 1 - 5! ", OutputManager::Color::RED);
			std::cout << "\n\nPress any key to continue...";
			_getch();
			displayPerformanceManagementMenu();
			std::getline(std::cin, choice);
		} else {
			return std::stoi(choice);
		}
	}
}

void EventSchedular::handlePerformanceMenuChoice(int choice) {
	OutputManager output;
	switch (choice) {
	case 1:
		addPerformance();
		break;
	case 2:
		displayPerformanceDetails();
		break;
	case 3:
		updatePerformance();
		break;
	case 4:
		deletePerformance();
		break;
	case 5:
		// Clear selected event when going back
		selectedEventId.clear();
		output.println("Returning to event selection...", OutputManager::Color::BRIGHT_GREEN);
		break;
	default:
		std::cout << "Invalid choice! Please enter again." << std::endl;
	}
}

void EventSchedular::pauseScreen() {
	std::cout << "\nPress any key to continue...";
	_getch();
}

void EventSchedular::addPerformance() {
	system("cls");
	OutputManager output;
	Validation validation;
	Event selectedEvent = getEventById(selectedEventId);
	std::cout << "\nAdding performance to: " << selectedEvent.eventName << " (ID: " << selectedEventId << ")" << std::endl;
	printMiddle(86, "ADD NEW PERFORMANCE");

	Performance newPerformance;
	newPerformance.eventId = selectedEventId;

	std::cout << "\nEvent ID: " << newPerformance.eventId << " (Auto-assigned)" << std::endl;
	std::cout << "[Press ESC to cancel]" << std::endl;

	//Validate performer name
	bool validPerformerName = false;
	while (!validPerformerName) {
		std::cout << "Enter Performer Name: ";
		if (!getInputWithESC("", newPerformance.performerName)) {
			return; 
		}
		
		newPerformance.performerName.erase(0, newPerformance.performerName.find_first_not_of(" \t\n\r\f\v"));
		newPerformance.performerName.erase(newPerformance.performerName.find_last_not_of(" \t\n\r\f\v") + 1);
		
		if (newPerformance.performerName.empty()) {
			output.println("Performer name cannot be empty! Please enter a valid name.", OutputManager::Color::RED);
			continue;
		}
		
		if (newPerformance.performerName.length() > 50) {
			output.println("Performer name too long! Maximum 50 characters allowed.", OutputManager::Color::RED);
			continue;
		}
		
		if (newPerformance.performerName.length() < 2) {
			output.println("Performer name too short! Minimum 2 characters required.", OutputManager::Color::RED);
			continue;
		}
		
		validPerformerName = true;
	}

	bool validPerformanceTime = false;
	while (!validPerformanceTime) {
		bool wasCancelled = false;
		newPerformance.startTime = getValidatedTimeInputWithESC("Enter Start Time ", selectedEvent, wasCancelled);
		if (wasCancelled) {
			return;
		}

		newPerformance.endTime = getValidatedTimeInputWithESC("Enter End Time ", selectedEvent, wasCancelled);
		if (wasCancelled) {
			return;
		}

		while (newPerformance.endTime <= newPerformance.startTime) {
			output.println("End time must be after start time!", OutputManager::Color::RED);
			newPerformance.endTime = getValidatedTimeInputWithESC("Enter End Time ", selectedEvent, wasCancelled);
			if (wasCancelled) {
				return;
			}
		}

		// NEW: Validate performance time conflicts
		Validation::PerformanceTimeValidationStatus timeStatus = validation.validatePerformanceTime(
			newPerformance.startTime, 
			newPerformance.endTime, 
			selectedEventId, 
			performances
		);

		switch (timeStatus) {
		case Validation::PerformanceTimeValidationStatus::EXACT_TIME_MATCH:
			output.println("A performance with the exact same start and end time already exists for this event!", OutputManager::Color::RED);
			output.println("Please choose different performance times.", OutputManager::Color::YELLOW);
			break;
		case Validation::PerformanceTimeValidationStatus::TIME_CONFLICT:
			output.println("The performance time conflicts with an existing performance for this event!", OutputManager::Color::RED);
			output.println("Please choose non-overlapping performance times.", OutputManager::Color::YELLOW);
			break;
		case Validation::PerformanceTimeValidationStatus::VALID:
			validPerformanceTime = true;
			break;
		}
	}

	if (!addSongToPerformanceWithESC(newPerformance)) {
		return;
	}

	newPerformance.performanceId = generatePerformanceId();
	
	std::cout << "\nPerformance ID: " << newPerformance.performanceId << " (Generated)" << std::endl;

	savePerformance(newPerformance);
	output.println("? Performance added successfully!", OutputManager::Color::BRIGHT_GREEN);
}

void EventSchedular::displayPerformance() {
	system("cls");
	OutputManager output;
	printMiddle(86, "PERFORMANCE SCHEDULE");

	std::cout << std::left << std::setw(10) << "ID"
		<< std::setw(12) << "Event ID"
		<< std::setw(20) << "Performer"
		<< std::setw(19) << "Start Time"
		<< std::setw(19) << "End Time"
		<< std::setw(6) << "Songs" << std::endl;
	std::cout << std::string(86, '-') << std::endl;

	if (performances.empty()) {
		output.println("No performances scheduled yet.", OutputManager::Color::YELLOW);
	}

	for (size_t i = 0; i < performances.size(); i++) {
		const auto& perf = performances[i];
		std::cout << std::left << std::setw(10) << perf.performanceId
			<< std::setw(12) << perf.eventId
			<< std::setw(20) << (perf.performerName.length() > 19 ? 
				perf.performerName.substr(0, 16) + "..." : perf.performerName)
			<< std::setw(19) << timeToString(perf.startTime)
			<< std::setw(19) << timeToString(perf.endTime)
			<< std::setw(6) << perf.songs.size() << std::endl;
	}
	std::cout << std::string(86, '-') << std::endl;
}

void EventSchedular::displayPerformanceDetails() {
	system("cls");
	OutputManager output;
	Event selectedEvent = getEventById(selectedEventId);
	std::cout << "\nEvent: " << selectedEvent.eventName << " (ID: " << selectedEventId << ")" << std::endl;
	printMiddle(115, "PERFORMANCE DETAILS");

	std::set<std::string> seenPerformanceIds;
	std::vector<Performance> filteredPerformances;

	for (const auto& perf : performances) {
		if (perf.eventId == selectedEventId && !perf.isDeleted) {
			if (seenPerformanceIds.find(perf.performanceId) == seenPerformanceIds.end()) {
				seenPerformanceIds.insert(perf.performanceId);
				filteredPerformances.push_back(perf);
			}
		}
	}

	if (filteredPerformances.empty()) {
		output.println("No performances scheduled for this event yet.", OutputManager::Color::YELLOW);
		return;
	}

	std::cout << std::left << std::setw(16) << "Performance ID"
		<< std::setw(18) << "Performer Name"
		<< std::setw(18) << "Start Time"
		<< std::setw(18) << "End Time"
		<< std::setw(13) << "Total Songs"
		<< std::setw(20) << "Song List" << std::endl;
	std::cout << std::string(115, '-') << std::endl;

	for (const auto& perf : filteredPerformances) {

		std::cout << std::left << std::setw(16) << perf.performanceId
			<< std::setw(18) << (perf.performerName.length() > 17 ?
				perf.performerName.substr(0, 14) + "..." : perf.performerName)
			<< std::setw(18) << timeToString(perf.startTime)
			<< std::setw(18) << timeToString(perf.endTime)
			<< std::setw(13) << perf.songs.size();

		if (!perf.songs.empty()) {
			std::cout << "1. " << perf.songs[0].title << " (ID: " << perf.songs[0].songId << ")" << std::endl;

			for (size_t j = 1; j < perf.songs.size(); j++) {
				std::cout << std::setw(83) << " " 
					<< (j + 1) << ". " << perf.songs[j].title
					<< " (ID: " << perf.songs[j].songId << ")" << std::endl;
			}
		}
		else {
			std::cout << "No songs assigned" << std::endl;
		}

		std::cout << std::string(115, '-') << std::endl;
	}
}

void EventSchedular::updatePerformance() {
	while (true) {  
		system("cls");
		OutputManager output;
		Event selectedEvent = getEventById(selectedEventId);
		std::cout << "\nEvent: " << selectedEvent.eventName << " (ID: " << selectedEventId << ")" << std::endl;
		printMiddle(74, "UPDATE PERFORMANCE");
		
		std::vector<Performance> filteredPerformances;
		for (const auto& perf : performances) {
			if (perf.eventId == selectedEventId && !perf.isDeleted) {
				filteredPerformances.push_back(perf);
			}
		}
		
		if (filteredPerformances.empty()) {
			output.println("No performances available to update for this event.", OutputManager::Color::YELLOW);
			return;
		}
		
		std::vector<std::string> headers = { "ID", "Performer", "Start Time", "End Time", "Songs" };
		std::vector<std::vector<std::string>> performanceData;
		
		for (const auto& perf : filteredPerformances) {
			std::vector<std::string> row;
			row.push_back(perf.performanceId);
			row.push_back(perf.performerName.length() > 19 ? 
				perf.performerName.substr(0, 16) + "..." : perf.performerName);
			row.push_back(timeToString(perf.startTime));
			row.push_back(timeToString(perf.endTime));
			row.push_back(std::to_string(perf.songs.size()));
			performanceData.push_back(row);
		}
		output.displayTable(headers, performanceData);

		std::cout << "\n[Press ESC to cancel]" << std::endl;
		std::cout << "Enter performance ID to update: ";
		std::string performanceId;
		if (!getInputWithESC("", performanceId)) {
			return; 
		}

		auto it = std::find_if(performances.begin(), performances.end(),
			[&performanceId](const Performance& perf) {
				return perf.performanceId == performanceId;
			});
		
		if (it == performances.end()) {
			output.println("Performance with ID '" + performanceId + "' not found!", OutputManager::Color::RED);
			std::cout << "\nPress any key to try again...";
			_getch();
			continue; 
		}

		if (it->eventId != selectedEventId) {
			output.println("Performance does not belong to the currently selected event!", OutputManager::Color::RED);
			std::cout << "\nPress any key to try again...";
			_getch();
			continue; 
		}

		while (true) {
			system("cls");
			std::cout << "\nEvent: " << selectedEvent.eventName << " (ID: " << selectedEventId << ")" << std::endl;
			printMiddle(74, "UPDATE PERFORMANCE - " + it->performanceId);
			
			std::cout << "\nCurrent Performance Details: " << std::endl;
			std::cout << "Performance ID: " << it->performanceId << std::endl;
			std::cout << "Event ID: " << it->eventId << std::endl;
			std::cout << "Performer Name: " << it->performerName << std::endl;
			std::cout << "Start Time: " << timeToString(it->startTime) << std::endl;
			std::cout << "End Time: " << timeToString(it->endTime) << std::endl;
			std::cout << "Number of Songs: " << it->songs.size() << std::endl;

			std::cout << "\nWhat would you like to update?" << std::endl;
			std::cout << "1. Performer Name" << std::endl;
			std::cout << "2. Start Time" << std::endl;
			std::cout << "3. End Time" << std::endl;
			std::cout << "4. Songs" << std::endl;
			std::cout << "5. Update All Fields" << std::endl;
			std::cout << "6. Back to Performance List" << std::endl;
			std::cout << "7. Cancel (Back to Main Menu)" << std::endl;
			std::cout << "Enter your choice (1-7): ";

			std::string choice;
			if (!getInputWithESC("", choice)) {
				return;
			}

			if (choice == "1") {
				updatePerformerName(*it);
				std::cout << "\nPress any key to continue...";
				_getch();
			}
			else if (choice == "2") {
				updateStartTime(*it, selectedEvent);
				std::cout << "\nPress any key to continue...";
				_getch();
			}
			else if (choice == "3") {
				updateEndTime(*it, selectedEvent);
				std::cout << "\nPress any key to continue...";
				_getch();
			}
			else if (choice == "4") {
				updateSongs(*it);
				std::cout << "\nPress any key to continue...";
				_getch();
			}
			else if (choice == "5") {
				updateAllFields(*it, selectedEvent);
				std::cout << "\nPress any key to continue...";
				_getch();
			}
			else if (choice == "6") {
				break;  // Go back to performance list
			}
			else if (choice == "7") {
				return;  // Cancel and go back to main menu
			}
			else {
				output.println("Invalid choice, please try again. ", OutputManager::Color::RED);
				std::cout << "\nPress any key to continue...";
				_getch();
			}
		}
	}
}

void EventSchedular::deletePerformance() {
	while (true) {
		system("cls");
		OutputManager output;
		Event selectedEvent = getEventById(selectedEventId);
		std::cout << "\nEvent: " << selectedEvent.eventName << " (ID: " << selectedEventId << ")" << std::endl;
		printMiddle(74, "DELETE PERFORMANCE");

		std::vector<Performance> filteredPerformances;
		for (const auto& perf : performances) {
			if (perf.eventId == selectedEventId && !perf.isDeleted) {
				filteredPerformances.push_back(perf);
			}
		}

		if (filteredPerformances.empty()) {
			output.println("No performances available to delete for this event.", OutputManager::Color::YELLOW);
			return;
		}

		std::vector<std::string> headers = { "ID", "Performer", "Start Time", "End Time", "Songs" };
		std::vector<std::vector<std::string>> performanceData;
		
		for (const auto& perf : filteredPerformances) {
			std::vector<std::string> row;
			row.push_back(perf.performanceId);
			row.push_back(perf.performerName.length() > 19 ? 
				perf.performerName.substr(0, 16) + "..." : perf.performerName);
			row.push_back(timeToString(perf.startTime));
			row.push_back(timeToString(perf.endTime));
			row.push_back(std::to_string(perf.songs.size()));
			performanceData.push_back(row);
		}
		output.displayTable(headers, performanceData);

		std::cout << "\n[Press ESC to cancel]" << std::endl;
		std::cout << "Delete Option: " << std::endl;
		std::cout << "1. Delete a specific performance" << std::endl;
		std::cout << "2. Delete all performances for this event" << std::endl;
		std::cout << "3. Cancel" << std::endl;
		std::cout << "Enter your choice (1-3): ";

		std::string choice;
		if (!getInputWithESC("", choice)) {
			return;
		}

		if (choice == "1") {
			deleteSpecificPerformanceForEvent();
		}
		else if (choice == "2") {
			deleteAllPerformancesForEvent();
		}
		else if (choice == "3") {
			std::cout << "Delete operation cancelled. " << std::endl;
			return;
		}
		else {
			output.println("Invalid choice, please try again. ", OutputManager::Color::RED);
			std::cout << "\nPress any key to continue...";
			_getch();
			continue;
		}
	}
}

//=================================================================================

std::string EventSchedular::generatePerformanceId() {
	performanceCounter++;
	return "PERF" + std::to_string(1000 + performanceCounter).substr(1);
}

time_t EventSchedular::getValidatedTimeInputWithESC(const std::string& prompt, const Event& event, bool& wasCancelled) {
	OutputManager output;
	Validation validation;
	std::string timeStr;
	wasCancelled = false;

	do {
		std::cout << prompt << "(HH:MM format): ";

		if (!getInputWithESC("", timeStr)) {
			wasCancelled = true;
			return 0;
		}

		if (!validation.validateTime(timeStr)) {
			output.println("Invalid time format! Please use HH:MM format (24-hour).", OutputManager::Color::RED);
			continue;
		}

		time_t performanceTime;
		if (!TimeManagement::convertTimeStringToTimeT(timeStr, event.eventDate, performanceTime)) {
			output.println("Failed to convert time. Please try again.", OutputManager::Color::RED);
			continue;
		}

		if (performanceTime < event.startTime) {
			output.println("Performance time must be after event start time (" +
				TimeManagement::formatTimeTToString(event.startTime, "%H:%M") + ")!", OutputManager::Color::RED);
			continue;
		}

		if (performanceTime > event.endTime) {
			output.println("Performance time must be before event end time (" + 
				TimeManagement::formatTimeTToString(event.endTime, "%H:%M") + ")!", OutputManager::Color::RED);
			continue;
		}

		return performanceTime;

	} while (true);
}

void EventSchedular::savePerformance(const Performance& performance) {
	performances.push_back(performance);
	savePerformancesToFile();
}

void EventSchedular::loadPerformances() {
	performances.clear();
	performances = dm.getAllActivePerformances();
}

void EventSchedular::savePerformancesToFile() {
	json j = json::array();

	for (const auto& performance : performances) {
		j.push_back(performance);
	}

	std::ofstream file("performances.json");
	file << j.dump(4);
	file.close();
}

std::string EventSchedular::timeToString(time_t timeValue) {
	struct tm* timeinfo = std::localtime(&timeValue);
	char buffer[20];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", timeinfo);
	return std::string(buffer);
}

time_t EventSchedular::stringToTime(const std::string& timeStr) {
	struct tm tm = {};
	std::istringstream ss(timeStr);
	ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
	return std::mktime(&tm);
}

//---------------------------------------------------------------------------------

void EventSchedular::printMiddle(int line_width, std::string title) {
	std::cout << std::string(line_width, '=') << std::endl;
	int padding = (line_width - title.length()) / 2;
	std::cout << std::string(padding, ' ') << title << std::endl;
	std::cout << std::string(line_width, '=') << std::endl;
}

//---------------------------------------------------------------------------------

void EventSchedular::updatePerformerName(Performance& performance) {
	OutputManager output;
	std::cout << "\nCurrent Performer Name: " << performance.performerName << std::endl;
	
	bool validPerformerName = false;
	while (!validPerformerName) {
		std::cout << "Enter new Performer Name (or press Enter to keep current): ";
		std::string newPerformerName;
		std::getline(std::cin, newPerformerName);

		if (newPerformerName.empty()) {
			std::cout << "Performer Name unchanged." << std::endl;
			return;
		}
		
		// Trim whitespace
		newPerformerName.erase(0, newPerformerName.find_first_not_of(" \t\n\r\f\v"));
		newPerformerName.erase(newPerformerName.find_last_not_of(" \t\n\r\f\v") + 1);
		
		// Check if after trimming it's empty
		if (newPerformerName.empty()) {
			output.println("Performer name cannot be empty! Please enter a valid name or press Enter to keep current.", OutputManager::Color::RED);
			continue;
		}
		
		if (newPerformerName.length() > 50) {
			output.println("Performer name too long! Maximum 50 characters allowed.", OutputManager::Color::RED);
			continue;
		}
		
		if (newPerformerName.length() < 2) {
			output.println("Performer name too short! Minimum 2 characters required.", OutputManager::Color::RED);
			continue;
		}
		
		// Valid input
		performance.performerName = newPerformerName;
		std::cout << "Performer Name updated to: " << performance.performerName << std::endl;
		savePerformancesToFile();
		output.println("? Performer name updated successfully!", OutputManager::Color::BRIGHT_GREEN);
		validPerformerName = true;
	}
}

void EventSchedular::updateStartTime(Performance& performance, const Event& event) {
	OutputManager output;
	Validation validation;
	std::cout << "\nCurrent Start Time: " << timeToString(performance.startTime) << std::endl;
	std::cout << "Do you want to update the start time? (y/n): ";
	std::string confirm;
	std::getline(std::cin, confirm);

	if (confirm == "y" || confirm == "Y" || confirm == "yes" || confirm == "Yes") {
		bool wasCancelled = false;
		time_t newStartTime = getValidatedTimeInputWithESC("Enter new Start Time ", event, wasCancelled);
		
		if (wasCancelled) {
			std::cout << "Start time update cancelled." << std::endl;
			return;
		}

		if (newStartTime >= performance.endTime) {
			output.println("Start time must be before end time!", OutputManager::Color::RED);
			std::cout << "Start time update cancelled." << std::endl;
			return;
		}

		// NEW: Validate performance time conflicts
		Validation::PerformanceTimeValidationStatus timeStatus = validation.validatePerformanceTime(
			newStartTime, 
			performance.endTime, 
			performance.eventId, 
			performances,
			performance.performanceId  // Exclude current performance from validation
		);

		switch (timeStatus) {
		case Validation::PerformanceTimeValidationStatus::EXACT_TIME_MATCH:
			output.println("A performance with the exact same start and end time already exists for this event!", OutputManager::Color::RED);
			std::cout << "Start time update cancelled." << std::endl;
			return;
		case Validation::PerformanceTimeValidationStatus::TIME_CONFLICT:
			output.println("The performance time conflicts with an existing performance for this event!", OutputManager::Color::RED);
			std::cout << "Start time update cancelled." << std::endl;
			return;
		case Validation::PerformanceTimeValidationStatus::VALID:
			performance.startTime = newStartTime;
			std::cout << "Start Time updated to: " << timeToString(performance.startTime) << std::endl;
			savePerformancesToFile();
			output.println("? Performance updated successfully!", OutputManager::Color::BRIGHT_GREEN);
			break;
		}
	}
	else {
		std::cout << "Start Time unchanged." << std::endl;
	}
}

void EventSchedular::updateEndTime(Performance& performance, const Event& event) {
	OutputManager output;
	Validation validation;
	std::cout << "\nCurrent End Time: " << timeToString(performance.endTime) << std::endl;
	std::cout << "Do you want to update the end time? (y/n): ";
	std::string confirm;
	std::getline(std::cin, confirm);
	
	if (confirm == "y" || confirm == "Y" || confirm == "yes" || confirm == "Yes") {
		bool wasCancelled = false;
		time_t newEndTime = getValidatedTimeInputWithESC("Enter new End Time ", event, wasCancelled);
		
		if (wasCancelled) {
			std::cout << "End time update cancelled." << std::endl;
			return;
		}

		if (newEndTime <= performance.startTime) {
			output.println("End time must be after start time!", OutputManager::Color::RED);
			std::cout << "End time update cancelled." << std::endl;
			return;
		}

		// NEW: Validate performance time conflicts
		Validation::PerformanceTimeValidationStatus timeStatus = validation.validatePerformanceTime(
			performance.startTime, 
			newEndTime, 
			performance.eventId, 
			performances,
			performance.performanceId  // Exclude current performance from validation
		);

		switch (timeStatus) {
		case Validation::PerformanceTimeValidationStatus::EXACT_TIME_MATCH:
			output.println("A performance with the exact same start and end time already exists for this event!", OutputManager::Color::RED);
			std::cout << "End time update cancelled." << std::endl;
			return;
		case Validation::PerformanceTimeValidationStatus::TIME_CONFLICT:
			output.println("The performance time conflicts with an existing performance for this event!", OutputManager::Color::RED);
			std::cout << "End time update cancelled." << std::endl;
			return;
		case Validation::PerformanceTimeValidationStatus::VALID:
			performance.endTime = newEndTime;
			std::cout << "End Time updated to: " << timeToString(performance.endTime) << std::endl;
			savePerformancesToFile();
			output.println("? End time updated successfully!", OutputManager::Color::BRIGHT_GREEN);
			break;
		}
	}
	else {
		std::cout << "End Time unchanged." << std::endl;
	}
}

void EventSchedular::updateSongs(Performance& performance) {
	OutputManager output;
	Validation validation;
	std::cout << "\nCurrent Songs (" << performance.songs.size() << "):" << std::endl;
	for (size_t i = 0; i < performance.songs.size(); i++) {
		std::cout << (i + 1) << ". " << performance.songs[i].title
			<< " (ID: " << performance.songs[i].songId << ")" << std::endl;
	}

	std::cout << "\nSong Update Options: " << std::endl;
	std::cout << "1. Add new songs" << std::endl;
	std::cout << "2. Remove all songs and add new ones" << std::endl;
	std::cout << "3. Keep current songs" << std::endl;
	std::cout << "Enter your choice (1-3): ";

	std::string choice;
	std::getline(std::cin, choice);

	if (choice == "1") {
		// Add validation for number of songs
		int numNewSongs = 0;
		bool validInput = false;
		
		while (!validInput) {
			std::cout << "\nHow many additional songs to add? ";
			std::string input;
			std::getline(std::cin, input);
			
			// Trim whitespace and check if empty
			input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
			input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);
			
			if (input.empty()) {
				output.println("Input cannot be empty! Please enter a number.", OutputManager::Color::RED);
				continue;
			}
			
			if (!validation.isInteger(input)) {
				output.println("Invalid input! Please enter a valid number.", OutputManager::Color::RED);
				continue;
			}
			
			numNewSongs = std::stoi(input);
			if (numNewSongs <= 0) {
				output.println("Please enter a positive number!", OutputManager::Color::RED);
				continue;
			}
			if (numNewSongs > 20) {
				output.println("Maximum 20 songs allowed! Please enter a smaller number.", OutputManager::Color::RED);
				continue;
			}
			validInput = true;
		}

		// Add validation for song titles
		for (int i = 0; i < numNewSongs; i++) {
			Song song;
			song.songId = generateSongId();

			std::cout << "\nNew Song " << (i + 1) << ":" << std::endl;
			std::cout << "Song ID: " << song.songId << std::endl;
			
			bool validTitle = false;
			while (!validTitle) {
				std::cout << "Enter Song Title: ";
				std::getline(std::cin, song.title);
				
				// Trim whitespace and check if empty
				song.title.erase(0, song.title.find_first_not_of(" \t\n\r\f\v"));
				song.title.erase(song.title.find_last_not_of(" \t\n\r\f\v") + 1);
				
				if (song.title.empty()) {
					output.println("Song title cannot be empty! Please enter a valid title.", OutputManager::Color::RED);
					continue;
				}
				
				if (song.title.length() > 100) {
					output.println("Song title too long! Maximum 100 characters allowed.", OutputManager::Color::RED);
					continue;
				}
				
				if (song.title.length() < 1) {
					output.println("Song title too short! Please enter a valid title.", OutputManager::Color::RED);
					continue;
				}
				
				validTitle = true;
			}

			performance.songs.push_back(song);
			saveSongToFile(song.songId, song.title, performance.performerName);
		}
		
		savePerformancesToFile();
		output.println("Added " + std::to_string(numNewSongs) + " new songs successfully!", OutputManager::Color::BRIGHT_GREEN);
	}
	else if (choice == "2") {
		// Delete old songs from file
		deleteSongsFromFile(performance.songs);
		performance.songs.clear();
		std::cout << "All previous songs removed." << std::endl;
		
		if (addSongToPerformanceWithESC(performance)) {
			savePerformancesToFile();
			output.println("? Songs updated successfully!", OutputManager::Color::BRIGHT_GREEN);
		}
	}
	else if (choice == "3") {
		std::cout << "Songs unchanged." << std::endl;
		savePerformancesToFile();
		output.println("? Performance updated successfully!", OutputManager::Color::BRIGHT_GREEN);
	}
	else {
		output.println("Invalid choice. Songs unchanged.", OutputManager::Color::RED);
	}
}

void EventSchedular::updateAllFields(Performance& performance, const Event& event) {
	std::cout << "\nUpdating all fields for Performance ID: " << performance.performanceId << std::endl;
	std::cout << "Note: Press Enter for any field to keep the current value.\n" << std::endl;

	updatePerformerName(performance);
	updateStartTime(performance, event);
	updateEndTime(performance, event);
	updateSongs(performance);

	std::cout << "\nAll fields update completed." << std::endl;
}

//---------------------------------------------------------------------------------

void EventSchedular::deleteSpecificPerformanceForEvent() {
	OutputManager output;

	std::cout << "\nEnter performance ID to delete: ";
	std::string performanceId;
	std::getline(std::cin, performanceId);

	auto it = std::find_if(performances.begin(), performances.end(),
		[&performanceId, this](const Performance& perf) {
			return perf.performanceId == performanceId && perf.eventId == selectedEventId;
		});

	if (it != performances.end()) {
		std::cout << "\nAre you sure you want to delete performance '" << it->performanceId
			<< "' by " << it->performerName << "? (y/n): ";
		std::string confirm;
		std::getline(std::cin, confirm);

		if (confirm == "y" || confirm == "Y" || confirm == "yes" || confirm == "Yes") {
			deleteSongsFromFile(it->songs);
			performances.erase(it);

			savePerformancesToFile();
			output.println("? Performance deleted successfully!", OutputManager::Color::BRIGHT_GREEN);
		}
		else {
			std::cout << "Delete operation cancelled." << std::endl;
			std::cout << "\nPress any key to continue...";
			_getch();
		}
	}
	else {
		output.println("Performance with ID '" + performanceId + "' not found in this event!", OutputManager::Color::RED);
		std::cout << "\nPress any key to continue...";
		_getch();
	}
}

void EventSchedular::deleteAllPerformancesForEvent() {
	OutputManager output;
	
	// Count performances for this event
	int eventPerformanceCount = 0;
	for (const auto& perf : performances) {
		if (perf.eventId == selectedEventId && !perf.isDeleted) {
			eventPerformanceCount++;
		}
	}
	
	Event selectedEvent = getEventById(selectedEventId);
	std::cout << "\nAre you sure you want to delete ALL " << eventPerformanceCount
		<< " performances for event '" << selectedEvent.eventName << "'? This action cannot be undone! (y/n): ";
	std::string confirm;
	std::getline(std::cin, confirm);

	if (confirm == "y" || confirm == "Y" || confirm == "yes" || confirm == "Yes") {
		std::cout << "Type 'DELETE ALL' to confirm: ";
		std::string doubleConfirm;
		std::getline(std::cin, doubleConfirm);

		if (doubleConfirm == "DELETE ALL") {
			// Remove performances for this event only
			auto it = performances.begin();
			while (it != performances.end()) {
				if (it->eventId == selectedEventId) {
					deleteSongsFromFile(it->songs);
					it = performances.erase(it);
				} else {
					++it;
				}
			}

			savePerformancesToFile();
			output.println("? All performances for this event deleted successfully!", OutputManager::Color::BRIGHT_GREEN);
		}
		else {
			std::cout << "Delete operation cancelled - confirmation text did not match." << std::endl;
			std::cout << "\nPress any key to continue...";
			_getch();
		}
	}
	else {
		std::cout << "Delete operation cancelled." << std::endl;
		std::cout << "\nPress any key to continue...";
		_getch();
	}
}

//---------------------------------------------------------------------------------

bool EventSchedular::getInputWithESC(const std::string& prompt, std::string& input) {
	OutputManager output;
	if (!prompt.empty()) {
		std::cout << prompt << " (Press ESC to cancel): ";
	}

	input.clear();
	char ch;

	while (true) {
		ch = _getch();

		if (ch == 27) {
			output.println("\nOperation cancelled by user.", OutputManager::Color::YELLOW);
			return false;
		}
		else if (ch == '\r' || ch == '\n') {
			std::cout << std::endl;
			break;
		}
		else if (ch == '\b') {
			if (!input.empty()) {
				input.pop_back();
				std::cout << "\b \b";
			}
		}
		else if (ch >= 32 && ch <= 126) {
			input += ch;
			std::cout << ch;
		}
	}

	return true;
}

bool EventSchedular::addSongToPerformanceWithESC(Performance& performance) {
	OutputManager output;
	Validation validation;
	std::string input;

	// Add validation for number of songs
	int numSongs = 0;
	bool validInput = false;
	
	while (!validInput) {
		std::cout << "\nHow many songs will be performed? ";
		if (!getInputWithESC("", input)) {
			return false; // User pressed ESC
		}
		
		// Trim whitespace and check if empty
		input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
		input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);
		
		if (input.empty()) {
			output.println("Input cannot be empty! Please enter a number.", OutputManager::Color::RED);
			continue;
		}
		
		if (!validation.isInteger(input)) {
			output.println("Invalid input! Please enter a valid number.", OutputManager::Color::RED);
			continue;
		}
		
		numSongs = std::stoi(input);
		if (numSongs <= 0) {
			output.println("Please enter a positive number!", OutputManager::Color::RED);
			continue;
		}
		if (numSongs > 20) {
			output.println("Maximum 20 songs allowed! Please enter a smaller number.", OutputManager::Color::RED);
			continue;
		}
		validInput = true;
	}

	// Add validation for song titles
	for (int i = 0; i < numSongs; i++) {
		Song song;
		song.songId = generateSongId();

		std::cout << "\nSong " << (i + 1) << ":" << std::endl;
		std::cout << "Song ID: " << song.songId << std::endl;
		
		bool validTitle = false;
		while (!validTitle) {
			std::cout << "Enter Song Title: ";
			if (!getInputWithESC("", song.title)) {
				return false; // User pressed ESC
			}
			
			// Trim whitespace and check if empty
			song.title.erase(0, song.title.find_first_not_of(" \t\n\r\f\v"));
			song.title.erase(song.title.find_last_not_of(" \t\n\r\f\v") + 1);
			
			if (song.title.empty()) {
				output.println("Song title cannot be empty! Please enter a valid title.", OutputManager::Color::RED);
				continue;
			}
			
			if (song.title.length() > 100) {
				output.println("Song title too long! Maximum 100 characters allowed.", OutputManager::Color::RED);
				continue;
			}
			
			if (song.title.length() < 1) {
				output.println("Song title too short! Please enter a valid title.", OutputManager::Color::RED);
				continue;
			}
			
			validTitle = true;
		}

		saveSongToFile(song.songId, song.title, performance.performerName);
		performance.songs.push_back(song);
	}

	return true;
}

//---------------------------------------------------------------------------------

void EventSchedular::loadGlobalSongCounter() {
	std::ifstream file("global_song_counter.txt");
	if (file.is_open()) {
		file >> globalSongCounter;
		file.close();
	}
}

void EventSchedular::saveGlobalSongCounter() {
	std::ofstream file("global_song_counter.txt");
	file << globalSongCounter;
	file.close();
}

std::string EventSchedular::generateSongId() {
	globalSongCounter++;
	saveGlobalSongCounter();
	return "SONG" + std::to_string(10000 + globalSongCounter).substr(1);
}

void EventSchedular::saveSongToFile(const std::string& songId, const std::string& title, const std::string& performerName) {
	std::ofstream file("song.txt", std::ios::app);
	if (file.is_open()) {
		file << songId << "|" << title << "|" << performerName << std::endl;
		file.close();
	}
}

void EventSchedular::deleteSongsFromFile(const std::vector<Song>& songs) {
	std::ifstream inputFile("song.txt");
	if (!inputFile.is_open()) {
		return;
	}

	std::vector<std::string> remainingLines;
	std::string line;

	while (std::getline(inputFile, line)) {
		bool shouldKeep = true;

		for (const auto& song : songs) {
			if (line.find(song.songId + "|") == 0) {
				shouldKeep = false;
				break;
			}
		}

		if (shouldKeep) {
			remainingLines.push_back(line);
		}
	}
	inputFile.close();

	std::ofstream outputFile("song.txt");
	if (outputFile.is_open()) {
		for (const auto& remainingLine : remainingLines) {
			outputFile << remainingLine << std::endl;
		}
		outputFile.close();
	}
}

void EventSchedular::deleteAllSongsFromFile() {
	std::ofstream file("song.txt", std::ios::trunc);
	if (file.is_open()) {
		file.close();
	}
}

//---------------------------------------------------------------------------------

void EventSchedular::loadEvents() {
	events = dm.getAllActiveEvents();
}

void EventSchedular::displayCurrentEvents() {
	OutputManager output;
	std::cout << "\n";
	output.printHeader("CURRENT EVENTS");

	std::vector<std::string> headers = { "Event ID", "Event Name", "Date", "Organizer", "Status"};
	std::vector<std::vector<std::string>> eventDate;

	if (events.empty()) {
		output.println("No events available. Please create an event first.", OutputManager::Color::YELLOW);
		return;
	}

	for (const auto& event : events) {
		std::vector<std::string> row;
		row.push_back(event.eventId);
		// Truncate event name if too long
		row.push_back(event.eventName.length() > 24 ?
			event.eventName.substr(0, 21) + "..." : event.eventName);
		row.push_back(timeToString(event.eventDate));
		row.push_back(event.organizerId);

		std::string statusStr;
		switch (event.status) {
		case EventStatus::SCHEDULED:
			statusStr = "SCHEDULED";
			break;
		case EventStatus::ONGOING:
			statusStr = "ONGOING";
			break;
		case EventStatus::COMPLETED:
			statusStr = "COMPLETED";
			break;
		case EventStatus::CANCELLED:
			statusStr = "CANCELLED";
			break;
		default:
			statusStr = "UNKNOWN";
			break;
		}
		row.push_back(statusStr);

		eventDate.push_back(row);
	}
	output.displayTable(headers, eventDate);
}

bool EventSchedular::selectEvent(const std::string& eventId) {
	OutputManager output;

	if (events.empty()) {
		output.println("No events available to select.", OutputManager::Color::RED);
		return false;
	}

	// Find the event with the given ID
	auto it = std::find_if(events.begin(), events.end(),
		[&eventId](const Event& event) {
			return event.eventId == eventId && !event.isDeleted;
		});

	if (it == events.end()) {
		output.println("Event with ID '" + eventId + "' not found!", OutputManager::Color::RED);
		selectedEventId.clear();
		return false;
	}

	selectedEventId = eventId;
	output.println("Selected Event: " + it->eventName + " (ID: " + selectedEventId + ")", OutputManager::Color::BRIGHT_GREEN);
	return true;
}

Event EventSchedular::getEventById(const std::string& eventId) {
	auto it = std::find_if(events.begin(), events.end(),
		[&eventId](const Event& event) {
			return event.eventId == eventId;
		});

	if (it != events.end()) {
		return *it;
	}
	return Event();
}

void EventSchedular::displayPerformanceForEvent() {
	OutputManager output;
	output.printHeader("PERFORMANCE SCHEDULE");

	std::vector<std::string> headers = { "ID", "Event ID", "Performer", "Start Time", "End Time", "Songs" };
	std::vector<std::vector<std::string>> performanceData;

	std::vector<Performance> filteredPerformances;
	if (!selectedEventId.empty()) {
		for (const auto& perf : performances) {
			if (perf.eventId == selectedEventId && !perf.isDeleted) {
				filteredPerformances.push_back(perf);
			}
		}
	}

	for (const auto& perf : filteredPerformances) {
		std::vector<std::string> row;
		row.push_back(perf.performanceId);
		row.push_back(perf.eventId);
		// Truncate performer name if too long
		row.push_back(perf.performerName.length() > 19 ?
			perf.performerName.substr(0, 16) + "..." : perf.performerName);
		row.push_back(timeToString(perf.startTime));
		row.push_back(timeToString(perf.endTime));
		row.push_back(std::to_string(perf.songs.size()));

		performanceData.push_back(row);
	}
	output.displayTable(headers, performanceData);

	if (filteredPerformances.empty()) {
		if (selectedEventId.empty()) {
			output.println("Please select an event to view its performances.", OutputManager::Color::YELLOW);
		}
		else {
			output.println("No performances scheduled for this event yet.", OutputManager::Color::YELLOW);
		}
	}
}

//---------------------------------------------------------------------------------

void EventSchedular::runForCustomer(const std::string& eventId) {
	OutputManager output;
	
	// Load events and performances
	loadEvents();
	loadPerformances();
	
	// Validate the event ID exists
	auto eventIt = std::find_if(events.begin(), events.end(),
		[&eventId](const Event& event) {
			return event.eventId == eventId && !event.isDeleted;
		});
	
	if (eventIt == events.end()) {
		output.println("Event not found or no longer available.", OutputManager::Color::RED);
		return;
	}
	
	// Set the selected event ID directly
	selectedEventId = eventId;
	
	system("cls");
	output.printHeader("PERFORMANCE SCHEDULE");
	std::cout << "Event: " << eventId << " (ID: " << eventId << ")" << std::endl;
	std::cout << "Customer View - Performance Details Only" << std::endl;
	
	// Display performance details for this specific event
	displayPerformanceDetails();
	
	std::cout << "\nPress any key to return to menu...";
	_getch();
}