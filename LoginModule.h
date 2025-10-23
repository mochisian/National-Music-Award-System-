#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Model.h"
#include "DataManager.h"
#include "OutputManager.h"
#include "Validation.h"

namespace LoginModule {
    class LoginSystem {
    public:
        LoginSystem();
        void run(User& user);

        void manageUserAccounts();
        void userSettings();

    private:
        OutputManager output;
        Validation validation;
        DataManager dm;
        std::vector<User> users;
        int organizerCounter;
        int customerCounter;
        User currentUser;
        std::string userFile = "users.json";

        // Core functionality
        void loadUsers();
        void saveUsers();
        void updateCounters();
        void createAdmin();

        // User ID generation
        std::string generateUserId(UserType userType);

        // Validation methods
        bool isUniqueUsername(const std::string& username);
        bool isUniqueEmail(const std::string& email);
        bool isUniquePhoneNo(const std::string& phoneNumber);

        // Helper functions for uniqueness check (excluding current user)
        bool isUniqueUsernameExcludeCurrent(const std::string& username);
        bool isUniqueEmailExcludeCurrent(const std::string& email);
        bool isUniquePhoneNoExcludeCurrent(const std::string& phoneNumber);

        // Input handling
        std::string getStringInput(const std::string& prompt);

        // Menu display
        void showMainMenu();

        // Login handlers
        bool handleAdminLogin(User& loggedInUser);
        bool handleOrganizerLogin(User& loggedInUser);
        bool handleCustomerLogin(User& loggedInUser);
        bool handleUserTypeLogin(UserType userType, const std::string& typeName, User& loggedInUser);

        // Authentication methods
        bool signUp(UserType userType);
        bool signIn(UserType userType, User& loggedInUser);

        // User Account Management (Admin)
        void showManageUserAccountsMenu();
        void viewAllOrganizers();
        void viewAllCustomers();
        void deleteOrganizerAccount();
        void deleteCustomerAccount();

        // Settings (Customer,Organizer)
        void showUserSettingsMenu();
        void changeUsername();
        void changePassword();
        void changeEmail();
        void changePhoneNumber();
        void viewMyAccount();

        // Utility methods
        std::string getUserTypeString(UserType userType);
    };
}