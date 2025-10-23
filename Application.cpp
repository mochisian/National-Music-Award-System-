#include <iostream>
#include "Application.h"
#include "EventSystemMenu.h"
#include "CustomerDashboard.h"
#include "EventBooking.h"
#include "PaymentCheckout.h"
#include "TimeUtils.h"
#include "Vote.h"  

Application::Application() : currentDateTime(time(0)) {}

void Application::run() {
    bool running = true;

    while (running) {
        loginSystem.run(loggedInUser);

        if (loggedInUser.userId.empty()) {
            running = false;
            break;
        }

        switch (loggedInUser.userType) {
        case UserType::ADMIN:
            runAdminDashboard();
            break;
        case UserType::ORGANIZER:
            runOrganizerDashboard();
            break;
        case UserType::CUSTOMER:
            runCustomerDashboard();
            break;
        default:
            output.println("Unknown user type! Logging out.", OutputManager::Color::RED);
            break;
        }

        output.println("Logging out...", OutputManager::Color::GREEN);
        system("pause");
    }

    output.println("Thank you for using the Event Management System!", OutputManager::Color::GREEN);
    output.println("Goodbye!");
}

void Application::runAdminDashboard() {
    bool shouldLogout = false;
    while (!shouldLogout) {
        output.clearScreen();
        output.printHeader("Admin Dashboard");
        output.println("Welcome, " + loggedInUser.username + "!");
        output.println("----------------------------------------");
        output.println("1.  Manage User Accounts");
        output.println("2.  Access Event Systems");
        output.println("3.  Logout");
        output.println("----------------------------------------");
        output.print("Enter your choice (1-3): ");

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            loginSystem.manageUserAccounts();
        }
        else if (choice == "2") {
            EventSystemMenu eventMenu;
            eventMenu.run(loggedInUser, currentDateTime);
        }
        else if (choice == "3") {
            shouldLogout = true;
        }
        else {
            output.println("Invalid choice! Please select 1-3.", OutputManager::Color::RED);
            system("pause");
        }
    }
}

void Application::runOrganizerDashboard() {
    bool shouldLogout = false;
    dm.updateEventStatus(currentDateTime);
    while (!shouldLogout) {
        output.clearScreen();
        output.printHeader("Organizer Dashboard");
        output.println("Welcome, " + loggedInUser.username + "!");
        output.println("Current Date: " + TimeManagement::formatTimeTToString(currentDateTime, "%Y-%m-%d"));
        output.println("Current Time: " + TimeManagement::formatTimeTToString(currentDateTime, "%H:%M"));
        output.println("----------------------------------------");
        output.println("1.  Create Event");
        output.println("2.  Access Event Systems");
        output.println("3.  Set Current Date and Time");
        output.println("4.  Account Settings");
        output.println("5.  Logout");
        output.println("----------------------------------------");
        output.print("Enter your choice (1-5): ");

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            EventRegistration::EventRegistration eventCreation(loggedInUser, currentDateTime);
        }
        else if (choice == "2") {
            OrganizerDashboard::showAndManageOrganizerEvents(loggedInUser, currentDateTime, output, validation);
        }
        else if (choice == "3") {
            eventManagement.getUserDateTimeInput(currentDateTime);
        }
        else if (choice == "4") {
            loginSystem.userSettings();
        }
        else if (choice == "5") {
            shouldLogout = true;
        }
        else {
            output.println("Invalid choice! Please select 1-5.", OutputManager::Color::RED);
            system("pause");
        }
    }
}

void Application::runCustomerDashboard() {
    bool shouldLogout = false;
    dm.updateEventStatus(currentDateTime);
    while (!shouldLogout) {
        output.clearScreen();
        output.printHeader("Customer Dashboard");
        output.println("Welcome, " + loggedInUser.username + "!");
        output.println("Current Date: " + TimeManagement::formatTimeTToString(currentDateTime, "%Y-%m-%d"));
        output.println("Current Time: " + TimeManagement::formatTimeTToString(currentDateTime, "%H:%M"));
        output.println("----------------------------------------");
        output.println("1.  Buy Ticket");
        output.println("2.  Manage My Registered Events");
        output.println("3.  Payment System");
        output.println("4.  Vote System");
        output.println("5.  Set Current Date and Time");
        output.println("6.  Account Settings");
        output.println("7.  Logout");
        output.println("----------------------------------------");
        output.print("Enter your choice (1-7): ");

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            EventBooking::EventBooking eventBooking;
            eventBooking.run(loggedInUser, currentDateTime);
        }
        else if (choice == "2") {
            CustomerDashboard::showAndManageCustomerEvents(loggedInUser, currentDateTime, output, validation);
        }
        else if (choice == "3") {
            PaymentCheckout::PaymentCheckout paymentSystem(loggedInUser, dm, currentDateTime);
            paymentSystem.run();
        }
        else if (choice == "4") {
            Voting::VoteSystem voteSystem(loggedInUser.userId, dm);
            voteSystem.run(currentDateTime);

        }
        else if (choice == "5") {
            eventManagement.getUserDateTimeInput(currentDateTime);
        }
        else if (choice == "6") {
            loginSystem.userSettings();
        }
        else if (choice == "7") {
            shouldLogout = true;
        }
        else {
            output.println("Invalid choice! Please select 1-7.", OutputManager::Color::RED);
            system("pause");
        }
    }
}
