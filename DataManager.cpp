#include "DataManager.h"
#include <unordered_set>
#include "EventRegistration.h"
#include <iostream>

std::string DataManager::generateNewEventId() {
	std::vector<Event> events = getAllActiveEvents();

	if (events.empty()) {
		return "EV001";
	}

	std::string lastId = events.back().eventId;
	int nextIdNum = std::stoi(lastId.substr(2)) + 1;

	std::ostringstream oss;
	oss << "EV" << std::setfill('0') << std::setw(3) << nextIdNum;
	return oss.str();
}

std::string DataManager::generateNewAttendanceId() {
	std::vector<Attendance> attendances = getAllActiveAttendances();

	if (attendances.empty()) {
		return "ATD001";
	}

	std::string lastId = attendances.back().attendanceId;
	int nextIdNum = std::stoi(lastId.substr(3)) + 1;

	std::ostringstream oss;
	oss << "ATD" << std::setfill('0') << std::setw(3) << nextIdNum;
	return oss.str();
}

std::string DataManager::generateNewVoteId() {
	std::vector<Vote> votes = getAllActiveVotes();
	if (votes.empty()) {
		return "VOTE001";
	}

	std::string lastId = votes.back().voteId;
	int nextIdNum = std::stoi(lastId.substr(4)) + 1;

	std::ostringstream oss;
	oss << "VOTE" << std::setfill('0') << std::setw(3) << nextIdNum;
	return oss.str();
}

std::string DataManager::generateNewRegistrationId() {
	std::vector<Registration> registrations = getAllActiveRegistrations();

	if (registrations.empty()) {
		return "REG001";
	}

	std::string lastId = registrations.back().registrationId;
	int nextIdNum = std::stoi(lastId.substr(3)) + 1;

	std::ostringstream oss;
	oss << "REG" << std::setfill('0') << std::setw(3) << nextIdNum;
	return oss.str();
}

std::string DataManager::generateNewRefundId() {
	std::vector<Refund> allRefund = getAllActiveRefunds();

	if (allRefund.empty()) {
		return "REF001";
	}

	std::string lastId = allRefund.back().refundId;
	int nextIdNum = std::stoi(lastId.substr(3)) + 1;

	std::ostringstream oss;
	oss << "REF" << std::setfill('0') << std::setw(3) << nextIdNum;
	return oss.str();
}

int DataManager::getAvailableSeats(const std::string& eventId) {
	int totalSeats = getEventFromEventId(eventId).totalSeats;

	std::vector<Registration> registrations = getAllActiveRegistrations();

	int reservedSeats = 0;
	for (const auto& reg : registrations) {
		if (reg.eventId == eventId && reg.registrationStatus != RegistrationStatus::CANCELLED) {
			reservedSeats += reg.seats.size();
		}
	}

	return totalSeats - reservedSeats;
}

Event DataManager::getEventFromEventId(const std::string& targetEventId) {
	std::vector<Event> events = getAllActiveEvents();

	for (auto a : events) {
		if (a.eventId == targetEventId) {
			return a;
		}
	}
	return Event();
}

std::vector<Seat> DataManager::getOccupiedSeatsForEvent(const std::string& eventId) {
	std::vector<Seat> occupiedSeats;

	for (Registration& reg : getAllActiveRegistrations()) {
		if (reg.registrationStatus == RegistrationStatus::CANCELLED) {
			continue;
		}
		if (reg.eventId == eventId) {
			for (const auto& seat : reg.seats) {
				if (seat.status == SeatStatus::RESERVED) {
					occupiedSeats.push_back(seat);
				}
			}
		}
	}

	return occupiedSeats;
}

std::vector<Registration> DataManager::getRegistrationsForEvent(const std::string& eventId) {
	std::vector<Registration> allRegistrations = getAllActiveRegistrations();
	std::vector<Registration> eventRegistrations;
	for (const Registration& reg : allRegistrations) {
		if (reg.eventId == eventId && reg.registrationStatus != RegistrationStatus::CANCELLED) {
			eventRegistrations.push_back(reg);
		}
	}
	return eventRegistrations;
}

void DataManager::getAttendanceFromEvent(std::vector<std::vector<int>>& venueAttendance, Event& event) {
	std::vector<Registration> allRegistrations = getAllActiveRegistrations();
	std::vector<Attendance> allAttendances = getAllActiveAttendances();

	std::vector<Registration> eventRegistrations;
	for (const Registration& reg : allRegistrations) {
		if (reg.eventId == event.eventId &&
			(reg.registrationStatus != RegistrationStatus::CANCELLED)) {
			eventRegistrations.push_back(reg);
		}
	}

	std::unordered_map<std::string, AttendanceStatus> attendanceMap;
	for (const Attendance& att : allAttendances) {
		attendanceMap[att.registrationId] = att.attendanceStatus;
	}

	for (auto& row : venueAttendance) {
		for (auto& seat : row) {
			seat = -1;
		}
	}

	for (const Registration& reg : eventRegistrations) {
		bool isPresent = false;
		auto attendanceIt = attendanceMap.find(reg.registrationId);


		if (attendanceIt != attendanceMap.end()) {
			if (attendanceIt->second == AttendanceStatus::PRESENT) {
				isPresent = true;
			}
		}

		for (const Seat& seat : reg.seats) {
			int rowIndex = std::stoi(seat.row) - 1;
			int columnIndex = std::stoi(seat.column) - 1;

			if (rowIndex >= 0 && rowIndex < venueAttendance.size() &&
				columnIndex >= 0 && columnIndex < venueAttendance[0].size()) {
				venueAttendance[rowIndex][columnIndex] = isPresent ? 1 : 0;
			}
		}
	}
}

Registration DataManager::getRegistrationFromRegistrationId(const std::string& registrationId) {
	std::vector<Registration> allRegistrations = getAllActiveRegistrations();
	for (Registration reg : allRegistrations) {
		if (reg.registrationId == registrationId) {
			return reg;
		}
	}
	return Registration();
}

Registration DataManager::getRegistrationFromEventId(const std::string& eventId, const std::string& userId) {
	std::vector<Registration> allRegistrations = getAllActiveRegistrations();
	std::vector<Registration> relevantRegistrations;

	for (Registration reg : allRegistrations) {
		if (reg.eventId == eventId && reg.customerId == userId) {
			return reg;
		}
	}
}

User DataManager::getUserFromUserId(const std::string& userId) {
	std::vector<User> allUsers = getAllActiveUsers();

	for (User user : allUsers) {
		if (user.userId == userId) {
			return user;
		}
	}
}

Payment DataManager::getPaymentFromPaymentId(const std::string& paymentId){
	std::vector<Payment> allPayments = getAllActivePayments();

	for (Payment payment : allPayments) {
		if (payment.paymentId == paymentId) {
			return payment;
		}
	}
}

std::vector<User> DataManager::getAllActiveUsers() {
	auto allUsers = loadData<User>(userFileName);
	return allUsers;
}
	
std::vector<Event> DataManager::getAllActiveEvents() {
	auto allEvents = loadData<Event>(eventFileName);
	return allEvents;
}

std::vector<Event> DataManager::getAllScheduledEvent() {
	std::vector<Event> scheduledEvent;
	for (Event event : getAllActiveEvents()) {
		if (event.status == EventStatus::SCHEDULED) {
			scheduledEvent.push_back(event);
		}
	}
	return scheduledEvent;
}

std::vector<Registration> DataManager::getAllActiveRegistrations() {
	auto allRegistrations = loadData<Registration>(registrationFileName);
	return allRegistrations;
}

std::vector<Payment> DataManager::getAllActivePayments() {
	auto allPayments = loadData<Payment>(paymentFileName);
	return allPayments;
}

std::vector<Refund> DataManager::getAllActiveRefunds() {
	auto allRefunds = loadData<Refund>(refundFileName);
	return allRefunds;
}

std::vector<Performance> DataManager::getAllActivePerformances() {
	auto allPerformances = loadData<Performance>(performanceFileName);
	return allPerformances;
}

std::vector<Vote> DataManager::getAllActiveVotes() {
	auto allVotes = loadData<Vote>(voteFileName);
	return allVotes;
}

std::vector<Attendance> DataManager::getAllActiveAttendances() {
	auto allAttendances = loadData<Attendance>(attendanceFileName);
	return allAttendances;
}

void DataManager::updateEventStatus(time_t& currentDateTime) {
	std::vector<Event> allEvents = getAllActiveEvents();
	for (Event& event : allEvents) {
		if (event.status == EventStatus::CANCELLED) {
			continue;
		}
		else if (currentDateTime >= event.startTime && currentDateTime <= event.endTime) {
			event.status = EventStatus::ONGOING;
		}
		else if (currentDateTime < event.startTime) {
			event.status = EventStatus::SCHEDULED;
		}
		else if (currentDateTime > event.endTime) {
			event.status = EventStatus::COMPLETED;
		}
	}
	saveData(allEvents, eventFileName);
	updateAttendanceStatus();
	updateRegistrationStatus(currentDateTime);
}

void DataManager::updateRegistrationStatus(time_t& currentDateTime) {
	Validation validation;
	bool changeMade = false;
	std::vector<Registration> allRegistrations = getAllActiveRegistrations();
	for (Registration& reg : allRegistrations) {
		if (reg.registrationStatus == RegistrationStatus::PENDING) {
			if (!validation.validateUnpaidRegistration(reg.registerTime, currentDateTime)) {
				changeMade = true;
				reg.registrationStatus = RegistrationStatus::CANCELLED;
			}
		}
	}
	if (changeMade) {
		saveData(allRegistrations, registrationFileName);
	}
}

void DataManager::updateAttendanceStatus() {
	std::vector<Attendance> allAttendances = loadData<Attendance>(attendanceFileName);
	std::vector<Event> allEvents = loadData<Event>(eventFileName);
	std::vector<Registration> allRegistrations = loadData<Registration>(registrationFileName);

	bool changesMade = false;

	for (const Event& event : allEvents) {
		if (event.status == EventStatus::COMPLETED) {
			std::vector<Registration> eventRegistrations;
			for (const Registration& reg : allRegistrations) {
				if (reg.eventId == event.eventId) {
					eventRegistrations.push_back(reg);
				}
			}

			for (const Registration& reg : eventRegistrations) {
				bool attendanceExists = false;

				for (const Attendance& attendance : allAttendances) {
					if (attendance.registrationId == reg.registrationId) {
						attendanceExists = true;
						break;
					}
				}

				if (!attendanceExists) {
					Attendance att;
					att.attendanceId = generateNewAttendanceId();
					att.registrationId = reg.registrationId;
					att.attendanceStatus = AttendanceStatus::ABSENT;
					att.isDeleted = false;

					allAttendances.push_back(att);
					changesMade = true;

					std::cout << "Created ABSENT attendance for registration: "
						<< reg.registrationId << std::endl;
				}
			}
		}
	}

	if (changesMade) {
		saveData(allAttendances, attendanceFileName);
	}
}

std::vector<std::vector<std::string>> DataManager::loadScheduledAndOngoingDataOfUser(const std::string& userId) {
	std::vector<std::vector<std::string>> eventsData;
	std::vector<Registration> allRegistrations = getAllActiveRegistrations();
	std::vector<Registration> relevantRegistrations;
	std::unordered_map<std::string, Event> eventCache;

	for (const Registration& reg : allRegistrations) {
		if (reg.registrationStatus == RegistrationStatus::CANCELLED) continue;
		if (reg.customerId != userId) continue;

		if (eventCache.find(reg.eventId) == eventCache.end()) {
			eventCache[reg.eventId] = getEventFromEventId(reg.eventId);
		}
		const Event& ev = eventCache[reg.eventId];

		if (ev.status == EventStatus::SCHEDULED || ev.status == EventStatus::ONGOING) {
			relevantRegistrations.push_back(reg);
		}
	}

	if (relevantRegistrations.empty()) {
		eventsData.push_back({ "No ongoing events available", "", "", "", "", "", "", "" });
		return eventsData;
	}

	std::sort(relevantRegistrations.begin(), relevantRegistrations.end(),
		[&eventCache](const Registration& a, const Registration& b) {
			const Event& ea = eventCache.at(a.eventId);
			const Event& eb = eventCache.at(b.eventId);
			if (ea.eventDate != eb.eventDate) return ea.eventDate < eb.eventDate;
			return ea.startTime < eb.startTime;
		});

	int rowNo = 1;
	for (const Registration& reg : relevantRegistrations) {
		const Event& event = eventCache.at(reg.eventId);
		std::vector<std::string> eventRow;

		eventRow.push_back(std::to_string(rowNo++));         // No.
		eventRow.push_back(reg.registrationId);               // Reg Id
		eventRow.push_back(event.eventName);                  // Event name
		eventRow.push_back(TimeManagement::formatTimeTToString(event.eventDate, "%Y-%m-%d")); // Date

		std::string timeRange = TimeManagement::formatTimeTToString(event.startTime, "%H:%M")
			+ " - " + TimeManagement::formatTimeTToString(event.endTime, "%H:%M");
		eventRow.push_back(timeRange);                        // Time

		eventRow.push_back(std::to_string(reg.seats.size())); // Number of ticket

		// Seat Ids
		std::string seatIds;
		for (size_t i = 0; i < reg.seats.size(); ++i) {
			if (i) seatIds += ", ";
			seatIds += reg.seats[i].seatId;
		}
		eventRow.push_back(seatIds);

		// Status
		std::string status;
		Event registeredEvent = getEventFromEventId(reg.eventId);
		switch (registeredEvent.status) {
		case EventStatus::COMPLETED: status = "Completed"; break;
		case EventStatus::SCHEDULED: status = "Scheduled"; break;
		case EventStatus::ONGOING: status = "Ongoing"; break;
		case EventStatus::CANCELLED: status = "Cancelled"; break;
		default: status = "Unknown"; break;
		}
		eventRow.push_back(status);

		eventsData.push_back(eventRow);
	}

	return eventsData;
}


std::vector<std::vector<std::string>> DataManager::loadScheduledAndOngoingDataOfOrganizer(std::string& userId) {
	std::vector<std::vector<std::string>> eventsData;
	std::vector<Event> allEvents = getAllActiveEvents();
	std::vector<Event> relevantEvents;

	for (Event& event : allEvents) {
		if(event.organizerId == userId || userId == "ADM001") {
			relevantEvents.push_back(event);
		}
	}
	
	std::sort(relevantEvents.begin(), relevantEvents.end(), [](const Event& a, const Event& b) {
		if (a.eventDate != b.eventDate) {
			return a.eventDate < b.eventDate; 
		}
		return a.startTime < b.startTime;
	});

	int eventNumber = 1;
	for(Event& event : relevantEvents){
		std::vector<std::string> eventRow;
		// Number
		eventRow.push_back(std::to_string(eventNumber++));

		// Event Id
		eventRow.push_back(event.eventId);

		// Event Name
		eventRow.push_back(event.eventName);

		// Date
		eventRow.push_back(TimeManagement::formatTimeTToString(event.eventDate, "%Y-%m-%d"));

		// Time
		std::string timeRange = TimeManagement::formatTimeTToString(event.startTime, "%H:%M") + " - " +
			TimeManagement::formatTimeTToString(event.endTime, "%H:%M");
		eventRow.push_back(timeRange);

		// Event status
		std::string status;
		if (event.status == EventStatus::SCHEDULED) {
			status = "Schedueling";
		}
		else if (event.status == EventStatus::ONGOING) {
			status = "Ongoing";
		}
		else if (event.status == EventStatus::COMPLETED) {
			status = "Completed";
		}
		else if (event.status == EventStatus::CANCELLED) {
			status = "Cancelled";
		}

		eventRow.push_back(status);

		eventsData.push_back(eventRow);
	}

	if (eventsData.empty()) {
		eventsData.push_back({ "No ongoing events available", "", "", "", "", "" });
	}

	return eventsData;
}

// { "No.", "Time", "Payment Id", "Amount", "Method", "Status", "Customer"
void DataManager::loadCompletedSalesOfEvent(std::vector<std::vector<std::string>>& sales, double& totalSales, Event& event) {
	std::vector<Payment> allPayments = getAllActivePayments();

	int paymentNumber = 1;
	for(Payment& payment : allPayments){
		std::vector<std::string> paymentRow;

		if (getRegistrationFromRegistrationId(payment.registrationId).eventId != event.eventId) {
			continue;
		}
		if (payment.paymentStatus != PaymentStatus::COMPLETED && getRegistrationFromRegistrationId(payment.registrationId).eventId != event.eventId) {
			continue;
		}

		// Number
		paymentRow.push_back(std::to_string(paymentNumber++));
	
		// Time
		paymentRow.push_back(TimeManagement::formatTimeTToString(payment.paymentDate, "%Y-%m-%d %H:%M"));

		// Payment Id
		paymentRow.push_back(payment.paymentId);

		// Amount
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << payment.amount;
		paymentRow.push_back(ss.str());

		// Method
		std::string paymentMethod;
		switch (payment.paymentMethod) {
		case PaymentMethod::ONLINE_BANKING:
			paymentMethod = "Online Banking";
			break;
		case PaymentMethod::E_WALLET:
			paymentMethod = "E Wallet";
			break;
		case PaymentMethod::CARD_PAYMENT:
			paymentMethod = "Card Payment";
			break;
		}
		paymentRow.push_back(paymentMethod);

		// Status
		std::string paymentStatus;
		switch (payment.paymentStatus) {
		case PaymentStatus::FAILED:
			paymentStatus = "Failed";
			break;
		case PaymentStatus::REFUNDED:
			paymentStatus = "Refunded";
			break;
		case PaymentStatus::COMPLETED:
			paymentStatus = "Completed";
			break;
		case PaymentStatus::PENDING:
			paymentStatus = "Pending";
			break;
		}
		paymentRow.push_back(paymentStatus);

		// Customer Name
		paymentRow.push_back(getUserFromUserId(getRegistrationFromRegistrationId(payment.registrationId).customerId).username);

		sales.push_back(paymentRow);
		totalSales += payment.amount;
	
	}

	if(sales.empty()){
		sales.push_back({ "No completed sales available", "", "", "", "", "", "" });
	}
}

// {No. RefundId, PaymentId, Customer, Refund Amount, Reason, Status}
void DataManager::loadRefundedSalesOfEvent(std::vector<std::vector<std::string>>& sales, Event& event) {
	std::vector<Refund> allRefunds = getAllActiveRefunds();

	int refundNumber = 1;
	for(Refund refund : allRefunds){

		if (getRegistrationFromRegistrationId(getPaymentFromPaymentId(refund.paymentId).registrationId).eventId != event.eventId) {
			continue;
		}

		std::vector<std::string> refundRow;

		// Number
		refundRow.push_back(std::to_string(refundNumber++));

		// Time
		refundRow.push_back(TimeManagement::formatTimeTToString(refund.refundDate, "%Y-%m-%d %H:%M"));

		// Redund Id
		refundRow.push_back(refund.refundId);

		// Payment Id
		refundRow.push_back(refund.paymentId);

		// Customer Name
		refundRow.push_back(getUserFromUserId(getRegistrationFromRegistrationId(getPaymentFromPaymentId(refund.paymentId).registrationId).customerId).username);

		// Refund Amount
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << refund.refundAmount;
		refundRow.push_back(ss.str());

		// Reason
		refundRow.push_back(refund.reason);

		// Status
		refundRow.push_back("Refunded");

		sales.push_back(refundRow);
	}

	if(sales.empty()){
		sales.push_back({ "No refunded sales available", "", "", "", "", "", "", ""});
	}
}

 //No. AttendanceId Customer Event Status Time Seat
std::vector<std::vector<std::string>> DataManager::loadAttendanceMonitoring(Event& event) {
	std::vector<Attendance> allAttendances = getAllActiveAttendances();
	std::vector<std::vector<std::string>> attendanceData;

	int attendanceNumber = 1;
	
	for(Attendance att : allAttendances){
		if(getRegistrationFromRegistrationId((att.registrationId)).eventId != event.eventId){
			continue;
		}
		
		std::vector<std::string> attendanceRow;
		
		// Number
		attendanceRow.push_back(std::to_string(attendanceNumber++));

		// Attendance Id
		attendanceRow.push_back(att.attendanceId);

		// Customer Name
		attendanceRow.push_back(getUserFromUserId(getRegistrationFromRegistrationId(att.registrationId).customerId).username);
	
		// Event Name
		attendanceRow.push_back(getEventFromEventId(getRegistrationFromRegistrationId(att.registrationId).eventId).eventName);

		// Status
		std::string status;
		switch (att.attendanceStatus) {
		case AttendanceStatus::PRESENT:
			status = "Present";
			break;
		case AttendanceStatus::ABSENT:
			status = "Absent";
			break;
		}
		attendanceRow.push_back(status);

		// Time
		attendanceRow.push_back(TimeManagement::formatTimeTToString(att.attendanceTime, "%Y-%m-%d %H:%M"));

		// Seat Id
		std::string seatIds;
		int count = 0;
		for (Seat seat : getRegistrationFromRegistrationId(att.registrationId).seats) {
			if (count != 0) {
				seatIds += ", ";
			}
			seatIds += seat.seatId;
			count++;
		}
		attendanceRow.push_back(seatIds);

		attendanceData.push_back(attendanceRow);
	}

	if (attendanceData.empty()) {
		attendanceData.push_back({ "No attendance records available", "", "", "", "", "", ""});
	}
	
	return attendanceData;
}

std::vector<std::vector<std::string>> DataManager::loadSeatMonitoring(Event& event) {
	std::vector<std::vector<std::string>> seatData;
	std::vector<Registration> allRegistrations = getAllActiveRegistrations();

	int seatNumber = 1;
	for (Registration& reg : allRegistrations) {
		if (reg.eventId != event.eventId || reg.registrationStatus == RegistrationStatus::CANCELLED) {
			continue;
		}

		for (Seat seat : reg.seats) {
			std::vector<std::string> seatRow;
			// Number
			seatRow.push_back(std::to_string(seatNumber));
			seatNumber++;

			// Seat Id
			seatRow.push_back(seat.seatId);

			// Row
			seatRow.push_back(seat.row);

			// Column
			seatRow.push_back(seat.column);

			// Customer
			seatRow.push_back(getUserFromUserId(reg.customerId).username);
			
			seatData.push_back(seatRow);
		}
	}

	if (seatData.empty()) {
		seatData.push_back({ "No seat records found for this events", "", "", "", "" });
	}
	return seatData;
}