#pragma once
#include "Model.h"
#include "DataManager.h"
#include "OutputManager.h"
#include "Validation.h"
#include "TimeUtils.h"
#include <vector>
#include <string>
#include <ctime>

namespace EventBooking {
    class EventBooking {
    public:
        void run(User& user, time_t& currentDateTime);
    private:
        std::vector<Event> events;
        int ticketBuy = 0;
        std::vector<std::string> selectedSeats;
        User user;
        Event event;
        DataManager dm;
        OutputManager output;
        Validation validation;
        Registration registration;
        time_t currentDateTime;

        void showMenu();
        void displayMenu();
        void loadDataIntoVector(std::vector<std::vector<std::string>>& eventAttribute);
        void buyingTicket(int choice);
        bool getValidateInput(const std::string& prompt, std::string& result);
        void printSeatMenu(const std::vector<std::string>& currentSelection);
        void selectSeats();
        Registration completeRegistration(RegistrationStatus status = RegistrationStatus::COMPLETED);
        void showPaymentOptions();
        void proceedToPayment(Registration& registrationToPay);
        bool isSeatAvailable(const std::string& row, const std::string& column);
    };
}
