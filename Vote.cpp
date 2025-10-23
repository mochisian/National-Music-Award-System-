#include "Vote.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <set>
#include <map>

namespace Voting {

	VoteSystem::VoteSystem(const std::string& userId, DataManager& dm)
		: userId(userId), dm(dm) {
	}

	void VoteSystem::run() {
		showMainMenu(time(0));
	}

	void VoteSystem::run(time_t currentDateTime) {
		showMainMenu(currentDateTime);
	}

	void VoteSystem::runForSpecificEvent(const std::string& eventId, const std::string& registrationId, time_t currentDateTime) {
		showEventSpecificMenu(eventId, registrationId, currentDateTime);
	}

	bool VoteSystem::isValidInteger(const std::string& str) {
		if (str.empty()) return false;

		size_t start = 0;
		if (str[0] == '+' || str[0] == '-') {
			start = 1;
			if (str.length() == 1) return false;
		}

		for (size_t i = start; i < str.length(); i++) {
			if (!std::isdigit(str[i])) {
				return false;
			}
		}
		return true;
	}

	bool VoteSystem::canVoteForEvent(const std::string& eventId, time_t currentDateTime) {
		Event event = getEventById(eventId);

		if (event.eventId.empty()) {
			output.println("Event not found!", OutputManager::Color::RED);
			return false;
		}

		if (event.status != EventStatus::ONGOING) {
			output.println("Voting is not available for event '" + event.eventName + "'. Event must be in 'ONGOING' status to vote.", OutputManager::Color::RED);
			std::string statusStr;
			switch (event.status) {
			case EventStatus::SCHEDULED:
				statusStr = "Scheduled";
				break;
			case EventStatus::COMPLETED:
				statusStr = "Completed";
				break;
			case EventStatus::CANCELLED:
				statusStr = "Cancelled";
				break;
			default:
				statusStr = "Unknown";
				break;
			}
			output.println("Current event status: " + statusStr, OutputManager::Color::YELLOW);
			return false;
		}

		if (currentDateTime < event.startTime || currentDateTime > event.endTime) {
			output.println("Voting is not available outside event hours.", OutputManager::Color::RED);
			output.println("Current time: " + TimeManagement::formatTimeTToString(currentDateTime, "%Y-%m-%d %H:%M:%S"), OutputManager::Color::CYAN);
			output.println("Event period: " +
				TimeManagement::formatTimeTToString(event.startTime, "%Y-%m-%d %H:%M") +
				" to " +
				TimeManagement::formatTimeTToString(event.endTime, "%Y-%m-%d %H:%M"),
				OutputManager::Color::CYAN);
			return false;
		}

		return true;
	}

	void VoteSystem::showMainMenu(time_t currentDateTime) {
		bool isMenuActive = true;
		while (isMenuActive) {
			system("cls");
			output.printHeader("Vote System");

			output.println("=== BASIC VOTING ===");
			output.println("1. Quick Voting");

			output.println("\n=== RESULTS & ANALYTICS ===");
			output.println("2. View Voting Results & Rankings");
			output.println("3. Advanced Analytics");
			output.println("4. Export Results");

			output.println("\n=== PERSONAL ===");
			output.println("5. My Voting History");
			output.println("6. Back to Main Menu");

			output.print("Enter your choice (1-6): ");
			std::string choice;
			std::getline(std::cin, choice);

			if (!isValidInteger(choice)) {
				output.println("Invalid input! Please enter a whole number only (1-6).", OutputManager::Color::RED);
				system("pause");
				continue;
			}

			try {
				int choiceNum = std::stoi(choice);
				switch (choiceNum) {
				case 1:
					castVotes(currentDateTime);
					break;
				case 2:
					viewVotingResults();
					break;
				case 3:
					voteAnalytics();
					break;
				case 4:
					exportVotingResults();
					break;
				case 5:
					viewMyVotes();
					break;
				case 6:
					isMenuActive = false;
					break;
				default:
					output.println("Invalid input! Please enter a whole number only (1-6).", OutputManager::Color::RED);
					system("pause");
					break;
				}
			}
			catch (...) {
				output.println("Invalid input! Please enter a whole number only (1-6).", OutputManager::Color::RED);
				system("pause");
			}
		}
	}

	void VoteSystem::showEventSpecificMenu(const std::string& eventId, const std::string& registrationId, time_t currentDateTime) {
		bool isEventMenuActive = true;
		while (isEventMenuActive) {
			system("cls");
			Event event = getEventById(eventId);
			output.printHeader("Vote for Songs - " + event.eventName);

			output.println("=== VOTING FOR THIS EVENT ===");
			output.println("1. Cast Your Votes");
			output.println("2. View Results for This Event");
			output.println("3. My Voting History for This Event");
			output.println("4. Back to Event Menu");

			output.print("Enter your choice (1-4): ");
			std::string choice;
			std::getline(std::cin, choice);

			if (!isValidInteger(choice)) {
				output.println("Invalid input! Please enter a whole number only (1-4).", OutputManager::Color::RED);
				system("pause");
				continue;
			}

			try {
				int choiceNum = std::stoi(choice);
				switch (choiceNum) {
				case 1:
					processVotingForSpecificRegistration(registrationId, eventId, currentDateTime);
					break;
				case 2:
					viewEventSpecificResults(event);
					break;
				case 3:
				{
					system("cls");
					output.printHeader("My Voting History - " + event.eventName);

					auto votes = getActiveVotesForUser();
					auto songs = loadSongsFromPerformancesForEvent(eventId);

					std::vector<Vote> myEventVotes;
					for (const auto& vote : votes) {
						if (vote.registrationId == registrationId) {
							myEventVotes.push_back(vote);
						}
					}

					if (myEventVotes.empty()) {
						output.println("You haven't cast any votes for this event yet.", OutputManager::Color::YELLOW);
					}
					else {
						std::vector<std::string> headers = { "#", "Song Title", "Performer", "Weight" };
						std::vector<std::vector<std::string>> rows;

						std::map<std::string, int> songVoteWeights;
						for (const auto& vote : myEventVotes) {
							songVoteWeights[vote.songId] += vote.voteWeight;
						}

						int rowNumber = 1;
						for (const auto& songWeight : songVoteWeights) {
							std::string songTitle = "Unknown";
							std::string performer = "Unknown";

							for (const auto& song : songs) {
								if (song.songId == songWeight.first) {
									songTitle = song.title;
									performer = song.performer;
									break;
								}
							}

							std::vector<std::string> row = {
								std::to_string(rowNumber),
								songTitle,
								performer,
								std::to_string(songWeight.second)
							};
							rows.push_back(row);
							rowNumber++;
						}

						output.displayTable(headers, rows);
					}
					system("pause");
				}
				break;
				case 4:
					isEventMenuActive = false;
					break;
				default:
					output.println("Invalid input! Please enter a whole number only (1-4).", OutputManager::Color::RED);
					system("pause");
					break;
				}
			}
			catch (...) {
				output.println("Invalid input! Please enter a whole number only (1-4).", OutputManager::Color::RED);
				system("pause");
			}
		}
	}

	void VoteSystem::processVotingForSpecificRegistration(const std::string& registrationId, const std::string& eventId, time_t currentDateTime) {
		if (!canVoteForEvent(eventId, currentDateTime)) {
			system("pause");
			return;
		}

		Registration registration = dm.getRegistrationFromRegistrationId(registrationId);
		if (registration.registrationId.empty()) {
			output.println("Registration not found!", OutputManager::Color::RED);
			system("pause");
			return;
		}

		if (isRegistrationRefunded(registrationId)) {
			output.println("Cannot vote with a refunded registration!", OutputManager::Color::RED);
			system("pause");
			return;
		}

		output.printHeader("Voting Session");
		Event event = getEventById(eventId);

		auto votes = getActiveVotesForUser();
		int votesAlreadyCast = 0;
		for (const auto& vote : votes) {
			if (vote.registrationId == registrationId) {
				votesAlreadyCast++;
			}
		}

		int remainingVotes = static_cast<int>(registration.seats.size()) - votesAlreadyCast;

		output.println("Event: " + event.eventName);
		output.println("Registration ID: " + registrationId);
		output.println("Total seats: " + std::to_string(registration.seats.size()));
		output.println("Votes already cast: " + std::to_string(votesAlreadyCast));
		output.println("Remaining votes: " + std::to_string(remainingVotes));
		output.println("========================================");

		auto songs = loadSongsFromPerformancesForEvent(eventId);

		if (songs.empty()) {
			output.println("No songs available for this event.", OutputManager::Color::RED);
			output.println("Event ID: " + eventId, OutputManager::Color::CYAN);
			system("pause");
			return;
		}

		if (remainingVotes <= 0) {
			output.println("You have already cast all available votes for this registration.", OutputManager::Color::YELLOW);
			system("pause");
			return;
		}

		for (int voteIndex = 0; voteIndex < remainingVotes; voteIndex++) {
			system("cls");
			output.printHeader("Vote " + std::to_string(votesAlreadyCast + voteIndex + 1) +
				" of " + std::to_string(registration.seats.size()));

			int seatIndex = votesAlreadyCast + voteIndex;
			if (seatIndex < static_cast<int>(registration.seats.size())) {
				output.println("Seat: " + registration.seats[seatIndex].seatId +
					" (Row " + registration.seats[seatIndex].row +
					", Column " + registration.seats[seatIndex].column + ")");
			}
			output.println("Event: " + event.eventName);
			output.println("Remaining votes after this: " + std::to_string(remainingVotes - voteIndex - 1));
			output.println("========================================");

			displaySongs(songs);

			output.print("Select song to vote for (1-" + std::to_string(songs.size()) + ") or 'c' to cancel: ");
			std::string voteChoice;
			std::getline(std::cin, voteChoice);

			if (validation.shouldCancel(voteChoice)) {
				output.println("Voting cancelled.", OutputManager::Color::YELLOW);
				system("pause");
				return;
			}

			if (!isValidInteger(voteChoice)) {
				output.println("Invalid input! Please enter a whole number only (1-" +
					std::to_string(songs.size()) + ").", OutputManager::Color::RED);
				voteIndex--;
				system("pause");
				continue;
			}

			try {
				int songChoice = std::stoi(voteChoice) - 1;
				if (songChoice < 0 || songChoice >= static_cast<int>(songs.size())) {
					output.println("Invalid input! Please enter a whole number only (1-" +
						std::to_string(songs.size()) + ").", OutputManager::Color::RED);
					voteIndex--;
					system("pause");
					continue;
				}

				Vote vote;
				vote.voteId = generateVoteId();
				vote.songId = songs[songChoice].songId;
				vote.registrationId = registrationId;
				vote.voteWeight = 1;
				vote.isDeleted = false;

				auto allVotes = dm.getAllActiveVotes();
				allVotes.push_back(vote);
				dm.saveData(allVotes, dm.voteFileName);

				output.println("Vote cast for: " + songs[songChoice].title + " by " + songs[songChoice].performer,
					OutputManager::Color::GREEN);

				if (voteIndex < remainingVotes - 1) {
					output.println("Press Enter to continue to next vote...");
					std::cin.get();
				}

			}
			catch (...) {
				output.println("Invalid input! Please enter a whole number only (1-" +
					std::to_string(songs.size()) + ").", OutputManager::Color::RED);
				voteIndex--;
				system("pause");
				continue;
			}
		}

		output.println("\n========================================");
		output.println("All remaining votes cast successfully!", OutputManager::Color::GREEN);
		output.println("Thank you for participating in the voting!");
		system("pause");
	}

	void VoteSystem::castVotes(time_t currentDateTime) {
		system("cls");
		output.printHeader("Quick Voting");

		auto registrations = getUserRegistrations();

		if (registrations.empty()) {
			output.println("You don't have any event registrations.", OutputManager::Color::YELLOW);
			output.println("Please register for an event first to participate in voting.", OutputManager::Color::CYAN);
			system("pause");
			return;
		}

		std::vector<Registration> eligibleRegistrations;
		for (const auto& reg : registrations) {
			if (hasCompletedPayment(reg.registrationId) && !isRegistrationRefunded(reg.registrationId)) {
				if (canVoteForEvent(reg.eventId, currentDateTime)) {
					eligibleRegistrations.push_back(reg);
				}
			}
		}

		if (eligibleRegistrations.empty()) {
			output.println("No eligible registrations found.", OutputManager::Color::YELLOW);
			output.println("Reasons could be:", OutputManager::Color::YELLOW);
			output.println("- Payment not completed", OutputManager::Color::YELLOW);
			output.println("- Event not in 'ONGOING' status", OutputManager::Color::YELLOW);
			output.println("- Not within event time period", OutputManager::Color::YELLOW);
			output.println("- Registration has been refunded", OutputManager::Color::YELLOW);

			output.println("\nFound registrations but not eligible:", OutputManager::Color::CYAN);
			for (const auto& reg : registrations) {
				Event event = getEventById(reg.eventId);
				std::string statusStr = "";
				switch (event.status) {
				case EventStatus::SCHEDULED:
					statusStr = "Scheduled";
					break;
				case EventStatus::ONGOING:
					statusStr = "Ongoing";
					break;
				case EventStatus::COMPLETED:
					statusStr = "Completed";
					break;
				case EventStatus::CANCELLED:
					statusStr = "Cancelled";
					break;
				default:
					statusStr = "Unknown";
					break;
				}

				output.println("- Registration: " + reg.registrationId +
					" | Event: " + event.eventName +
					" | Payment: " + (hasCompletedPayment(reg.registrationId) ? "Completed" : "Pending") +
					" | Status: " + statusStr +
					" | Refunded: " + (isRegistrationRefunded(reg.registrationId) ? "Yes" : "No"),
					OutputManager::Color::CYAN);
			}

			system("pause");
			return;
		}

		displayRegistrations(eligibleRegistrations);

		output.print("Select registration to vote with (1-" + std::to_string(eligibleRegistrations.size()) + ") or 'c' to cancel: ");
		std::string choiceStr;
		std::getline(std::cin, choiceStr);

		if (validation.shouldCancel(choiceStr)) return;

		if (!isValidInteger(choiceStr)) {
			output.println("Invalid input! Please enter a whole number only (1-" +
				std::to_string(eligibleRegistrations.size()) + ").", OutputManager::Color::RED);
			system("pause");
			return;
		}

		try {
			int choice = std::stoi(choiceStr) - 1;
			if (choice < 0 || choice >= static_cast<int>(eligibleRegistrations.size())) {
				output.println("Invalid input! Please enter a whole number only (1-" +
					std::to_string(eligibleRegistrations.size()) + ").", OutputManager::Color::RED);
				system("pause");
				return;
			}

			Registration selectedReg = eligibleRegistrations[choice];

			if (hasAlreadyVoted(selectedReg.registrationId)) {
				output.println("You have already cast all available votes for this registration.", OutputManager::Color::YELLOW);
				output.println("All seats have been used for voting.", OutputManager::Color::CYAN);
				system("pause");
				return;
			}

			processVotingForRegistration(selectedReg, currentDateTime);

		}
		catch (...) {
			output.println("Invalid input! Please enter a whole number only (1-" +
				std::to_string(eligibleRegistrations.size()) + ").", OutputManager::Color::RED);
			system("pause");
		}
	}

	void VoteSystem::processVotingForRegistration(const Registration& registration, time_t currentDateTime) {
		if (!canVoteForEvent(registration.eventId, currentDateTime)) {
			system("pause");
			return;
		}

		if (isRegistrationRefunded(registration.registrationId)) {
			output.println("Cannot vote with a refunded registration!", OutputManager::Color::RED);
			system("pause");
			return;
		}

		output.printHeader("Voting Session");
		Event event = getEventById(registration.eventId);

		auto votes = getActiveVotesForUser();
		int votesAlreadyCast = 0;
		for (const auto& vote : votes) {
			if (vote.registrationId == registration.registrationId) {
				votesAlreadyCast++;
			}
		}

		int remainingVotes = static_cast<int>(registration.seats.size()) - votesAlreadyCast;

		output.println("Event: " + event.eventName);
		output.println("Registration ID: " + registration.registrationId);
		output.println("Total seats: " + std::to_string(registration.seats.size()));
		output.println("Votes already cast: " + std::to_string(votesAlreadyCast));
		output.println("Remaining votes: " + std::to_string(remainingVotes));
		output.println("========================================");

		auto songs = loadSongsFromPerformancesForEvent(registration.eventId);

		if (songs.empty()) {
			output.println("No songs available for this event.", OutputManager::Color::RED);
			output.println("Event ID: " + registration.eventId, OutputManager::Color::CYAN);
			system("pause");
			return;
		}

		for (int voteIndex = 0; voteIndex < remainingVotes; voteIndex++) {
			system("cls");
			output.printHeader("Vote " + std::to_string(votesAlreadyCast + voteIndex + 1) +
				" of " + std::to_string(registration.seats.size()));

			int seatIndex = votesAlreadyCast + voteIndex;
			if (seatIndex < static_cast<int>(registration.seats.size())) {
				output.println("Seat: " + registration.seats[seatIndex].seatId +
					" (Row " + registration.seats[seatIndex].row +
					", Column " + registration.seats[seatIndex].column + ")");
			}
			output.println("Event: " + event.eventName);
			output.println("Remaining votes after this: " + std::to_string(remainingVotes - voteIndex - 1));
			output.println("========================================");

			displaySongs(songs);

			output.print("Select song to vote for (1-" + std::to_string(songs.size()) + ") or 'c' to cancel: ");
			std::string voteChoice;
			std::getline(std::cin, voteChoice);

			if (validation.shouldCancel(voteChoice)) {
				output.println("Voting cancelled.", OutputManager::Color::YELLOW);
				system("pause");
				return;
			}

			if (!isValidInteger(voteChoice)) {
				output.println("Invalid input! Please enter a whole number only (1-" +
					std::to_string(songs.size()) + ").", OutputManager::Color::RED);
				voteIndex--;
				system("pause");
				continue;
			}

			try {
				int songChoice = std::stoi(voteChoice) - 1;
				if (songChoice < 0 || songChoice >= static_cast<int>(songs.size())) {
					output.println("Invalid song selection!", OutputManager::Color::RED);
					voteIndex--;
					system("pause");
					continue;
				}

				Vote vote;
				vote.voteId = generateVoteId();
				vote.songId = songs[songChoice].songId;
				vote.registrationId = registration.registrationId;
				vote.voteWeight = 1;
				vote.isDeleted = false;

				auto allVotes = dm.getAllActiveVotes();
				allVotes.push_back(vote);
				dm.saveData(allVotes, dm.voteFileName);

				output.println("Vote cast for: " + songs[songChoice].title + " by " + songs[songChoice].performer,
					OutputManager::Color::GREEN);

				if (voteIndex < remainingVotes - 1) {
					output.println("Press Enter to continue to next vote...");
					std::cin.get();
				}

			}
			catch (...) {
				output.println("Invalid input! Please enter a whole number only.", OutputManager::Color::RED);
				voteIndex--;
				system("pause");
			}
		}

		output.println("\n========================================");
		output.println("All remaining votes cast successfully!", OutputManager::Color::GREEN);
		output.println("Thank you for participating in the voting!");
		system("pause");
	}

	void VoteSystem::viewVotingResults() {
		system("cls");
		output.printHeader("Voting Results & Rankings");

		auto eventsWithVotes = getEventsWithVotes();

		if (eventsWithVotes.empty()) {
			output.println("No votes have been cast yet.", OutputManager::Color::YELLOW);
			system("pause");
			return;
		}

		output.println("SELECT EVENT TO VIEW RESULTS:");
		output.println("=============================");

		for (size_t i = 0; i < eventsWithVotes.size(); i++) {
			const auto& eventData = eventsWithVotes[i];
			output.println(std::to_string(i + 1) + ". " + eventData.first.eventName +
				" (" + std::to_string(eventData.second) + " votes)");
		}

		output.println(std::to_string(eventsWithVotes.size() + 1) + ". View All Events Combined");
		output.println(std::to_string(eventsWithVotes.size() + 2) + ". Back");

		output.print("Select option (1-" + std::to_string(eventsWithVotes.size() + 2) + "): ");
		std::string choice;
		std::getline(std::cin, choice);

		if (!isValidInteger(choice)) {
			output.println("Invalid input! Please enter a whole number only (1-" +
				std::to_string(eventsWithVotes.size() + 2) + ").", OutputManager::Color::RED);
			system("pause");
			return;
		}

		try {
			int choiceNum = std::stoi(choice);
			if (choiceNum >= 1 && choiceNum <= static_cast<int>(eventsWithVotes.size())) {
				Event selectedEvent = eventsWithVotes[choiceNum - 1].first;
				viewEventSpecificResults(selectedEvent);
			}
			else if (choiceNum == static_cast<int>(eventsWithVotes.size()) + 1) {
				viewCombinedResults();
			}
			else if (choiceNum == static_cast<int>(eventsWithVotes.size()) + 2) {
				return;
			}
			else {
				output.println("Invalid input! Please enter a whole number only (1-" +
					std::to_string(eventsWithVotes.size() + 2) + ").", OutputManager::Color::RED);
				system("pause");
			}
		}
		catch (...) {
			output.println("Invalid input! Please enter a whole number only (1-" +
				std::to_string(eventsWithVotes.size() + 2) + ").", OutputManager::Color::RED);
			system("pause");
		}
	}

	std::vector<std::pair<Event, int>> VoteSystem::getEventsWithVotes() {
		std::vector<std::pair<Event, int>> eventsWithVotes;
		auto votes = getActiveVotesForUser();
		auto registrations = dm.getAllActiveRegistrations();
		auto events = dm.getAllActiveEvents();

		std::map<std::string, int> votesPerEvent;
		for (const auto& vote : votes) {
			for (const auto& reg : registrations) {
				if (reg.registrationId == vote.registrationId) {
					votesPerEvent[reg.eventId]++;
					break;
				}
			}
		}

		for (const auto& event : events) {
			if (votesPerEvent[event.eventId] > 0) {
				eventsWithVotes.push_back(std::make_pair(event, votesPerEvent[event.eventId]));
			}
		}

		return eventsWithVotes;
	}

	void VoteSystem::viewEventSpecificResults(const Event& event) {
		system("cls");
		output.printHeader("Voting Results for " + event.eventName);

		auto results = calculateVoteResultsForEvent(event.eventId);

		if (results.empty()) {
			output.println("No votes found for this event.", OutputManager::Color::YELLOW);
			system("pause");
			return;
		}

		std::sort(results.begin(), results.end(),
			[](const VoteResult& a, const VoteResult& b) {
				return a.totalVoteWeight > b.totalVoteWeight;
			});

		output.println("Event: " + event.eventName, OutputManager::Color::CYAN);
		output.println("Event ID: " + event.eventId, OutputManager::Color::CYAN);
		output.println("========================================");

		displayVoteResults(results);
		system("pause");
	}

	void VoteSystem::viewCombinedResults() {
		system("cls");
		output.printHeader("Combined Voting Results (All Events)");

		output.println("Note: Results shown are combined from all events", OutputManager::Color::YELLOW);
		output.println("========================================");

		auto results = calculateVoteResults();

		if (results.empty()) {
			output.println("No votes have been cast yet.", OutputManager::Color::YELLOW);
			system("pause");
			return;
		}

		std::sort(results.begin(), results.end(),
			[](const VoteResult& a, const VoteResult& b) {
				return a.totalVoteWeight > b.totalVoteWeight;
			});

		displayVoteResults(results);
		system("pause");
	}

	std::vector<VoteResult> VoteSystem::calculateVoteResultsForEvent(const std::string& eventId) {
		auto votes = getActiveVotesForUser();
		auto registrations = dm.getAllActiveRegistrations();
		auto songs = loadSongsFromPerformancesForEvent(eventId);

		std::vector<Vote> eventVotes;
		for (const auto& vote : votes) {
			for (const auto& reg : registrations) {
				if (reg.registrationId == vote.registrationId && reg.eventId == eventId) {
					eventVotes.push_back(vote);
					break;
				}
			}
		}

		std::map<std::string, VoteResult> resultMap;

		for (const auto& song : songs) {
			VoteResult result;
			result.songId = song.songId;
			result.title = song.title;
			result.performer = song.performer;
			result.totalVotes = 0;
			result.totalVoteWeight = 0;
			result.percentage = 0.0;
			resultMap[song.songId] = result;
		}

		for (const auto& vote : eventVotes) {
			if (resultMap.find(vote.songId) != resultMap.end()) {
				resultMap[vote.songId].totalVotes++;
				resultMap[vote.songId].totalVoteWeight += vote.voteWeight;
			}
		}

		int totalVoteWeight = 0;
		for (const auto& pair : resultMap) {
			totalVoteWeight += pair.second.totalVoteWeight;
		}

		std::vector<VoteResult> results;
		for (auto& pair : resultMap) {
			if (pair.second.totalVoteWeight > 0) {
				if (totalVoteWeight > 0) {
					pair.second.percentage = (static_cast<double>(pair.second.totalVoteWeight) / static_cast<double>(totalVoteWeight)) * 100.0;
				}
				results.push_back(pair.second);
			}
		}

		return results;
	}

	void VoteSystem::voteAnalytics() {
		system("cls");
		output.printHeader("Advanced Voting Analytics Dashboard");

		auto votes = getActiveVotesForUser();
		auto songs = loadSongsFromPerformances();

		if (votes.empty()) {
			output.println("No voting data available.", OutputManager::Color::YELLOW);
			system("pause");
			return;
		}

		int totalVotes = static_cast<int>(votes.size());
		int totalVoteWeight = 0;
		std::map<std::string, int> songVoteCount;
		std::map<std::string, int> userVoteCount;

		for (const auto& vote : votes) {
			totalVoteWeight += vote.voteWeight;
			songVoteCount[vote.songId]++;

			auto registrations = dm.getAllActiveRegistrations();
			for (const auto& reg : registrations) {
				if (reg.registrationId == vote.registrationId) {
					userVoteCount[reg.customerId]++;
					break;
				}
			}
		}

		output.println("COMPREHENSIVE VOTING STATISTICS");
		output.println("================================");
		output.println("Total Votes Cast: " + std::to_string(totalVotes));
		output.println("Total Vote Weight: " + std::to_string(totalVoteWeight));
		output.println("Unique Voters: " + std::to_string(userVoteCount.size()));
		output.println("Songs Receiving Votes: " + std::to_string(songVoteCount.size()));

		if (!userVoteCount.empty()) {
			double avgVotesPerUser = static_cast<double>(totalVotes) / static_cast<double>(userVoteCount.size());
			double avgWeightPerVote = static_cast<double>(totalVoteWeight) / static_cast<double>(totalVotes);
			output.println("Average Votes per User: " + std::to_string(static_cast<int>(avgVotesPerUser)));

			std::ostringstream oss;
			oss << std::fixed << std::setprecision(1) << avgWeightPerVote;
			output.println("Average Weight per Vote: " + oss.str());
		}

		output.println("\nTOP PERFORMING SONGS");
		output.println("====================");

		std::vector<std::pair<std::string, int>> sortedSongs;
		for (const auto& pair : songVoteCount) {
			sortedSongs.push_back(pair);
		}

		std::sort(sortedSongs.begin(), sortedSongs.end(),
			[](const auto& a, const auto& b) { return a.second > b.second; });

		for (size_t i = 0; i < std::min(size_t(10), sortedSongs.size()); i++) {
			const auto& songPair = sortedSongs[i];

			std::string songTitle = "Unknown";
			std::string performer = "Unknown";
			for (const auto& song : songs) {
				if (song.songId == songPair.first) {
					songTitle = song.title;
					performer = song.performer;
					break;
				}
			}

			double percentage = 0.0;
			if (totalVotes > 0) {
				percentage = (static_cast<double>(songPair.second) / static_cast<double>(totalVotes)) * 100.0;
			}

			output.println(std::to_string(i + 1) + ". " + songTitle + " - " + performer);
			output.println("   Votes: " + std::to_string(songPair.second) +
				" (" + std::to_string(static_cast<int>(percentage)) + "%)");
		}

		system("pause");
	}

	void VoteSystem::exportVotingResults() {
		system("cls");
		output.printHeader("Export Voting Results");

		auto results = calculateVoteResults();
		if (results.empty()) {
			output.println("No voting data to export.", OutputManager::Color::YELLOW);
			system("pause");
			return;
		}

		std::sort(results.begin(), results.end(),
			[](const VoteResult& a, const VoteResult& b) {
				return a.totalVoteWeight > b.totalVoteWeight;
			});

		std::string filename = "voting_results.txt";

		std::ofstream file(filename);
		if (!file.is_open()) {
			output.println("Error: Could not create export file.", OutputManager::Color::RED);
			system("pause");
			return;
		}

		file << "VOTING RESULTS EXPORT\n";
		file << "=====================\n\n";
		file << "Export Date: " << TimeManagement::formatTimeTToString(time(0), "%Y-%m-%d %H:%M:%S") << "\n\n";

		int maxTitleLength = 10;
		int maxPerformerLength = 9;

		for (const auto& result : results) {
			if (static_cast<int>(result.title.length()) > maxTitleLength) {
				maxTitleLength = static_cast<int>(result.title.length());
			}
			if (static_cast<int>(result.performer.length()) > maxPerformerLength) {
				maxPerformerLength = static_cast<int>(result.performer.length());
			}
		}

		maxTitleLength += 2;
		maxPerformerLength += 2;

		file << std::left
			<< std::setw(6) << "RANK"
			<< std::setw(maxTitleLength) << "SONG TITLE"
			<< std::setw(maxPerformerLength) << "PERFORMER"
			<< std::setw(8) << "VOTES"
			<< std::setw(8) << "WEIGHT"
			<< std::setw(12) << "PERCENTAGE" << "\n";

		file << std::string(6, '=') << " "
			<< std::string(maxTitleLength - 1, '=') << " "
			<< std::string(maxPerformerLength - 1, '=') << " "
			<< std::string(7, '=') << " "
			<< std::string(7, '=') << " "
			<< std::string(11, '=') << "\n";

		for (size_t i = 0; i < results.size(); i++) {
			const auto& result = results[i];

			file << std::left
				<< std::setw(6) << (i + 1)
				<< std::setw(maxTitleLength) << result.title
				<< std::setw(maxPerformerLength) << result.performer
				<< std::setw(8) << result.totalVotes
				<< std::setw(8) << result.totalVoteWeight
				<< std::fixed << std::setprecision(1) << result.percentage << "%\n";
		}

		file << "\n\nSUMMARY STATISTICS\n";
		file << "==================\n";

		int totalVotes = 0;
		int totalWeight = 0;
		for (const auto& result : results) {
			totalVotes += result.totalVotes;
			totalWeight += result.totalVoteWeight;
		}

		file << "Total Votes Cast: " << totalVotes << "\n";
		file << "Total Vote Weight: " << totalWeight << "\n";
		file << "Songs with Votes: " << results.size() << "\n";

		if (!results.empty() && results[0].totalVoteWeight > 0) {
			file << "\nWINNING SONG\n";
			file << "============\n";
			file << "Title: " << results[0].title << "\n";
			file << "Performer: " << results[0].performer << "\n";
			file << "Votes: " << results[0].totalVotes << "\n";
			file << "Weight: " << results[0].totalVoteWeight << "\n";
			file << "Percentage: " << std::fixed << std::setprecision(1) << results[0].percentage << "%\n";
		}

		file.close();

		output.println("Results exported successfully!", OutputManager::Color::GREEN);
		output.println("File saved as: " + filename);
		output.println("Location: Current directory");
		system("pause");
	}

	void VoteSystem::viewMyVotes() {
		system("cls");
		output.printHeader("My Voting History");
		displayMyVotingHistory();
		system("pause");
	}

	bool VoteSystem::isRegistrationRefunded(const std::string& registrationId) {
		auto payments = dm.getAllActivePayments();
		for (const auto& payment : payments) {
			if (payment.registrationId == registrationId &&
				payment.paymentStatus == PaymentStatus::REFUNDED) {
				return true;
			}
		}
		return false;
	}

	std::vector<Vote> VoteSystem::getActiveVotesForUser() {
		auto votes = dm.getAllActiveVotes();
		auto registrations = dm.getAllActiveRegistrations();
		std::vector<Vote> activeVotes;

		for (const auto& vote : votes) {
			for (const auto& reg : registrations) {
				if (reg.registrationId == vote.registrationId) {
					if (!isRegistrationRefunded(reg.registrationId)) {
						activeVotes.push_back(vote);
					}
					break;
				}
			}
		}

		return activeVotes;
	}

	std::vector<Registration> VoteSystem::getUserRegistrations() {
		auto registrations = dm.getAllActiveRegistrations();
		std::vector<Registration> userRegistrations;

		for (const auto& reg : registrations) {
			if (!reg.customerId.empty() && reg.customerId == userId &&
				reg.registrationStatus == RegistrationStatus::COMPLETED) {
				userRegistrations.push_back(reg);
			}
		}

		return userRegistrations;
	}

	std::vector<SongInfo> VoteSystem::loadSongsFromFile() {
		return std::vector<SongInfo>();
	}

	std::vector<SongInfo> VoteSystem::loadSongsFromPerformances() {
		std::vector<SongInfo> songs;
		auto performances = dm.getAllActivePerformances();

		for (const auto& perf : performances) {
			for (const auto& song : perf.songs) {
				SongInfo songInfo;
				songInfo.songId = song.songId;
				songInfo.title = song.title;
				songInfo.performer = perf.performerName;
				songs.push_back(songInfo);
			}
		}

		return songs;
	}

	std::vector<SongInfo> VoteSystem::loadSongsFromPerformancesForEvent(const std::string& eventId) {
		std::vector<SongInfo> songs;
		auto performances = dm.getAllActivePerformances();

		for (const auto& perf : performances) {
			if (perf.eventId == eventId) {
				for (const auto& song : perf.songs) {
					SongInfo songInfo;
					songInfo.songId = song.songId;
					songInfo.title = song.title;
					songInfo.performer = perf.performerName;
					songs.push_back(songInfo);
				}
			}
		}

		return songs;
	}

	bool VoteSystem::hasCompletedPayment(const std::string& registrationId) {
		auto payments = dm.getAllActivePayments();

		for (const auto& payment : payments) {
			if (payment.registrationId == registrationId &&
				payment.paymentStatus == PaymentStatus::COMPLETED) {
				return true;
			}
		}

		return true;
	}

	bool VoteSystem::hasAlreadyVoted(const std::string& registrationId) {
		auto votes = getActiveVotesForUser();
		auto registrations = dm.getAllActiveRegistrations();

		int totalSeats = 0;
		for (const auto& reg : registrations) {
			if (reg.registrationId == registrationId) {
				totalSeats = static_cast<int>(reg.seats.size());
				break;
			}
		}

		if (totalSeats == 0) {
			return true;
		}

		int votesCount = 0;
		for (const auto& vote : votes) {
			if (vote.registrationId == registrationId) {
				votesCount++;
			}
		}

		return votesCount >= totalSeats;
	}

	void VoteSystem::displayRegistrations(const std::vector<Registration>& registrations) {
		output.println("Your eligible registrations:");
		output.println("============================");

		for (size_t i = 0; i < registrations.size(); i++) {
			const auto& reg = registrations[i];
			Event event = getEventById(reg.eventId);

			auto votes = getActiveVotesForUser();
			int votesCount = 0;
			for (const auto& vote : votes) {
				if (vote.registrationId == reg.registrationId) {
					votesCount++;
				}
			}

			output.println(std::to_string(i + 1) + ". " + event.eventName);
			output.println("   Registration ID: " + reg.registrationId);
			output.println("   Number of seats: " + std::to_string(reg.seats.size()));
			output.println("   Seats: ", OutputManager::Color::CYAN);

			for (const auto& seat : reg.seats) {
				output.print("     " + seat.seatId + " (Row " + seat.row + ", Col " + seat.column + ")",
					OutputManager::Color::CYAN);
				std::cout << std::endl;
			}

			if (hasAlreadyVoted(reg.registrationId)) {
				output.println("   Status: All Votes Cast (" + std::to_string(votesCount) + "/" +
					std::to_string(reg.seats.size()) + ")", OutputManager::Color::YELLOW);
			}
			else {
				output.println("   Status: Votes Available (" + std::to_string(votesCount) + "/" +
					std::to_string(reg.seats.size()) + ")", OutputManager::Color::GREEN);
			}
			output.println("----------------------------");
		}
	}

	void VoteSystem::displaySongs(const std::vector<SongInfo>& songs) {
		output.println("\nAvailable songs to vote for:");
		output.println("=============================");

		for (size_t i = 0; i < songs.size(); i++) {
			output.println(std::to_string(i + 1) + ". " + songs[i].title + " by " + songs[i].performer);
		}
		output.println("=============================");
	}

	std::vector<VoteResult> VoteSystem::calculateVoteResults() {
		auto votes = getActiveVotesForUser();
		auto songs = loadSongsFromPerformances();

		std::map<std::string, VoteResult> resultMap;

		for (const auto& song : songs) {
			VoteResult result;
			result.songId = song.songId;
			result.title = song.title;
			result.performer = song.performer;
			result.totalVotes = 0;
			result.totalVoteWeight = 0;
			result.percentage = 0.0;
			resultMap[song.songId] = result;
		}

		for (const auto& vote : votes) {
			if (resultMap.find(vote.songId) != resultMap.end()) {
				resultMap[vote.songId].totalVotes++;
				resultMap[vote.songId].totalVoteWeight += vote.voteWeight;
			}
		}

		int totalVoteWeight = 0;
		for (const auto& pair : resultMap) {
			totalVoteWeight += pair.second.totalVoteWeight;
		}

		std::vector<VoteResult> results;
		for (auto& pair : resultMap) {
			if (totalVoteWeight > 0) {
				pair.second.percentage = (static_cast<double>(pair.second.totalVoteWeight) / static_cast<double>(totalVoteWeight)) * 100.0;
			}
			results.push_back(pair.second);
		}

		return results;
	}

	void VoteSystem::displayVoteResults(const std::vector<VoteResult>& results) {
		output.println("Current Vote Rankings:");
		output.println("=====================");

		std::vector<std::string> headers = { "Rank", "Song Title", "Performer", "Votes", "Weight", "%" };
		std::vector<std::vector<std::string>> rows;

		for (size_t i = 0; i < results.size(); i++) {
			const auto& result = results[i];
			std::vector<std::string> row = {
				std::to_string(i + 1),
				result.title,
				result.performer,
				std::to_string(result.totalVotes),
				std::to_string(result.totalVoteWeight),
				std::to_string(static_cast<int>(result.percentage)) + "%"
			};
			rows.push_back(row);
		}

		output.displayTable(headers, rows);

		if (!results.empty() && results[0].totalVoteWeight > 0) {
			output.println("========================================");
			output.println("Current Leader: " + results[0].title + " by " + results[0].performer,
				OutputManager::Color::GREEN);
			output.println("Total Votes: " + std::to_string(results[0].totalVotes));
			output.println("Vote Weight: " + std::to_string(results[0].totalVoteWeight));
			output.println("========================================");
		}
	}

	void VoteSystem::displayMyVotingHistory() {
		auto votes = getActiveVotesForUser();
		auto songs = loadSongsFromPerformances();

		std::vector<Vote> myVotes;
		for (const auto& vote : votes) {
			auto registrations = dm.getAllActiveRegistrations();
			for (const auto& reg : registrations) {
				if (reg.registrationId == vote.registrationId && reg.customerId == userId) {
					myVotes.push_back(vote);
					break;
				}
			}
		}

		if (myVotes.empty()) {
			output.println("You haven't cast any votes yet.", OutputManager::Color::YELLOW);
			output.println("Go to 'Quick Voting' or 'Cast Your Votes' to participate!");
			return;
		}

		output.println("Your Voting History:");
		output.println("===================");

		std::map<std::string, Vote> combinedVotes;

		for (const auto& vote : myVotes) {
			std::string key = vote.songId + "|" + vote.registrationId;

			if (combinedVotes.find(key) == combinedVotes.end()) {
				combinedVotes[key] = vote;
			}
			else {
				combinedVotes[key].voteWeight += vote.voteWeight;
			}
		}

		std::vector<std::string> headers = { "#", "Song Title", "Performer", "Weight", "Registration" };
		std::vector<std::vector<std::string>> rows;

		int rowNumber = 1;
		for (const auto& pair : combinedVotes) {
			const auto& vote = pair.second;

			std::string songTitle = "Unknown";
			std::string performer = "Unknown";

			for (const auto& song : songs) {
				if (song.songId == vote.songId) {
					songTitle = song.title;
					performer = song.performer;
					break;
				}
			}

			std::vector<std::string> row = {
				std::to_string(rowNumber),
				songTitle,
				performer,
				std::to_string(vote.voteWeight),
				vote.registrationId
			};
			rows.push_back(row);
			rowNumber++;
		}

		output.displayTable(headers, rows);

		int totalWeight = 0;
		std::set<std::string> uniqueSongs;
		for (const auto& vote : myVotes) {
			totalWeight += vote.voteWeight;
			uniqueSongs.insert(vote.songId);
		}

		output.println("YOUR VOTING SUMMARY");
		output.println("===================");
		output.println("Total Votes Cast: " + std::to_string(myVotes.size()));
		output.println("Total Vote Weight: " + std::to_string(totalWeight));
		output.println("Unique Songs Voted: " + std::to_string(uniqueSongs.size()));
	}

	Event VoteSystem::getEventById(const std::string& eventId) {
		auto events = dm.getAllActiveEvents();
		for (const auto& event : events) {
			if (event.eventId == eventId) {
				return event;
			}
		}
		return Event();
	}

	std::string VoteSystem::generateVoteId() {
		return dm.generateNewVoteId();
	}

}
