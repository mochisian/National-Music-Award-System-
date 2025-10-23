#include "LoginModule.h"
#include "DataManager.h"
#include <iostream>
#include <iomanip>
#include <regex>
#include <algorithm>
#include <cctype>

namespace LoginModule {
    LoginSystem::LoginSystem() : organizerCounter(0), customerCounter(0) {
        loadUsers();
        updateCounters();
        createAdmin();
    }

    void LoginSystem::run(User& loggedInUser) {
        bool running = true;

        while (running) {
            showMainMenu();

            std::string inputLine;
            output.print("Enter your choice (1-4): ", OutputManager::Color::WHITE);
            std::getline(std::cin, inputLine);

            if (!validation.validateMenuInput(1,4,inputLine)) {
                output.println("Invalid input! Please enter a number.", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            bool shouldExit = false;

            switch (std::stoi(inputLine)) {
            case 1:
                shouldExit = handleAdminLogin(loggedInUser);
                break;
            case 2:
                shouldExit = handleOrganizerLogin(loggedInUser);
                break;
            case 3:
                shouldExit = handleCustomerLogin(loggedInUser);
                break;
            case 4:
                output.println("Thank you for using our system...", OutputManager::Color::GREEN);
                loggedInUser = User();
                running = false;
                break;
            default:
                output.println("Invalid choice! Please select 1-4.", OutputManager::Color::RED);
                system("pause");
                break;
            }

            if (shouldExit) {
                currentUser = loggedInUser;
                running = false;
            }
        }
    }

    void LoginSystem::loadUsers() {
        DataManager dm;
        users = dm.getAllActiveUsers();

        std::cout << "Loaded " << users.size() << " users from JSON" << std::endl;
        for (const auto& user : users) {
            std::cout << "User: " << user.username << " (" << user.userId << ")" << std::endl;
        }
    }

    void LoginSystem::saveUsers() {
        DataManager dm;
        dm.saveData<User>(users, "users.json");
    }

    void LoginSystem::updateCounters() {
        int maxOrgCounter = 0;
        int maxCusCounter = 0;

        for (const auto& user : users) {
            if (user.userType == UserType::ORGANIZER) {
                try {
                    std::string idNum = user.userId.substr(3);
                    int num = std::stoi(idNum);
                    maxOrgCounter = std::max(maxOrgCounter, num);
                }
                catch (const std::exception&) {

                }
            }
            else if (user.userType == UserType::CUSTOMER) {
                try {
                    std::string idNum = user.userId.substr(3);
                    int num = std::stoi(idNum);
                    maxCusCounter = std::max(maxCusCounter, num);
                }
                catch (const std::exception&) {

                }
            }
        }

        organizerCounter = maxOrgCounter;
        customerCounter = maxCusCounter;
    }

    void LoginSystem::createAdmin() {
        for (const auto& user : users) {
            if (user.userType == UserType::ADMIN) {
                return;
            }
        }

        User admin;
        admin.userId = "ADM001";
        admin.username = "Admin_123";
        admin.passwordHash = "Admin_123";
        admin.email = "admin01@gmail.com";
        admin.phoneNumber = "";
        admin.userType = UserType::ADMIN;

        users.push_back(admin);
        saveUsers();
        output.println("Default admin created successfully!\n");
    }

    std::string LoginSystem::generateUserId(UserType userType) {
        if (userType == UserType::ORGANIZER) {
            organizerCounter++;
            std::string numStr = std::to_string(organizerCounter);
            return "ORG" + std::string(3 - std::min(3, (int)numStr.length()), '0') + numStr;
        }
        else if (userType == UserType::CUSTOMER) {
            customerCounter++;
            std::string numStr = std::to_string(customerCounter);
            return "CUS" + std::string(3 - std::min(3, (int)numStr.length()), '0') + numStr;
        }
        return "";
    }

    bool LoginSystem::isUniqueUsername(const std::string& username) {
        for (const auto& user : users) {
            if (user.username == username && !user.isDeleted)
                return false;
        }
        return true;
    }

    bool LoginSystem::isUniqueEmail(const std::string& email) {
        for (const auto& user : users) {
            if (user.email == email && !user.isDeleted)
                return false;
        }
        return true;
    }

    bool LoginSystem::isUniquePhoneNo(const std::string& phoneNumber) {
        for (const auto& user : users) {
            if (user.phoneNumber == phoneNumber && !user.isDeleted)
                return false;
        }
        return true;
    }

    bool LoginSystem::isUniqueUsernameExcludeCurrent(const std::string& username) {
        for (const auto& user : users) {
            if (user.username == username && user.userId != currentUser.userId && !user.isDeleted)
                return false;
        }
        return true;
    }

    bool LoginSystem::isUniqueEmailExcludeCurrent(const std::string& email) {
        for (const auto& user : users) {
            if (user.email == email && user.userId != currentUser.userId && !user.isDeleted)
                return false;
        }
        return true;
    }

    bool LoginSystem::isUniquePhoneNoExcludeCurrent(const std::string& phoneNumber) {
        for (const auto& user : users) {
            if (user.phoneNumber == phoneNumber && user.userId != currentUser.userId && !user.isDeleted)
                return false;
        }
        return true;
    }

    std::string LoginSystem::getStringInput(const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    void LoginSystem::showMainMenu() {
        output.clearScreen();
        output.showMainMenu();
        output.println("Welcome to National Music Award System");
        output.println("Please select your role:\n");
        output.println("1.\tAdmin");
        output.println("2.\tOrganizer");
        output.println("3.\tCustomer");
        output.println("4.\tExit\n");
    }

#include <string>
#include <limits>
#include <sstream>

    bool LoginSystem::handleAdminLogin(User& loggedInUser) {
        bool running = true;

        while (running) {
            output.clearScreen();
            output.printHeader("Admin Login");
            output.println("1.  Sign In");
            output.println("2.  Back to Main Menu");
            output.print("Enter your choice (1-2): ", OutputManager::Color::WHITE);

            std::string inputLine;
            std::getline(std::cin, inputLine);

            if (!validation.validateMenuInput(1,2,inputLine)) {
                output.println("Invalid input! Please enter a number.", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            switch (std::stoi(inputLine)) {
            case 1:
                if (signIn(UserType::ADMIN, loggedInUser)) {
                    output.println("Login successful! Welcome, " + loggedInUser.username + "!", OutputManager::Color::GREEN);
                    system("pause");
                    return true;
                }
                break;
            case 2:
                running = false;
                break;
            default:
                output.println("Invalid option! Please try again.", OutputManager::Color::RED);
                system("pause");
                break;
            }
        }

        return false;
    }

    bool LoginSystem::handleOrganizerLogin(User& loggedInUser) {
        return handleUserTypeLogin(UserType::ORGANIZER, "Organizer", loggedInUser);
    }

    bool LoginSystem::handleCustomerLogin(User& loggedInUser) {
        return handleUserTypeLogin(UserType::CUSTOMER, "Customer", loggedInUser);
    }

    bool LoginSystem::handleUserTypeLogin(UserType userType, const std::string& typeName, User& loggedInUser) {
        bool running = true;

        while (running) {
            output.clearScreen();
            output.printHeader(typeName + " Login");
            output.println("1. Sign Up");
            output.println("2. Sign In");
            output.println("3. Back to Main Menu");
            output.print("Enter your choice (1-3): ");

            std::string inputLine;
            std::getline(std::cin, inputLine);

            if (!validation.validateMenuInput(1, 3, inputLine)) {
                output.println("Invalid input! Please enter a number between 1-3.", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            switch (std::stoi(inputLine)) {
            case 1:
                if (signUp(userType)) {
                    output.println("Registration successful! Please sign in to continue.", OutputManager::Color::GREEN);
                    system("pause");
                }
                break;
            case 2:
                if (signIn(userType, loggedInUser)) {
                    output.println("Login successful! Welcome, " + loggedInUser.username + "!", OutputManager::Color::GREEN);
                    system("pause");
                    return true;
                }
                break;
            case 3:
                running = false;
                break;
            default:
                output.println("An unexpected error occurred with menu choice.", OutputManager::Color::RED);
                system("pause");
                break;
            }
        }

        return false;
    }

    bool LoginSystem::signUp(UserType userType) {
        User newUser;
        newUser.userType = userType;
        newUser.userId = generateUserId(userType);

        bool running = true;

        while (running) {
            output.clearScreen();
            output.printHeader("Registration - " + getUserTypeString(userType));
            newUser.username = getStringInput("Enter username (5-12 chars, can include symbols/digits/letters): ");

            if (validation.shouldCancel(newUser.username)) {
                return false;
            }

            if (!validation.isValidUsername(newUser.username)) {
                output.println("Invalid format! Minimum 5, maximum 12 characters, can include symbols/digits/letters.", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return false;
                }
                continue;
            }

            if (!isUniqueUsername(newUser.username)) {
                output.println("Username already exists.", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return false;
                }
                continue;
            }

            running = false;
        }

        running = true;

        while (running) {
            newUser.passwordHash = getStringInput("Enter password (Min 8 chars, include uppercase/lowercase/digit): ");

            if (validation.shouldCancel(newUser.passwordHash)) {
                return false;
            }

            if (!validation.isValidPassword(newUser.passwordHash)) {
                output.println("Invalid password! Minimum 8 characters, must include uppercase, lowercase, and digit.", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return false;
                }
                continue;
            }
            running = false;
        }

        running = true;

        while (running) {
            newUser.email = getStringInput("Enter email: ");

            if (validation.shouldCancel(newUser.email)) {
                return false;
            }

            if (!validation.isValidEmail(newUser.email)) {
                output.println("Invalid email format.", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return false;
                }
                continue;
            }

            if (!isUniqueEmail(newUser.email)) {
                output.println("Email already registered!", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return false;
                }
                continue;
            }
            running = false;
        }

        running = true;

        while (running) {
            output.print("Enter phone number (+6012-1234567): +60");
            std::string phoneInput;
            std::getline(std::cin, phoneInput);

            if (validation.shouldCancel(phoneInput)) {
                return false;
            }

            newUser.phoneNumber = phoneInput;

            if (!validation.isValidPhoneNo(newUser.phoneNumber)) {
                output.println("Invalid phone number format. Expected format: 1X-XXXXXXX", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return false;
                }
                continue;
            }

            if (!isUniquePhoneNo(newUser.phoneNumber)) {
                output.println("Phone number already registered.", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return false;
                }
                continue;
            }
            running = false;
        }

        users.push_back(newUser);
        saveUsers();

        output.println("Account created successfully! Hi, " + newUser.username + "! Your UID: " + newUser.userId, OutputManager::Color::GREEN);
        system("pause");

        return true;
    }

    bool LoginSystem::signIn(UserType userType, User& loggedInUser) {
        loadUsers();

        int attemptCount = 0;
        const int maxAttempts = 3;

        while (attemptCount < maxAttempts) {
            output.clearScreen();
            output.printHeader("Sign In - " + getUserTypeString(userType));

            std::string username = getStringInput("Enter username (or 'c' to cancel): ");

            if (validation.shouldCancel(username)) {
                return false;
            }

            std::string password = getStringInput("Enter password (or 'c' to cancel): ");

            if (validation.shouldCancel(password)) {
                return false;
            }

            for (const auto& user : users) {
                if (user.username == username && user.passwordHash == password && user.userType == userType && user.isDeleted != true) {
                    loggedInUser = user;
                    return true;
                }
            }

            attemptCount++;
            output.println("Invalid credentials. Attempt " + std::to_string(attemptCount) + " of " + std::to_string(maxAttempts), OutputManager::Color::RED);

            if (attemptCount < maxAttempts) {
                output.println("Press any key to try again or 'c' to cancel...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return false;
                }
            }
        }

        output.println("Too many failed attempts. Returning to menu...", OutputManager::Color::RED);
        system("pause");
        return false;
    }

    void LoginSystem::manageUserAccounts() {
        bool running = true;

        while (running) {
            showManageUserAccountsMenu();
            std::string inputLine;
            std::getline(std::cin, inputLine); 


            if (!validation.validateMenuInput(1, 5, inputLine)) {
                output.println("Invalid input! Please enter a number between 1-5.", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            switch (std::stoi(inputLine)) {
            case 1:
                viewAllOrganizers();
                break;
            case 2:
                viewAllCustomers();
                break;
            case 3:
                deleteOrganizerAccount();
                break;
            case 4:
                deleteCustomerAccount();
                break;
            case 5:
                running = false;
                break;
            default:
                output.println("An unexpected error occurred with menu choice.", OutputManager::Color::RED);
                system("pause");
                break;
            }
        }
    }

    void LoginSystem::showManageUserAccountsMenu() {
        output.clearScreen();
        output.printHeader("Manage User Accounts");
        output.println("1. View All Organizers");
        output.println("2. View All Customers");
        output.println("3. Delete Organizer Account");
        output.println("4. Delete Customer Account");
        output.println("5. Back to Admin Menu");
        output.print("Enter your choice (1-5): ");
    }

    void LoginSystem::viewAllOrganizers() {
        output.clearScreen();
        output.printHeader("All Organizers");
        std::vector<std::string> header = { "User ID", "Username", "Email", "Phone Number" };
        std::vector<std::vector<std::string>> rows;

        for (const auto& user : users) {
            if (user.userType == UserType::ORGANIZER && !user.isDeleted) {
                std::vector<std::string> row = {
                    user.userId,
                    user.username,
                    user.email,
                    "+60" + user.phoneNumber
                };
                rows.push_back(row);
            }
        }

        if (rows.empty()) {
            output.println("No organizers found!", OutputManager::Color::RED);
        }
        else {
            output.displayTable(header, rows);
        }

        system("pause");
    }

    void LoginSystem::viewAllCustomers() {
        output.clearScreen();
        output.printHeader("All Customers");
        std::vector<std::string> header = { "User ID", "Username", "Email", "Phone Number" };
        std::vector<std::vector<std::string>> rows;

        for (const auto& user : users) {
            if (user.userType == UserType::CUSTOMER && !user.isDeleted) {
                std::vector<std::string> row = {
                    user.userId,
                    user.username,
                    user.email,
                    "+60" + user.phoneNumber
                };
                rows.push_back(row);
            }
        }

        if (rows.empty()) {
            output.println("No customers found!", OutputManager::Color::RED);
        }
        else {
            output.displayTable(header, rows);
        }

        system("pause");
    }

    void LoginSystem::deleteOrganizerAccount() {
        output.clearScreen();
        output.printHeader("Delete Organizer Account");

        std::string userId = getStringInput("Enter Organizer ID to delete (or 'c' to cancel): ");

        if (validation.shouldCancel(userId)) {
            return;
        }

        auto it = std::find_if(users.begin(), users.end(),
            [&userId](const User& user) {
                return user.userId == userId && user.userType == UserType::ORGANIZER;
            });

        if (it != users.end()) {
            output.println("Found organizer: " + it->username);
            output.print("Are you sure you want to delete this account? (y/n): ");
            char confirm;
            std::cin >> confirm;
            std::cin.ignore();

            if (confirm == 'y' || confirm == 'Y') {
                it->softDelete();
                saveUsers();
                users.clear();
                users = dm.getAllActiveUsers();
                output.println("Organizer account deleted successfully!", OutputManager::Color::GREEN);
            }
            else {
                output.println("Deletion cancelled.", OutputManager::Color::RED);
            }
        }
        else {
            output.println("Organizer not found!", OutputManager::Color::RED);
        }

        system("pause");
    }

    void LoginSystem::deleteCustomerAccount() {
        output.clearScreen();
        output.printHeader("Delete Customer Account");

        std::string userId = getStringInput("Enter Customer ID to delete (or 'c' to cancel): ");

        if (validation.shouldCancel(userId)) {
            return;
        }

        auto it = std::find_if(users.begin(), users.end(),
            [&userId](const User& user) {
                return user.userId == userId && user.userType == UserType::CUSTOMER;
            });

        if (it != users.end()) {
            output.println("Found customer: " + it->username);
            output.print("Are you sure you want to delete this account? (y/n): ");
            char confirm;
            std::cin >> confirm;
            std::cin.ignore();

            if (confirm == 'y' || confirm == 'Y') {
                it->softDelete();
                saveUsers();
                users.clear();
                users = dm.getAllActiveUsers();
                output.println("Customer account deleted successfully!", OutputManager::Color::GREEN);
            }
            else {
                output.println("Deletion cancelled.", OutputManager::Color::RED);
            }
        }
        else {
            output.println("Customer not found!", OutputManager::Color::RED);
        }

        system("pause");
    }

    void LoginSystem::userSettings() {
        bool running = true;

        while (running) {
            showUserSettingsMenu();

            std::string inputLine;
            std::getline(std::cin, inputLine);

            bool isValid = !inputLine.empty();
            for (char c : inputLine) {
                if (!std::isdigit(c)) {
                    isValid = false;
                    break;
                }
            }

            if (!isValid) {
                output.println("Invalid input! Please enter a whole number only (1-6).", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            int choice;
            try {
                choice = std::stoi(inputLine);
            }
            catch (...) {
                output.println("Invalid input! Please enter a whole number only (1-6).", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            switch (choice) {
            case 1:
                viewMyAccount();
                break;
            case 2:
                changeUsername();
                break;
            case 3:
                changePassword();
                break;
            case 4:
                changeEmail();
                break;
            case 5:
                changePhoneNumber();
                break;
            case 6:
                running = false;
                break;
            default:
                output.println("Invalid choice! Please select 1-6.", OutputManager::Color::RED);
                system("pause");
                break;
            }
        }
    }

    void LoginSystem::showUserSettingsMenu() {
        output.clearScreen();
        output.printHeader("Account Settings");
        output.println("1. View My Account");
        output.println("2. Change Username");
        output.println("3. Change Password");
        output.println("4. Change Email");
        output.println("5. Change Phone Number");
        output.println("6. Back to Main Menu");
        output.print("Enter your choice (1-6): ");
    }

    void LoginSystem::changeUsername() {
        output.clearScreen();
        output.printHeader("Change Username");

        bool running = true;

        while (running) {
            std::string newUsername = getStringInput("Enter new username (5-12 chars, can include symbols/digits/letters) or 'c' to cancel: ");

            if (validation.shouldCancel(newUsername)) {
                return;
            }

            if (!validation.isValidUsername(newUsername)) {
                output.println("Invalid format! Minimum 5, maximum 12 characters, can include symbols/digits/letters.", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return;
                }
                continue;
            }

            if (!isUniqueUsernameExcludeCurrent(newUsername)) {
                output.println("Username already exists!", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return;
                }
                continue;
            }

            for (auto& user : users) {
                if (user.userId == currentUser.userId) {
                    user.username = newUsername;
                    break;
                }
            }
            currentUser.username = newUsername;
            saveUsers();
            output.println("Username updated successfully!", OutputManager::Color::GREEN);
            
            running = false;
        }

        system("pause");
    }

    void LoginSystem::changePassword() {
        output.clearScreen();
        output.printHeader("Change Password");

        bool running = true;

        while (running) {
            std::string newPassword = getStringInput("Enter new password (Min 8 chars, include uppercase/lowercase/digit) or 'c' to cancel: ");

            if (validation.shouldCancel(newPassword)) {
                return;
            }

            if (!validation.isValidPassword(newPassword)) {
                output.println("Invalid password! Minimum 8 characters, must include uppercase, lowercase, and digit.", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return;
                }
                continue;
            }

            for (auto& user : users) {
                if (user.userId == currentUser.userId) {
                    user.passwordHash = newPassword;
                    break;
                }
            }
            currentUser.passwordHash = newPassword;
            saveUsers();
            output.println("Password updated successfully!", OutputManager::Color::GREEN);
            
            running = false;
        }

        system("pause");
    }

    void LoginSystem::changeEmail() {
        output.clearScreen();
        output.printHeader("Change Email");

        bool running = true;

        while (running) {
            std::string newEmail = getStringInput("Enter new email or 'c' to cancel: ");

            if (validation.shouldCancel(newEmail)) {
                return;
            }

            if (!validation.isValidEmail(newEmail)) {
                output.println("Invalid email format!", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return;
                }
                continue;
            }

            if (!isUniqueEmailExcludeCurrent(newEmail)) {
                output.println("Email already registered!", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return;
                }
                continue;
            }

            for (auto& user : users) {
                if (user.userId == currentUser.userId) {
                    user.email = newEmail;
                    break;
                }
            }
            currentUser.email = newEmail;
            saveUsers();
            output.println("Email updated successfully!", OutputManager::Color::GREEN);
            
            running = false;
        }

        system("pause");
    }

    void LoginSystem::changePhoneNumber() {
        output.clearScreen();
        output.printHeader("Change Phone Number");

        bool running = true;

        while (running) {
            output.print("Enter new phone number: +60");
            std::string phoneInput;
            std::getline(std::cin, phoneInput);

            if (validation.shouldCancel(phoneInput)) {
                return;
            }

            if (!validation.isValidPhoneNo(phoneInput)) {
                output.println("Invalid phone number format. Expected format: 1X-XXXXXXX", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return;
                }
                continue;
            }

            if (!isUniquePhoneNoExcludeCurrent(phoneInput)) {
                output.println("Phone number already registered!", OutputManager::Color::RED);
                output.println("Press 'c' to cancel or any key to try again...");
                std::string choice;
                std::getline(std::cin, choice);
                if (validation.shouldCancel(choice)) {
                    return;
                }
                continue;
            }

            for (auto& user : users) {
                if (user.userId == currentUser.userId) {
                    user.phoneNumber = phoneInput;
                    break;
                }
            }
            currentUser.phoneNumber = phoneInput;
            saveUsers();
            output.println("Phone number updated successfully!", OutputManager::Color::GREEN);
            
            running = false;
        }

        system("pause");
    }

    void LoginSystem::viewMyAccount() {
        output.clearScreen();
        output.printHeader("My Account Information");
        output.println("User ID: " + currentUser.userId);
        output.println("Username: " + currentUser.username);
        output.println("Email: " + currentUser.email);
        output.println("Phone: +60" + currentUser.phoneNumber);
        output.println("User Type: " + getUserTypeString(currentUser.userType));

        system("pause");
    }

    std::string LoginSystem::getUserTypeString(UserType userType) {
        switch (userType) {
        case UserType::ADMIN: return "Admin";
        case UserType::ORGANIZER: return "Organizer";
        case UserType::CUSTOMER: return "Customer";
        default: return "Unknown";
        }
    }
}
