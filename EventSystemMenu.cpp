#include <iostream>
#include "EventSystemMenu.h"
#include "OutputManager.h"
#include "EventRegistration.h"
#include "EventSchedular.h"
#include "EventReporting.h"
#include "Attendance.h"
#include "Vote.h"
#include "PaymentCheckOut.h"
#include "Model.h"

void EventSystemMenu::run(User& user, time_t& currentDateTime, const std::vector<std::string>& chosenEvent) {
    std::string choice;
    do {
        showMenu(user, chosenEvent);
        choice = getChoice(user);

        if (user.userType == UserType::CUSTOMER) {
            handleCustomerChoice(choice, user, currentDateTime, chosenEvent);
        }
        else {
            handleOrganizerChoice(choice, user, chosenEvent, currentDateTime);
        }
    } while ((user.userType == UserType::CUSTOMER && choice != "5") ||
        (user.userType != UserType::CUSTOMER && choice != "5"));
}

void EventSystemMenu::showMenu(const User& user, const std::vector<std::string>& chosenEvent) {
    OutputManager output;
    output.clearScreen();

    std::string userTypeStr = (user.userType == UserType::ADMIN) ? "Admin" : "Organizer";
    if (user.userType == UserType::CUSTOMER) userTypeStr = "Customer";

    output.printHeader("Event System Menu - " + userTypeStr);
    std::cout << "Welcome, " << user.username << "!" << std::endl;

    if (user.userType == UserType::CUSTOMER) {
        if (!chosenEvent.empty()) {
            output.displayTable({ "No.", "Reg Id.", "Event Name", "Date", "Time", "Number of ticket", "Seat Id", "Status" }, { chosenEvent });
        }
        output.println("\nPlease choose an option:\n");
        output.println("1.  Take Attendance");
        output.println("2.  View Schedule");
        output.println("3.  Vote");
        output.println("4.  Cancel & Refund");
        output.println("5.  Back to Dashboard\n");
        output.print("Enter your choice (1-5): ");
    }
    else {
        if (!chosenEvent.empty()) {
            output.displayTable({ "No.", "Event Id.","Event Name", "Date", "Time", "Status" }, { chosenEvent });
        }
        output.println("\nPlease choose an option:\n");
        output.println("1.  Event Scheduler");
        output.println("2.  Event Monitoring");
        output.println("3.  Event Reporting");
        output.println("4.  Cancel the event");
        output.println("5.  Back to Dashboard\n");
        output.print("Enter your choice (1-4):");
    }
    std::cout.flush();
}

std::string EventSystemMenu::getChoice(const User& user) {
    OutputManager output;
    std::string choice;
    bool isValidChoice = false;

    const std::string validRange = (user.userType == UserType::CUSTOMER) ? "5" : "5";
    const std::string prompt = "Please enter 1 - " + validRange + ": ";

    do {
        std::getline(std::cin, choice);
        if (!choice.empty() && choice.length() == 1 && choice >= "1" && choice <= validRange) {
            isValidChoice = true;
        }
        else {
            output.print("Invalid input! ", OutputManager::Color::RED);
            output.print(prompt);
        }
    } while (!isValidChoice);

    return choice;
}

void EventSystemMenu::handleCustomerChoice(const std::string& choice, User& user, time_t& currentDateTime, const std::vector<std::string>& chosenEvent) {
    OutputManager output;
    DataManager dm;

    if (choice == "1") {
        if (chosenEvent.at(7) == "Ongoing" && dm.getRegistrationFromRegistrationId(chosenEvent.at(1)).registrationStatus == RegistrationStatus::COMPLETED) {
            AttendanceManager::AttendanceManager att;
            att.run(user, currentDateTime, chosenEvent[1]);
        }
        else {
            output.println("The event is not ongoing or no event selected!", OutputManager::Color::RED);
        }
        system("pause");
    }
    else if (choice == "2") {
        if (!chosenEvent.empty() && chosenEvent.size() > 1) {
            std::string registrationId = chosenEvent[1];
            std::vector<Registration> registrations = dm.getAllActiveRegistrations();
            std::string eventId = "";

            for (const auto& reg : registrations) {
                if (reg.registrationId == registrationId) {
                    eventId = reg.eventId;
                    break;
                }
            }

            if (!eventId.empty()) {
                EventSchedular eventSchedular(user);
                eventSchedular.runForCustomer(eventId);
            }
            else {
                output.println("Unable to find event information for this registration.", OutputManager::Color::RED);
                system("pause");
            }
        }
        else {
            output.println("No event selected! Please select an event first.", OutputManager::Color::RED);
            system("pause");
        }
    }
    else if (choice == "3") {
        {
            std::string registrationId = chosenEvent[1];
            Registration registration = dm.getRegistrationFromRegistrationId(registrationId);
            Event event = dm.getEventFromEventId(registration.eventId);

            Voting::VoteSystem voteSystem(user.userId, dm);
            voteSystem.runForSpecificEvent(event.eventId, registrationId, currentDateTime); 
        }
    }
    else if (choice == "4") {
        PaymentCheckout::PaymentCheckout paymentSystem(user, dm, currentDateTime);
        paymentSystem.run();
        system("pause");
    }
}

void EventSystemMenu::handleOrganizerChoice(const std::string& choice, User& user, const std::vector<std::string>& chosenEvent, time_t& currentDateTime) {
    OutputManager output;
    DataManager dm;

    if (choice == "1") {
        EventSchedular eventSchedular;
        eventSchedular.run();
    }
    else if (choice == "2") {
        if (user.userType == UserType::ADMIN) {
            EventMonitoring::EventMonitoring eventMonitoring;
            eventMonitoring.adminView(user);
        }
        else {
            if (user.userType == UserType::ORGANIZER) {
                EventMonitoring::EventMonitoring eventMonitoring;
                eventMonitoring.run(chosenEvent[1], chosenEvent);
            }
        }
        system("pause");
    }
    else if (choice == "3") {
        EventReporting eventReporting;
        eventReporting.run();
    }
    else if (choice == "4") {
        EventCancellation::cancelEvent(chosenEvent[1], chosenEvent, currentDateTime);
        system("pause");
    }
}
