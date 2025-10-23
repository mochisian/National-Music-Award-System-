#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream> 
#include <algorithm>
#include <nlohmann/json.hpp>
#include "Model.h"
#include "TimeUtils.h"
#include <iomanip>

class DataManager {
public:
    template <typename T>
    void saveData(const std::vector<T>& data, const std::string& filename) {
        nlohmann::json j = data;
        std::ofstream file(filename);
        file << j.dump(4);
    }

    std::string userFileName = "users.json";
    std::string eventFileName = "events.json";
    std::string registrationFileName = "registrations.json"; 
    std::string paymentFileName = "payments.json";
    std::string refundFileName = "refunds.json";
    std::string performanceFileName = "performances.json";
    std::string voteFileName = "votes.json";
    std::string attendanceFileName = "attendances.json";

    template <typename T>
    std::vector<T> loadData(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::ofstream newFile(filename);
            return {};
        }
        nlohmann::json j;
        try {
            file >> j;
            if (j.is_null()) {
                return {};
            }
        }
        catch (nlohmann::json::parse_error& e) {
            return {};
        }
        return j.get<std::vector<T>>();
    }

    std::string generateNewEventId();
    std::string generateNewRegistrationId();
    std::string generateNewAttendanceId();
    std::string generateNewVoteId();
    std::string generateNewRefundId();
    int getAvailableSeats(const std::string& eventId);
    Event getEventFromEventId(const std::string& targetEventId);
    std::vector<Seat> getOccupiedSeatsForEvent(const std::string& eventId);
    void getAttendanceFromEvent(std::vector<std::vector<int>>& venueAttendance, Event& event);
    Registration getRegistrationFromRegistrationId(const std::string& registrationId);
    void updateEventStatus(time_t& currentDateTime);
    void updateAttendanceStatus();
    std::vector<std::vector<std::string>> loadScheduledAndOngoingDataOfUser(const std::string& userId);
    Registration getRegistrationFromEventId(const std::string& eventId, const std::string& userId);
    std::vector<Registration> getRegistrationsForEvent(const std::string& eventId);
    User getUserFromUserId(const std::string& userId);
    Payment getPaymentFromPaymentId(const std::string& paymentId);
    std::vector<std::vector<std::string>> loadScheduledAndOngoingDataOfOrganizer(std::string& userId);
    void loadCompletedSalesOfEvent(std::vector<std::vector<std::string>>& sales, double& totalSales, Event& event);
    void loadRefundedSalesOfEvent(std::vector<std::vector<std::string>>& sales, Event& event);
    std::vector<std::vector<std::string>> loadAttendanceMonitoring(Event& event);
    std::vector<std::vector<std::string>> loadSeatMonitoring(Event& event);
    void updateRegistrationStatus(time_t& currentDateTime);
    std::vector<Event> getAllScheduledEvent();

    std::vector<User> getAllActiveUsers();
    std::vector<Event> getAllActiveEvents();
    std::vector<Registration> getAllActiveRegistrations();
    std::vector<Payment> getAllActivePayments();
    std::vector<Refund> getAllActiveRefunds();
    std::vector<Performance> getAllActivePerformances();
    std::vector<Vote> getAllActiveVotes();
    std::vector<Attendance> getAllActiveAttendances();
};
