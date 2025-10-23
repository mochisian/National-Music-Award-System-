#include <iostream>
#include "CustomerDashboard.h"
#include "DataManager.h"
#include "EventSystemMenu.h"

namespace CustomerDashboard {

    void showScheduledAndOngoingEvent(OutputManager& output, const std::vector<std::vector<std::string>>& allScheduledEvent) {
        output.clearScreen();
        output.printHeader("My Registered Events");
        output.println("Scheduled & Ongoing Events");
        if (allScheduledEvent.empty() || allScheduledEvent[0].empty()) {
            output.println("\nYou have no registered events.", OutputManager::Color::YELLOW);
        }
        else {
            output.displayTable({ "No.", "Reg Id.","Event Name", "Date", "Time", "Number of ticket", "Seat Id", "Status" }, allScheduledEvent);
        }
        output.print("\nPlease enter the index of the event to manage (or 'c' to cancel): ");
    }

    void showAndManageCustomerEvents(User& loggedInUser, time_t& currentDateTime, OutputManager& output, Validation& validation) {
        DataManager dm;
        bool keepRunning = true;

        while (keepRunning) {
            std::vector<std::vector<std::string>> allScheduledEvent = dm.loadScheduledAndOngoingDataOfUser(loggedInUser.userId);

            showScheduledAndOngoingEvent(output, allScheduledEvent);

            std::string userInput;
            std::getline(std::cin, userInput);

            if (validation.shouldCancel(userInput)) {
                keepRunning = false;
                continue;
            }

            if (allScheduledEvent.empty() || allScheduledEvent[0][1].empty()) {
                output.println("No events to select.", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            if (validation.validateMenuInput(1, allScheduledEvent.size(), userInput)) {
                const auto& chosenEvent = allScheduledEvent.at(std::stoi(userInput) - 1);
                if (dm.getRegistrationFromRegistrationId(chosenEvent.at(1)).registrationStatus == RegistrationStatus::COMPLETED) {
                    EventSystemMenu eventMenu;
                    eventMenu.run(loggedInUser, currentDateTime, chosenEvent);
                }
                else {
                    output.println("Please complete your payment!", OutputManager::Color::RED);
                    system("pause");
                }

            }
            else {
                output.println("Invalid input!", OutputManager::Color::RED);
                system("pause");
            }
        }
    }
}