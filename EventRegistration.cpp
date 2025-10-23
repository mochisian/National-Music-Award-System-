#include "EventRegistration.h" 
#include "Validation.h"
#include "OutputManager.h"
#include "TimeUtils.h"
#include "Model.h"
#include "DataManager.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace EventRegistration {

    EventRegistration::EventRegistration(User& user, time_t& currentDateTime) {
        this->currentDateTime = currentDateTime;
        showMenu(user);
    }

    void EventRegistration::showMenu(User& user) {
        Event event;
        OutputManager output;
        Validation validation;
        event.eventId = generateNewEventId();
        event.totalSeats = 100;
        event.organizerId = user.userId;
        event.status = EventStatus::SCHEDULED;

        bool isError = false;
        bool editing = true;

        while (editing) {
            displayMenu(event);

            if (isError) {
                output.println("Invalid input! Please enter a number from 1 to 6.", OutputManager::Color::RED);
                isError = false;
            }

            output.print("Enter the index of the attribute to fill (1-6), 'c' to cancel, or 's' to save: ");
            std::string userInput;
            std::getline(std::cin, userInput);

            if (validation.shouldSave(userInput)) {
                if (saveEvent(event, validation)) {
                    output.println("Event saved successfully!", OutputManager::Color::GREEN);
                    system("pause");
                    editing = false;
                }
                else {
                    output.println("Please fill in all the attributes.", OutputManager::Color::RED);
                    system("pause");
                }
                continue;
            }

            if (validation.shouldCancel(userInput)) {
                editing = false;
                continue;
            }

            if (!validation.validateMenuInput(1, 6, userInput)) {
                isError = true;
                continue;
            }

            int choice = std::stoi(userInput);
            editAttribute(event, choice, validation, output);
        }
    }


    void EventRegistration::displayMenu(const Event& event) {
        OutputManager output;

        std::vector<std::string> EventCreationAttribute = {
            "Event Name", event.eventName,
            "Description", event.description,
            "Event Date", event.eventDate == 0 ? "" : TimeManagement::formatTimeTToString(event.eventDate, "%Y-%m-%d"),
            "Start Time", event.startTime == 0 ? "" : TimeManagement::formatTimeTToString(event.startTime, "%H:%M"),
            "End Time", event.endTime == 0 ? "" : TimeManagement::formatTimeTToString(event.endTime, "%H:%M"),
            "Ticket Price", event.ticketPrice == 0.0 ? "" : std::to_string(event.ticketPrice)
        };

        system("cls");
        output.printHeader("Event Creation");
        output.printMenu(EventCreationAttribute);
    }

    void EventRegistration::editAttribute(Event& event, int choice, Validation& validation, OutputManager& output) {  
        switch (choice) {
        case 1:
            editEventName(event, validation, output);
            break;
        case 2:
            editEventDescription(event, validation, output);
            break;
        case 3:
            editEventDate(event, validation, output);
            break;
        case 4:
            editStartTime(event, validation, output);
            break;
        case 5:
            editEndTime(event, validation, output);
            break;
        case 6:
            editTicketPrice(event, validation, output);
            break;
        default:
            output.println("Invalid choice.", OutputManager::Color::RED);
            break;
        }
    }

    void EventRegistration::editEventName(Event& event, Validation& validation, OutputManager& output) {  
        std::string eventName;
        bool inputReceived;
        bool nameIsValid = false;

        do {
            inputReceived = getValidateInput("Enter event name: ", eventName, validation, output);

            if (!inputReceived) {
                return;
            }

            if (eventName.empty()) {
                displayMenu(event);
                output.println("Event name cannot be empty!", OutputManager::Color::RED);
            }
            else {
                event.eventName = eventName;
                nameIsValid = true;
            }
        } while (!nameIsValid);
    }

    void EventRegistration::editEventDescription(Event& event, Validation& validation, OutputManager& output) {  
        std::string description;
        bool inputReceived;
        bool descriptionIsValid = false;
        do {
            inputReceived = getValidateInput("Enter event description: ", description, validation, output);
            
            if (!inputReceived) {
                return;
            }
            if (description.empty()) {
                displayMenu(event);
                output.println("Event description cannot be empty!", OutputManager::Color::RED);
            }
            else {
                event.description = description;
                descriptionIsValid = true;
            }
        } while (!descriptionIsValid);
    }

    void EventRegistration::editEventDate(Event& event, Validation& validation, OutputManager& output) {  
        std::string dateStr;
        bool inputReceived;
        Validation::DateValidationStatus status;

        do {
            inputReceived = getValidateInput("Enter event date (YYYY-MM-DD): ", dateStr, validation, output);

            if (!inputReceived) {
                return;
            }

            status = validation.validateAndParseDate(dateStr, event.eventDate, currentDateTime);

            switch (status) {
            case Validation::DateValidationStatus::VALID:
                break;
            case Validation::DateValidationStatus::INVALID_FORMAT:
                displayMenu(event);
                output.println("Invalid date format. Please use YYYY-MM-DD.", OutputManager::Color::RED);
                break;
            case Validation::DateValidationStatus::DATE_IN_PAST:
                displayMenu(event);
                output.println("Invalid date. The date must be today or in the future.", OutputManager::Color::RED);
                break;
            case Validation::DateValidationStatus::TOO_SOON:
                displayMenu(event);
                output.println("Invalid date. Events must be scheduled at least 7 days from today.", OutputManager::Color::RED);
                break;
            default:
                displayMenu(event);
                output.println("An unknown date validation error occurred.", OutputManager::Color::RED);
                break;
            }
        } while (status != Validation::DateValidationStatus::VALID);
    }

    void EventRegistration::editStartTime(Event& event, Validation& validation, OutputManager& output) {  
        std::string timeStr;
        bool validTimeEntered = false;

        do {
            if (!getValidateInput("Enter start time (HH:MM 24-hour format): ", timeStr, validation, output)) {
                return;
            }

            time_t newStartTime;
            if (TimeManagement::convertTimeStringToTimeT(timeStr, event.eventDate, newStartTime)) {
                if (event.endTime != 0 && newStartTime >= event.endTime) {
                    displayMenu(event);
                    output.println("Start time must be before the end time.", OutputManager::Color::RED);
                }
                else {
                    event.startTime = newStartTime;
                    validTimeEntered = true;
                }
            }
            else {
                displayMenu(event);
                output.println("Invalid time format. Please use HH:MM 24-hour format.", OutputManager::Color::RED);
            }
        } while (!validTimeEntered);
    }

    void EventRegistration::editEndTime(Event& event, Validation& validation, OutputManager& output) {  
        std::string timeStr;
        bool validTimeEntered = false;

        do {
            if (!getValidateInput("Enter end time (HH:MM 24-hour format): ", timeStr, validation, output)) {
                return;
            }

            time_t newEndTime;
            if (TimeManagement::convertTimeStringToTimeT(timeStr, event.eventDate, newEndTime)) {
                if (newEndTime <= event.startTime) {
                    displayMenu(event);
                    output.println("End time must be after the start time.", OutputManager::Color::RED);
                }
                else {
                    event.endTime = newEndTime;
                    validTimeEntered = true;
                }
            }
            else {
                displayMenu(event);
                output.println("Invalid time format. Please use HH:MM.", OutputManager::Color::RED);
            }
        } while (!validTimeEntered);
    }

    void EventRegistration::editTicketPrice(Event& event, Validation& validation, OutputManager& output) {  
        std::string priceStr;
        bool validPriceEntered = false;

        do {
            if (!getValidateInput("Enter ticket price: ", priceStr, validation, output)) {
                return;
            }


            try {
                double price = std::stod(priceStr);
                if (price < 0.0 || !validation.isFloat(priceStr)) {
                    throw std::invalid_argument("Price cannot be negative.");
                }
                event.ticketPrice = price;
                validPriceEntered = true;
            }
            catch (const std::exception& e) {
                displayMenu(event);
                output.println("Invalid price. Please enter a valid, non-negative number.", OutputManager::Color::RED);
            }
        } while (!validPriceEntered);
    }

    bool EventRegistration::getValidateInput(const std::string& prompt, std::string& result, Validation& validation, OutputManager& output) {  
        output.print(prompt + "(press 'c' to cancel): ");
        std::string input;
        std::getline(std::cin, input);

        if (validation.shouldCancel(input)) {
            return false;
        }

        result = input;
        return true;
    }

    std::string EventRegistration::generateNewEventId() {  
        DataManager dm;
        return dm.generateNewEventId();
    }

    bool EventRegistration::saveEvent(const Event& event, Validation& validation) {  
        if (validation.isObjectEmpty(event)) {
            return false;
        }
        DataManager dm;
        std::vector<Event> allEvents = dm.getAllActiveEvents();
        allEvents.push_back(event);
        dm.saveData(allEvents, "events.json");
        return true;
    }
}