#pragma once
#include "Model.h"
#include "DataManager.h"
#include "OutputManager.h"
#include "Validation.h"
#include "EventManagement.h"
#include "TimeUtils.h"
#include <string>
#include <vector>

namespace PaymentCheckout {

    class PaymentCheckout {
    public:
        PaymentCheckout(User& user, DataManager& dm, time_t& currentDateTime);
        void run();
        void processPaymentForRegistration(Registration& registration, const Event& event);
        void displayInvoice(const Registration& registration, const Event& event);

    private:
        User user;
        DataManager& dm;
        OutputManager output;
        Validation validation;
        time_t currentDateTime;

        void showMainMenu();
        void handleEventRegistrationPayment();
        void processRefund();
        void viewPaymentHistory();

        std::vector<Registration> getUserPendingRegistrations();
        std::vector<Payment> getUserCompletedPayments();
        std::vector<Payment> getUserAllPayments();
        PaymentMethod selectPaymentMethod();
        bool simulatePaymentProcess(double amount, PaymentMethod method);
        bool isValidInteger(const std::string& str);

        std::string generatePaymentId();
        std::string getCurrentDateTime();
        bool validateRefundReason(const std::string& reason);

        void releaseSeatsOnRefund(const std::string& registrationId);
        void ensureSeatsReservedForRegistration(const std::string& registrationId);

        Event getEventById(const std::string& eventId);
    };

}
