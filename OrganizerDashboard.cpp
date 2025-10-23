#include "OrganizerDashboard.h"

namespace OrganizerDashboard {

	void showScheduledAndOngoingEvent(OutputManager& output, const std::vector<std::vector<std::string>>& allScheduledEvent) {
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

    void showAndManageOrganizerEvents(User& loggedInUser, time_t& currentDateTime, OutputManager& output, Validation& validation) {
        DataManager dm;
        bool continueManagingEvents = true;

        while (continueManagingEvents) {
            std::vector<std::vector<std::string>> allScheduledEvent = dm.loadScheduledAndOngoingDataOfOrganizer(loggedInUser.userId);

            showScheduledAndOngoingEvent(output, allScheduledEvent);

            std::string userInput;
            std::getline(std::cin, userInput);

            if (validation.shouldCancel(userInput)) {
                continueManagingEvents = false;
                return;
            }

            if (allScheduledEvent.empty() || (allScheduledEvent[0].size() <= 1 || allScheduledEvent[0][1].empty())) {
                output.println("No events to select. ", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            if (validation.validateMenuInput(1, allScheduledEvent.size(), userInput)) {
                const auto& chosenEvent = allScheduledEvent.at(std::stoi(userInput) - 1);
                EventSystemMenu eventMenu;
                eventMenu.run(loggedInUser, currentDateTime, chosenEvent);
            }
            else {
                output.println("Invalid input!", OutputManager::Color::RED);
                system("pause");
            }
        }
    }
}