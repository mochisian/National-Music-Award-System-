#include "PaymentCheckout.h"
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cctype>

namespace PaymentCheckout {

    PaymentCheckout::PaymentCheckout(User& user, DataManager& dm, time_t& currentDateTime)
        : user(user), dm(dm) {
        this->currentDateTime = currentDateTime;
    }

    void PaymentCheckout::run() {
        showMainMenu();
    }

    bool PaymentCheckout::isValidInteger(const std::string& str) {
        if (str.empty()) return false;

        size_t start = 0;
        if (str[0] == '-') start = 1;

        for (size_t i = start; i < str.length(); i++) {
            if (!std::isdigit(str[i])) {
                return false;
            }
        }
        return true;
    }

    void PaymentCheckout::showMainMenu() {
        bool running = true;

        while (running) {
            system("cls");
            output.printHeader("Payment and Checkout System");

            output.println("1. Pay for Pending Registration");
            output.println("2. Request Refund");
            output.println("3. View Payment History");
            output.println("4. Back to Main Menu");

            output.print("Enter your choice (1-4): ");
            std::string choice;
            std::getline(std::cin, choice);

            if (validation.shouldCancel(choice)) {
                running = false;
                continue;
            }

            if (!isValidInteger(choice)) {
                output.println("Invalid input! Please enter a whole number only (no decimals).", OutputManager::Color::RED);
                system("pause");
                continue;
            }

            try {
                int choiceNum = std::stoi(choice);
                switch (choiceNum) {
                case 1:
                    handleEventRegistrationPayment();
                    break;
                case 2:
                    processRefund();
                    break;
                case 3:
                    viewPaymentHistory();
                    break;
                case 4:
                    running = false;
                    break;
                default:
                    output.println("Invalid choice! Please enter 1-4.", OutputManager::Color::RED);
                    system("pause");
                    break;
                }
            }
            catch (...) {
                output.println("Invalid input! Please enter a valid number.", OutputManager::Color::RED);
                system("pause");
            }
        }
    }

    std::vector<Registration> PaymentCheckout::getUserPendingRegistrations() {
        auto registrations = dm.loadData<Registration>("registrations.json");
        std::vector<Registration> pendingRegistrations;

        for (const auto& reg : registrations) {
            if (reg.customerId == user.userId &&
                reg.registrationStatus == RegistrationStatus::PENDING) {

                auto payments = dm.loadData<Payment>("payments.json");
                bool hasCompletedPayment = false;

                for (const auto& payment : payments) {
                    if (payment.registrationId == reg.registrationId &&
                        payment.paymentStatus == PaymentStatus::COMPLETED) {
                        hasCompletedPayment = true;
                        break;
                    }
                }

                if (!hasCompletedPayment) {
                    pendingRegistrations.push_back(reg);
                }
            }
        }

        return pendingRegistrations;
    }

    void PaymentCheckout::handleEventRegistrationPayment() {
        auto pendingRegistrations = getUserPendingRegistrations();
        if (pendingRegistrations.empty()) {
            output.println("No pending payments found.", OutputManager::Color::YELLOW);
            system("pause");
            return;
        }

        output.printHeader("Pending Payments");

        std::vector<std::string> header = { "No.", "Event Name", "Seats", "Price per Seat", "Total Amount" };
        std::vector<std::vector<std::string>> rows;

        auto events = dm.loadData<Event>("events.json");

        for (size_t i = 0; i < pendingRegistrations.size(); i++) {
            const auto& reg = pendingRegistrations[i];
            const Event* event = nullptr;

            for (const auto& ev : events) {
                if (ev.eventId == reg.eventId) {
                    event = &ev;
                    break;
                }
            }

            if (event) {
                double totalAmount = event->ticketPrice * reg.seats.size();
                std::vector<std::string> row = {
                    std::to_string(i + 1),
                    event->eventName,
                    std::to_string(reg.seats.size()),
                    "RM " + std::to_string(event->ticketPrice),
                    "RM " + std::to_string(totalAmount)
                };
                rows.push_back(row);
            }
        }

        if (!rows.empty()) {
            output.displayTable(header, rows);
        }

        output.print("Select registration to pay (1-" + std::to_string(pendingRegistrations.size()) + ") or 'c' to cancel: ");
        std::string choiceStr;
        std::getline(std::cin, choiceStr);

        if (validation.shouldCancel(choiceStr)) return;

        if (!isValidInteger(choiceStr)) {
            output.println("Invalid input! Please enter a whole number only (no decimals).", OutputManager::Color::RED);
            system("pause");
            return;
        }

        try {
            int choice = std::stoi(choiceStr) - 1;
            if (choice < 0 || choice >= pendingRegistrations.size()) {
                output.println("Invalid selection!", OutputManager::Color::RED);
                system("pause");
                return;
            }

            Registration selectedReg = pendingRegistrations[choice];
            Event event = getEventById(selectedReg.eventId);

            if (event.eventId.empty()) {
                output.println("Event not found!", OutputManager::Color::RED);
                return;
            }

            displayInvoice(selectedReg, event);

            output.print("Proceed with payment? (y/n): ");
            std::string confirm;
            std::getline(std::cin, confirm);

            if (confirm == "y" || confirm == "Y") {
                processPaymentForRegistration(selectedReg, event);
            }
        }
        catch (...) {
            output.println("Invalid input! Please enter a valid number.", OutputManager::Color::RED);
            system("pause");
        }
    }

    void PaymentCheckout::displayInvoice(const Registration& registration, const Event& event) {
        double totalAmount = event.ticketPrice * registration.seats.size();

        output.printHeader("INVOICE");
        output.println("Invoice Date: " + getCurrentDateTime());
        output.println("----------------------------------------");
        output.println("Registration ID: " + registration.registrationId);
        output.println("Event: " + event.eventName);
        output.println("Customer: " + user.username);
        output.println("----------------------------------------");
        output.println("Seat Details:");
        output.println("Number of Seats: " + std::to_string(registration.seats.size()));

        for (const auto& seat : registration.seats) {
            output.println("  - Seat " + seat.seatId + " (Row " + seat.row + ", Column " + seat.column + ")");
        }

        output.println("----------------------------------------");
        output.println("Ticket Price per seat: RM " + std::to_string(event.ticketPrice));
        output.println("Total Amount: RM " + std::to_string(totalAmount));
        output.println("========================================");
    }

    PaymentMethod PaymentCheckout::selectPaymentMethod() {
        bool running = true;

        while (running) {
            output.println("\nSelect Payment Method:");
            output.println("1. Online Banking");
            output.println("2. E-Wallet");
            output.println("3. Card Payment");
            output.println("4. Cancel Payment");

            output.print("Enter choice (1-4): ");
            std::string choice;
            std::getline(std::cin, choice);

            if (validation.shouldCancel(choice)) {
                return PaymentMethod::ONLINE_BANKING;
            }

            if (!isValidInteger(choice)) {
                output.println("Invalid input! Please enter a whole number only (no decimals).", OutputManager::Color::RED);
                continue;
            }

            try {
                int choiceNum = std::stoi(choice);
                switch (choiceNum) {
                case 1: return PaymentMethod::ONLINE_BANKING;
                case 2: return PaymentMethod::E_WALLET;
                case 3: return PaymentMethod::CARD_PAYMENT;
                case 4:
                    output.println("Payment cancelled.", OutputManager::Color::YELLOW);
                    throw std::runtime_error("Payment cancelled");
                default:
                    output.println("Invalid choice! Please select 1-4.", OutputManager::Color::RED);
                    break;
                }
            }
            catch (const std::runtime_error& e) {
                throw e;
            }
            catch (...) {
                output.println("Invalid input! Please enter a valid number.", OutputManager::Color::RED);
            }
        }

        return PaymentMethod::ONLINE_BANKING;
    }

    bool PaymentCheckout::simulatePaymentProcess(double amount, PaymentMethod method) {
        output.printHeader("Payment Processing");
        output.println("Amount: RM " + std::to_string(amount));

        switch (method) {
        case PaymentMethod::ONLINE_BANKING:
            output.println("Method: Online Banking");
            break;
        case PaymentMethod::E_WALLET:
            output.println("Method: E-Wallet");
            break;
        case PaymentMethod::CARD_PAYMENT:
            output.println("Method: Card Payment");
            break;
        }

        output.print("Enter 'confirm' to simulate successful payment or 'cancel' to abort: ");
        std::string confirm;
        std::getline(std::cin, confirm);

        if (confirm == "confirm") {
            return true;
        }
        else {
            output.println("Payment cancelled.", OutputManager::Color::YELLOW);
            return false;
        }
    }

    void PaymentCheckout::ensureSeatsReservedForRegistration(const std::string& registrationId) {
        auto registrations = dm.loadData<Registration>("registrations.json");
        bool changed = false;
        for (auto& reg : registrations) {
            if (reg.registrationId == registrationId) {
                for (auto& seat : reg.seats) {
                    if (seat.status != SeatStatus::RESERVED) {
                        seat.status = SeatStatus::RESERVED;
                        changed = true;
                    }
                    if (seat.registrationId.empty()) {
                        seat.registrationId = registrationId;
                        changed = true;
                    }
                }
                break;
            }
        }
        if (changed) {
            dm.saveData(registrations, "registrations.json");
        }
    }

    void PaymentCheckout::processPaymentForRegistration(Registration& registration, const Event& event) {
        double totalAmount = event.ticketPrice * registration.seats.size();

        try {
            PaymentMethod method = selectPaymentMethod();

            if (simulatePaymentProcess(totalAmount, method)) {
                Payment payment;
                payment.paymentId = generatePaymentId();
                payment.registrationId = registration.registrationId;
                payment.amount = totalAmount;
                payment.paymentDate = time(0);
                payment.paymentMethod = method;
                payment.paymentStatus = PaymentStatus::COMPLETED;

                auto payments = dm.loadData<Payment>("payments.json");
                payments.push_back(payment);
                dm.saveData(payments, "payments.json");

                auto registrations = dm.loadData<Registration>("registrations.json");
                for (auto& reg : registrations) {
                    if (reg.registrationId == registration.registrationId) {
                        reg.registrationStatus = RegistrationStatus::COMPLETED;

                        for (auto& seat : reg.seats) {
                            seat.status = SeatStatus::RESERVED;
                            if (seat.registrationId.empty()) seat.registrationId = reg.registrationId;
                        }
                        break;
                    }
                }
                dm.saveData(registrations, "registrations.json");

                auto events = dm.loadData<Event>("events.json");
                for (auto& ev : events) {
                    if (ev.eventId == event.eventId) {
                        ev.availableSeats -= registration.seats.size();
                        if (ev.availableSeats < 0) ev.availableSeats = 0;
                        break;
                    }
                }
                dm.saveData(events, "events.json");

                ensureSeatsReservedForRegistration(registration.registrationId);

                output.printHeader("PAYMENT SUCCESSFUL");
                output.println("Payment ID: " + payment.paymentId, OutputManager::Color::GREEN);
                output.println("Amount Paid: RM " + std::to_string(payment.amount));
                output.println("Payment Date: " + TimeManagement::formatTimeTToString(payment.paymentDate, "%Y-%m-%d %H:%M"));
                output.println(std::string("Payment Method: ") +
                    (method == PaymentMethod::ONLINE_BANKING ? "Online Banking" :
                        method == PaymentMethod::E_WALLET ? "E-Wallet" : "Card Payment"));
                output.println("----------------------------------------");
                output.println("Your registration is now confirmed!", OutputManager::Color::GREEN);

            }
            else {
                output.println("Payment failed.", OutputManager::Color::RED);
            }

        }
        catch (const std::exception& e) {
            output.println("Payment process cancelled.", OutputManager::Color::YELLOW);
        }

        system("pause");
    }

    std::vector<Payment> PaymentCheckout::getUserCompletedPayments() {
        auto payments = dm.loadData<Payment>("payments.json");
        std::vector<Payment> userPayments;

        for (const auto& payment : payments) {
            auto registrations = dm.loadData<Registration>("registrations.json");
            for (const auto& reg : registrations) {
                if (reg.registrationId == payment.registrationId &&
                    reg.customerId == user.userId &&
                    payment.paymentStatus == PaymentStatus::COMPLETED) {
                    userPayments.push_back(payment);
                    break;
                }
            }
        }

        return userPayments;
    }

    void PaymentCheckout::releaseSeatsOnRefund(const std::string& registrationId) {
        auto registrations = dm.loadData<Registration>("registrations.json");
        Registration* targetReg = nullptr;
        for (auto& reg : registrations) {
            if (reg.registrationId == registrationId) {
                for (auto& seat : reg.seats) {
                    seat.status = SeatStatus::AVAILABLE;
                    seat.registrationId.clear();
                }
                reg.registrationStatus = RegistrationStatus::CANCELLED;
                targetReg = &reg;
                break;
            }
        }
        dm.saveData(registrations, "registrations.json");

        if (targetReg) {
            auto events = dm.loadData<Event>("events.json");
            for (auto& ev : events) {
                if (ev.eventId == targetReg->eventId) {
                    ev.availableSeats += targetReg->seats.size();
                    if (ev.availableSeats > ev.totalSeats) {
                        ev.availableSeats = ev.totalSeats;
                    }
                    break;
                }
            }
            dm.saveData(events, "events.json");
        }
    }

    void PaymentCheckout::processRefund() {
        output.printHeader("Refund Request");

        auto userPayments = getUserCompletedPayments();
        std::vector<std::vector<std::string>> rows;
        std::vector<Payment> eligiblePayments;

        for (size_t i = 0; i < userPayments.size(); i++) {
            const auto& payment = userPayments[i];
            auto registrations = dm.loadData<Registration>("registrations.json");

            const Registration* reg = nullptr;
            for (const auto& r : registrations) {
                if (r.registrationId == payment.registrationId) {
                    reg = &r;
                    break;
                }
            }

            std::string eventName = "";
            bool eligible = true;

            if (reg) {
                Event event = getEventById(reg->eventId);
                eventName = event.eventName;

                time_t refundDeadline = event.startTime - (2 * 60 * 60);

                if (currentDateTime >= refundDeadline) {
                    eligible = false;
                }
            }

            if (eligible) {
                rows.push_back({
                    std::to_string(eligiblePayments.size() + 1),
                    payment.paymentId,
                    eventName,
                    "RM " + std::to_string(payment.amount),
                    TimeManagement::formatTimeTToString(payment.paymentDate, "%Y-%m-%d"),
                    "Eligible"
                    });
                eligiblePayments.push_back(payment);
            }
        }

        if (rows.empty()) {
            output.println("No eligible payments for refund.", OutputManager::Color::YELLOW);
            output.println("Note: Refunds are not allowed within 2 hours of event start time.", OutputManager::Color::CYAN);
            system("pause");
            return;
        }

        output.displayTable({ "No.", "Payment ID", "Event", "Amount", "Date", "Status" }, rows);

        output.print("Select payment index (1-" + std::to_string(rows.size()) + ") or 'c' to cancel: ");
        std::string choiceStr;
        std::getline(std::cin, choiceStr);

        if (validation.shouldCancel(choiceStr)) return;

        if (!isValidInteger(choiceStr)) {
            output.println("Invalid input! Please enter a whole number only (no decimals).", OutputManager::Color::RED);
            system("pause");
            return;
        }

        try {
            int choice = std::stoi(choiceStr) - 1;
            if (choice < 0 || choice >= eligiblePayments.size()) {
                output.println("Invalid selection!", OutputManager::Color::RED);
                system("pause");
                return;
            }

            Payment selectedPayment = eligiblePayments[choice];

            auto registrations = dm.loadData<Registration>("registrations.json");
            const Registration* reg = nullptr;
            for (const auto& r : registrations) {
                if (r.registrationId == selectedPayment.registrationId) {
                    reg = &r;
                    break;
                }
            }

            if (reg) {
                Event event = getEventById(reg->eventId);
                time_t currentTime = time(0);
                time_t refundDeadline = event.startTime - (2 * 60 * 60);

                if (currentTime >= refundDeadline) {
                    output.println("Refund not allowed. Event starts within 2 hours or has already started.", OutputManager::Color::RED);
                    output.println("Event Start Time: " + TimeManagement::formatTimeTToString(event.startTime, "%Y-%m-%d %H:%M"), OutputManager::Color::CYAN);
                    output.println("Current Time: " + TimeManagement::formatTimeTToString(currentTime, "%Y-%m-%d %H:%M"), OutputManager::Color::CYAN);
                    system("pause");
                    return;
                }
            }

            output.println("REFUND TERMS & CONDITIONS:", OutputManager::Color::YELLOW);
            output.println("- Processing fee: 20% of original payment");
            output.println("- Refund amount: RM " + std::to_string(selectedPayment.amount * 0.8));
            output.println("- Processing time: 3-5 business days");
            output.println("----------------------------------------");

            output.print("Do you agree to the terms and want to proceed? (y/n): ");
            std::string agree;
            std::getline(std::cin, agree);

            if (agree != "y" && agree != "Y") {
                output.println("Refund request cancelled.", OutputManager::Color::YELLOW);
                system("pause");
                return;
            }

            output.print("Enter refund reason (minimum 10 characters): ");
            std::string reason;
            std::getline(std::cin, reason);

            if (!validateRefundReason(reason)) {
                output.println("Invalid reason. Please provide a detailed reason (minimum 10 characters).", OutputManager::Color::RED);
                system("pause");
                return;
            }

            Refund refund;
            refund.refundId = "REF" + std::to_string(time(0));
            refund.paymentId = selectedPayment.paymentId;
            refund.refundAmount = selectedPayment.amount * 0.8;
            refund.refundDate = time(0);
            refund.reason = reason;

            auto payments = dm.loadData<Payment>("payments.json");
            for (auto& p : payments) {
                if (p.paymentId == selectedPayment.paymentId) {
                    p.paymentStatus = PaymentStatus::REFUNDED;
                    break;
                }
            }

            auto refunds = dm.loadData<Refund>("refunds.json");
            refunds.push_back(refund);
            dm.saveData(refunds, "refunds.json");
            dm.saveData(payments, "payments.json");

            releaseSeatsOnRefund(selectedPayment.registrationId);

            output.println("Refund request submitted successfully!", OutputManager::Color::GREEN);
            output.println("Refund ID: " + refund.refundId);
            output.println("Refund Amount: RM " + std::to_string(refund.refundAmount));
            output.println("Original Amount: RM " + std::to_string(selectedPayment.amount));
            output.println("Processing Fee: RM " + std::to_string(selectedPayment.amount * 0.2));
            output.println("----------------------------------------");

        }
        catch (...) {
            output.println("Invalid input! Please enter a valid number.", OutputManager::Color::RED);
            system("pause");
        }

        system("pause");
    }

    void PaymentCheckout::viewPaymentHistory() {
        auto userPayments = getUserAllPayments();
        std::vector<std::vector<std::string>> rows;

        for (const auto& payment : userPayments) {
            std::string eventName = "Unknown Event";
            auto registrations = dm.loadData<Registration>("registrations.json");
            for (const auto& reg : registrations) {
                if (reg.registrationId == payment.registrationId) {
                    Event ev = getEventById(reg.eventId);
                    if (!ev.eventName.empty()) eventName = ev.eventName;
                    break;
                }
            }

            rows.push_back({
                payment.paymentId,
                payment.registrationId,
                eventName,
                "RM " + std::to_string(payment.amount),
                TimeManagement::formatTimeTToString(payment.paymentDate, "%Y-%m-%d %H:%M"),
                (payment.paymentMethod == PaymentMethod::ONLINE_BANKING ? "Online Banking" :
                 payment.paymentMethod == PaymentMethod::E_WALLET ? "E-Wallet" : "Card"),
                (payment.paymentStatus == PaymentStatus::COMPLETED ? "COMPLETED" :
                 payment.paymentStatus == PaymentStatus::REFUNDED ? "REFUNDED" :
                 payment.paymentStatus == PaymentStatus::FAILED ? "FAILED" : "PENDING")
                });
        }

        output.printHeader("Payment History");
        if (rows.empty()) {
            output.println("No payment history found.", OutputManager::Color::YELLOW);
        }
        else {
            output.displayTable({ "Payment ID", "Registration ID", "Event", "Amount", "Date", "Method", "Status" }, rows);
        }
        system("pause");
    }

    std::vector<Payment> PaymentCheckout::getUserAllPayments() {
        auto payments = dm.loadData<Payment>("payments.json");
        std::vector<Payment> userPayments;

        for (const auto& payment : payments) {
            auto registrations = dm.loadData<Registration>("registrations.json");
            for (const auto& reg : registrations) {
                if (reg.registrationId == payment.registrationId &&
                    reg.customerId == user.userId) {
                    userPayments.push_back(payment);
                    break;
                }
            }
        }

        return userPayments;
    }

    Event PaymentCheckout::getEventById(const std::string& eventId) {
        auto events = dm.loadData<Event>("events.json");
        for (const auto& event : events) {
            if (event.eventId == eventId) {
                return event;
            }
        }
        return Event();
    }

    std::string PaymentCheckout::generatePaymentId() {
        return "PAY" + std::to_string(time(0));
    }

    std::string PaymentCheckout::getCurrentDateTime() {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);

        std::ostringstream oss;
        oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    bool PaymentCheckout::validateRefundReason(const std::string& reason) {
        return !reason.empty() && reason.length() >= 10;
    }
}
