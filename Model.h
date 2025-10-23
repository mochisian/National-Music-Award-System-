#pragma once
#include <ctime>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class UserType { ADMIN, ORGANIZER, CUSTOMER };
enum class SeatStatus { AVAILABLE, RESERVED };
enum class PaymentStatus { PENDING, COMPLETED, FAILED, REFUNDED };
enum class PaymentMethod { ONLINE_BANKING, E_WALLET, CARD_PAYMENT };
enum class EventStatus { SCHEDULED, ONGOING, COMPLETED, CANCELLED };
enum class RegistrationStatus { COMPLETED, CANCELLED, PENDING };
enum class AttendanceStatus { PRESENT, ABSENT };

NLOHMANN_JSON_SERIALIZE_ENUM(UserType, {
    {UserType::ADMIN, "ADMIN"},
    {UserType::ORGANIZER, "ORGANIZER"},
    {UserType::CUSTOMER, "CUSTOMER"}
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(SeatStatus, {
            {SeatStatus::AVAILABLE, "AVAILABLE"},
            {SeatStatus::RESERVED, "RESERVED"}
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(PaymentStatus, {
        {PaymentStatus::COMPLETED, "COMPLETED"},
        {PaymentStatus::FAILED, "FAILED"},
        {PaymentStatus::PENDING, "PENDING"},
        {PaymentStatus::REFUNDED, "REFUNDED"}
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(PaymentMethod, {
        {PaymentMethod::CARD_PAYMENT, "CARD_PAYMENT"},
        {PaymentMethod::E_WALLET, "E_WALLET"},
        {PaymentMethod::ONLINE_BANKING, "ONLINE_BANKING"}
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(EventStatus, {
        {EventStatus::CANCELLED, "CANCELLED"},
        {EventStatus::COMPLETED, "COMPLETED"},
        {EventStatus::ONGOING, "ONGOING"},
        {EventStatus::SCHEDULED, "SCHEDULED"}

        })

    NLOHMANN_JSON_SERIALIZE_ENUM(RegistrationStatus, {
        {RegistrationStatus::CANCELLED, "CANCELLED"},
        {RegistrationStatus::COMPLETED, "COMPLETED"},
        {RegistrationStatus::PENDING, "PENDING"}
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(AttendanceStatus, {
        {AttendanceStatus::ABSENT, "ABSENT"},
        {AttendanceStatus::PRESENT, "PRESENT"}
        })


    struct User {
    std::string userId;
    std::string username;
    std::string passwordHash;
    std::string email;
    std::string phoneNumber;
    UserType userType;
    bool isDeleted = false;

    void softDelete() {
        this->isDeleted = true;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, userId, username, passwordHash, email, phoneNumber, userType, isDeleted)
};

struct Event {
    std::string eventId;
    std::string eventName;
    std::string description;
    time_t eventDate = 0;
    time_t startTime = 0;
    time_t endTime = 0;
    std::string organizerId;
    double ticketPrice = 0;
    EventStatus status;
    int totalSeats = 100;
    int availableSeats = 100;
    bool isDeleted = false;

    void softDelete() {
        this->isDeleted = true;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Event, eventId, eventName, description, eventDate, startTime, endTime, organizerId, ticketPrice,
        status, totalSeats, isDeleted)
};

struct Seat {
    std::string seatId;
    std::string row;
    std::string column;
    std::string registrationId;
    SeatStatus status;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Seat, seatId, row, column, registrationId, status)
};

struct Registration {
    std::string registrationId;
    std::string customerId;
    std::string eventId;
    std::vector<Seat> seats;
    time_t registerTime;
    RegistrationStatus registrationStatus;
    bool isDeleted = false;

    void softDelete() {
        this->isDeleted = true;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Registration, registrationId, customerId, eventId, seats, registerTime, registrationStatus, isDeleted)
};

struct Payment {
    std::string paymentId;
    std::string registrationId;
    double amount;
    time_t paymentDate;
    PaymentMethod paymentMethod;
    PaymentStatus paymentStatus;
    bool isDeleted = false;

    void softDelete() {
        this->isDeleted = true;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Payment, paymentId, registrationId, amount, paymentDate, paymentMethod, paymentStatus, isDeleted)
};

struct Refund {
    std::string refundId;
    std::string paymentId;
    double refundAmount;
    time_t refundDate;
    std::string reason;
    bool isDeleted = false;

    void softDelete() {
        this->isDeleted = true;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Refund, refundId, paymentId, refundAmount, refundDate, reason, isDeleted)
};

struct Song {
    std::string songId;
    std::string title;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Song, songId, title)
};

struct Performance {
    std::string performanceId;
    std::string eventId;
    std::string performerName;
    std::vector<Song> songs;
    time_t startTime;
    time_t endTime;
    bool isDeleted = false;

    void softDelete() {
        this->isDeleted = true;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Performance, performanceId, eventId, performerName, songs, startTime, endTime, isDeleted)
};

struct Vote {
    std::string voteId;
    std::string songId;
    std::string registrationId;
    int voteWeight;
    bool isDeleted = false;

    void softDelete() {
        this->isDeleted = true;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vote, voteId, songId, registrationId, voteWeight, isDeleted)
};

struct Attendance {
    std::string attendanceId;
    std::string registrationId;
    AttendanceStatus attendanceStatus;
    time_t attendanceTime;
    bool isDeleted = false;

    void softDelete() {
        this->isDeleted = true;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Attendance, attendanceId, registrationId, attendanceStatus, attendanceTime, isDeleted)
};