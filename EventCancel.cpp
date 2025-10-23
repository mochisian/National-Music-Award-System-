#include "EventCancel.h"
#include "TimeUtils.h"
#include "Validation.h"

namespace EventCancellation {
	void EventCancellation::cancelEvent(const std::string& eventId, const std::vector<std::string>& chosenEvent, time_t& currentDateTime) {
		DataManager dm;
		Event event = dm.getEventFromEventId(eventId);
		OutputManager output;
		Validation validation;

		Validation::DateValidationStatus status = validation.validateEventCancellation(event.eventDate, currentDateTime);
		if (status == Validation::DateValidationStatus::TOO_SOON) {
			output.println("This event cannot be cancelled because it starts in less than 7 days.",
				OutputManager::Color::RED);
			return;
		}

		bool confirming = true;
		while (confirming) {
			output.clearScreen();
			output.printHeader("Event cancellation");
			output.displayTable({ "No.", "Event Id.","Event Name", "Date", "Time", "Status" }, { chosenEvent });
			output.print("\nAre you sure you want to cancel the event? (y/n): ");

			std::string userInput;
			std::getline(std::cin, userInput);

			if (userInput == "Y" || userInput == "y") {
				updateEventStatus(eventId);
				output.println("Event status updated!");
				return;
			}
			else if (userInput == "N" || userInput == "n") {
				return;
			}
			else {
				output.println("Invalid input!", OutputManager::Color::RED);
				system("pause");
			}
		}
	}

	void EventCancellation::updateEventStatus(const std::string& eventId) {
		DataManager dm;
		std::vector<Event> allEvents = dm.getAllActiveEvents();
		std::vector<Registration> allRegistrations = dm.getAllActiveRegistrations();
		std::vector<Payment> allPayment = dm.getAllActivePayments();
		std::vector<Refund> allRefund = dm.getAllActiveRefunds();
		
		for (Registration& reg : allRegistrations) {
			if (reg.eventId == eventId) {
				reg.registrationStatus = RegistrationStatus::CANCELLED;
			}
		}

		for (Event& event : allEvents) {
			if (event.eventId == eventId) {
				event.status = EventStatus::CANCELLED;
			}
		}

		for (Payment& payment : allPayment) {
			if (dm.getRegistrationFromRegistrationId(payment.registrationId).eventId == eventId) {
				payment.paymentStatus = PaymentStatus::REFUNDED;
				Refund refund;
				refund.refundId = dm.generateNewRefundId();
				refund.paymentId = payment.paymentId;
				refund.refundAmount = payment.amount;
				refund.refundDate = time(nullptr);
				refund.reason = "Event Cancelled";
				allRefund.push_back(refund);
			}
		}
				
		dm.saveData(allRegistrations, "Registrations.json");
		dm.saveData(allEvents, "Events.json");
		dm.saveData(allPayment, "Payments.json");
		dm.saveData(allRefund, "Refunds.json");
		allRefund.clear();
	}
}