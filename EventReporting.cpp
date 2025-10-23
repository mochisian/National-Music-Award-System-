#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <map>
#include <tuple>
#include <conio.h>
#include <windows.h>
#include "EventReporting.h"
#include "OutputManager.h"
#include "Model.h"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

EventReporting::EventReporting() {
	loadAllData();
}

void EventReporting::run() {
	int choice;

	do {
		displayReportMenu();
		choice = getUserChoice();
		handleMenuChoice(choice);

		if (choice != 4) {
			pauseScreen();
		}

	} while (choice != 4);
}

void EventReporting::displayReportMenu() {
	system("cls");
	OutputManager output;
	printMiddle(30, "EVENT REPORTING SYSTEM");

	cout << "\nSelect Report Type: " << endl;
	cout << "1. Financial Reports" << endl;
	cout << "2. Attendance Report" << endl;
	cout << "3. Voting Report" << endl;
	cout << "4. Exit" << endl;
	cout << "Enter your choice (1-4): ";
}

int EventReporting::getUserChoice() {
	OutputManager output;
	string choice;
	getline(cin, choice);

	while (true) {
		if (choice.empty()) {
			output.print("Invalid input! Please enter 1-4. ", OutputManager::Color::RED);
			cout << "\n\nPress any key to continue...";
			_getch();
			displayReportMenu();
			getline(cin, choice);
			continue;
		}

		bool isValid = true;
		for (char c : choice) {
			if (!isdigit(c)) {
				isValid = false;
				break;
			}
		}

		if (!isValid) {
			output.print("Invalid input! Please enter 1-4. ", OutputManager::Color::RED);
			cout << "\n\nPress any key to continue...";
			_getch();
			displayReportMenu();
			getline(cin, choice);
			continue;
		}

		int numChoice = stoi(choice);

		if (numChoice >= 1 && numChoice <= 4) {
			return numChoice;
		}

		output.print("Invalid input! Please enter 1-4. ", OutputManager::Color::RED);
		cout << "\n\nPress any key to continue...";
		_getch();
		displayReportMenu();
		getline(cin, choice);
	}
}

void EventReporting::handleMenuChoice(int choice) {
	switch (choice) {
	case 1:
		generateFinancialReport();
		break;
	case 2:
		generateAttendanceReport();
		break;
	case 3:
		generateVotingReport();
		break;
	case 4:
		cout << "Exiting Event Reporting System..." << endl;
		break;
	default:
		cout << "Invalid choice! Please try again." << endl;
	}
}

void EventReporting::pauseScreen() {
	cout << "\nPress any key to continue...";
	_getch();
}

//--------------------------------LOAD DATA FUNCTION---------------------------------

void EventReporting::loadAllData() {
	loadPayments();
	loadRegistrations();
	loadAttendances();
	loadVotes();
	loadUsers();
	loadEvents();
	loadPerformances();
	loadRefunds();
}

void EventReporting::loadPayments() {
	payments = dm.getAllActivePayments();
}

void EventReporting::loadRegistrations() {
	registrations = dm.getAllActiveRegistrations();
}

void EventReporting::loadAttendances() {
	attendances = dm.getAllActiveAttendances();
}

void EventReporting::loadVotes() {
	votes = dm.getAllActiveVotes();
}

void EventReporting::loadUsers() {
	users = dm.getAllActiveUsers();
}

void EventReporting::loadEvents() {
	events = dm.getAllActiveEvents();
}

void EventReporting::loadPerformances() {
	performances = dm.getAllActivePerformances();
}

void EventReporting::loadRefunds() {
	refunds = dm.getAllActiveRefunds();
}

//--------------------------------Generate Report Function---------------------------------

void EventReporting::generateFinancialReport() {
	int choice;

	do {
		displayFinancialReportMenu();
		choice = getFinancialReportChoice();
		handleFinancialReportChoice(choice);

		if (choice != 3) {
			pauseScreen();
		}

	} while (choice != 3);
}

void EventReporting::displayFinancialReportMenu() {
	system("cls");
	OutputManager output;
	printMiddle(90, "FINANCIAL REPORT");

	// Use displayTable instead of manual formatting
	std::vector<std::string> headers = { "Payment ID", "Registration ID", "Amount", "Payment Date", "Method", "Status" };
	std::vector<std::vector<std::string>> paymentData;

	if (payments.empty()) {
		output.println("No payment data available. ", OutputManager::Color::YELLOW);
	}
	else {
		for (const auto& payment : payments) {
			std::vector<std::string> row;
			
			// Create formatted amount string with dollar sign
			ostringstream amountStr;
			amountStr << "$" << fixed << setprecision(2) << payment.amount;
			
			string method;
			switch (payment.paymentMethod) {
			case PaymentMethod::CARD_PAYMENT: method = "CARD"; break;
			case PaymentMethod::E_WALLET: method = "E-WALLET"; break;
			case PaymentMethod::ONLINE_BANKING: method = "BANKING"; break;
			}
			
			string status;
			switch (payment.paymentStatus) {
			case PaymentStatus::COMPLETED: status = "COMPLETED"; break;
			case PaymentStatus::PENDING: status = "PENDING"; break;
			case PaymentStatus::FAILED: status = "FAILED"; break;
			case PaymentStatus::REFUNDED: status = "REFUNDED"; break;
			}
			
			row.push_back(payment.paymentId);
			row.push_back(payment.registrationId);
			row.push_back(amountStr.str());
			row.push_back(timeToString(payment.paymentDate));
			row.push_back(method);
			row.push_back(status);
			
			paymentData.push_back(row);
		}
	}
	
	output.displayTable(headers, paymentData);

	cout << "\nSelect Financial Report Type: " << endl;
	cout << "1. Financial Summary" << endl;
	cout << "2. Payment Method Breakdown" << endl;
	cout << "3. Back to Main Menu" << endl;
	cout << "Enter your choice (1-3): ";
}

int EventReporting::getFinancialReportChoice() {
	OutputManager output;
	string choice;
	getline(cin, choice);

	while (true) {
		if (choice.empty()) {
			output.print("Invalid input! Please enter 1-3. ", OutputManager::Color::RED);
			cout << "\n\nPress any key to continue...";
			_getch();
			displayFinancialReportMenu();
			getline(cin, choice);
			continue;
		}

		bool isValid = true;
		for (char c : choice) {
			if (!isdigit(c)) {
				isValid = false;
				break;
			}
		}

		if (!isValid) {
			output.print("Invalid input! Please enter 1-3. ", OutputManager::Color::RED);
			cout << "\n\nPress any key to continue...";
			_getch();
			displayFinancialReportMenu();
			getline(cin, choice);
			continue;
		}

		int numChoice = stoi(choice);

		if (numChoice >= 1 && numChoice <= 3) {
			return numChoice;
		}

		output.print("Invalid input! Please enter 1-3. ", OutputManager::Color::RED);
		cout << "\n\nPress any key to continue...";
		_getch();
		displayFinancialReportMenu();
		getline(cin, choice);
	}
}

void EventReporting::handleFinancialReportChoice(int choice) {
	switch (choice) {
	case 1:
		generateFinancialSummary();
		break;
	case 2:
		generatePaymentMethodBreakdown();
		break;
	case 3:
		cout << "Returning to main menu..." << endl;
		break;
	default:
		cout << "Invalid choice! Please try again." << endl;
	}
}

void EventReporting::generateFinancialSummary() {
	system("cls");
	OutputManager output;
	printMiddle(45, "FINANCIAL SUMMARY REPORT");

	double totalRevenue = 0.0;
	double completedAmount = 0.0;
	double pendingAmount = 0.0;
	double failedAmount = 0.0;
	double refundedAmount = 0.0;

	int totalTransactions = static_cast<int>(payments.size());
	int completedCount = 0;
	int pendingCount = 0;
	int failedCount = 0;
	int refundedCount = 0;

	double totalOriginalRevenue = 0.0;

	// Calculate financial metrics from payments
	for (const auto& payment : payments) {
		switch (payment.paymentStatus) {
		case PaymentStatus::COMPLETED:
			completedAmount += payment.amount;
			completedCount++;
			totalRevenue += payment.amount;
			totalOriginalRevenue += payment.amount;
			break;
		case PaymentStatus::PENDING:
			pendingAmount += payment.amount;
			pendingCount++;
			break;
		case PaymentStatus::FAILED:
			failedAmount += payment.amount;
			failedCount++;
			break;
		case PaymentStatus::REFUNDED:
			refundedAmount += payment.amount;
			refundedCount++;
			totalOriginalRevenue += payment.amount;
			break;
		}
	}

	double totalRefundsPaidOut = 0.0;
	for (const auto& refund : refunds) {
		totalRefundsPaidOut += refund.refundAmount;
	}

	double netRevenue = totalOriginalRevenue - totalRefundsPaidOut;

	double processingFeesEarned = refundedAmount - totalRefundsPaidOut;

	if (netRevenue < 0) {
		netRevenue = 0;
	}

	cout << left << setw(25) << "Total Transactions: " << totalTransactions << endl;
	cout << left << setw(25) << "Completed Revenue: " << "RM " << fixed << setprecision(2) << totalRevenue << endl;
	cout << left << setw(25) << "Refunded Revenue: " << "RM " << fixed << setprecision(2) << refundedAmount << endl;
	cout << left << setw(25) << "Gross Revenue: " << "RM " << fixed << setprecision(2) << totalOriginalRevenue << endl;
	cout << left << setw(25) << "Refunds Paid Out: " << "RM " << fixed << setprecision(2) << totalRefundsPaidOut << endl;
	cout << left << setw(25) << "Processing Fees: " << "RM " << fixed << setprecision(2) << processingFeesEarned << endl;
	cout << left << setw(25) << "Net Revenue: " << "RM " << fixed << setprecision(2) << netRevenue << endl;
	cout << endl;
	cout << left << setw(25) << "Completed Payments: " << completedCount
		<< " (RM" << fixed << setprecision(2) << completedAmount << ")" << endl;
	cout << left << setw(25) << "Pending Payments: " << pendingCount
		<< " (RM" << fixed << setprecision(2) << pendingAmount << ")" << endl;
	cout << left << setw(25) << "Failed Payments: " << failedCount
		<< " (RM" << fixed << setprecision(2) << failedAmount << ")" << endl;
	cout << left << setw(25) << "Refunded Payments: " << refundedCount
		<< " (RM" << fixed << setprecision(2) << refundedAmount << ")" << endl;

	if (totalTransactions > 0) {
		double successRate = (double)(completedCount + refundedCount) / totalTransactions * 100;
		cout << left << setw(25) << "Success Rate: " << fixed << setprecision(2) << successRate << "%" << endl;
	}
}

void EventReporting::generatePaymentMethodBreakdown() {
	system("cls");
	OutputManager output;
	printMiddle(65, "PAYMENT METHOD BREAKDOWN");

	// Use displayTable instead of manual formatting
	std::vector<std::string> headers = { "Payment Method", "Count", "Total Amount", "Percentage" };
	std::vector<std::vector<std::string>> methodData;

	map<PaymentMethod, pair<int, double>> methodStats;
	for (const auto& payment : payments) {
		if (payment.paymentStatus == PaymentStatus::COMPLETED) {
			methodStats[payment.paymentMethod].first++;
			methodStats[payment.paymentMethod].second += payment.amount;
		}
	}

	if (methodStats.empty()) {
		output.println("No completed payment data available for breakdown.", OutputManager::Color::YELLOW);
		return;
	}

	double totalAmount = 0.0;
	int totalCount = 0;

	// Calculate totals first
	for (const auto& stat : methodStats) {
		totalAmount += stat.second.second;
		totalCount += stat.second.first;
	}

	// Display breakdown using displayTable
	for (const auto& stat : methodStats) {
		std::vector<std::string> row;
		
		string method;
		switch (stat.first) {
		case PaymentMethod::CARD_PAYMENT: method = "Card Payment"; break;
		case PaymentMethod::E_WALLET: method = "E-Wallet"; break;
		case PaymentMethod::ONLINE_BANKING: method = "Online Banking"; break;
		}

		double percentage = totalAmount > 0 ? (stat.second.second / totalAmount) * 100 : 0;

		stringstream amountStr;
		amountStr << "RM" << fixed << setprecision(2) << stat.second.second;

		stringstream percentageStr;
		percentageStr << fixed << setprecision(1) << percentage << "%";

		row.push_back(method);
		row.push_back(std::to_string(stat.second.first));
		row.push_back(amountStr.str());
		row.push_back(percentageStr.str());
		
		methodData.push_back(row);
	}
	
	output.displayTable(headers, methodData);

	cout << "\nPAYMENT METHOD SUMMARY:" << endl;
	printSeparator(65);
	cout << left << setw(25) << "Total Methods Used: " << methodStats.size() << endl;
	cout << left << setw(25) << "Total Transactions: " << totalCount << endl;
	cout << left << setw(25) << "Total Amount: " << "RM" << fixed << setprecision(2) << totalAmount << endl;

	// Show most popular method
	auto mostPopular = max_element(methodStats.begin(), methodStats.end(),
		[](const pair<PaymentMethod, pair<int, double>>& a, const pair<PaymentMethod, pair<int, double>>& b) {
			return a.second.first < b.second.first;
		});

	if (mostPopular != methodStats.end()) {
		string popularMethod;
		switch (mostPopular->first) {
		case PaymentMethod::CARD_PAYMENT: popularMethod = "Card Payment"; break;
		case PaymentMethod::E_WALLET: popularMethod = "E-Wallet"; break;
		case PaymentMethod::ONLINE_BANKING: popularMethod = "Online Banking"; break;
		}
		cout << left << setw(25) << "Most Popular Method: " << popularMethod
			<< " (" << mostPopular->second.first << " transactions)" << endl;
	}
}

//--------------------------------Attendance Report Function---------------------------------

void EventReporting::generateAttendanceReport() {
	int choice;

	do {
		displayAttendanceReportMenu();
		choice = getAttendanceReportChoice();
		handleAttendanceReportChoice(choice);

		if (choice != 3) {
			pauseScreen();
		}

	} while (choice != 3);
}

void EventReporting::displayAttendanceReportMenu() {
	system("cls");
	OutputManager output;
	printMiddle(90, "ATTENDANCE REPORT");

	// Use displayTable instead of manual formatting
	std::vector<std::string> headers = { "Registration ID", "Customer ID", "Event ID", "Seats", "Attendance", "Status" };
	std::vector<std::vector<std::string>> attendanceData;

	if (registrations.empty()) {
		output.println("No registration data available.", OutputManager::Color::YELLOW);
	}
	else {
		for (const auto& registration : registrations) {
			std::vector<std::string> row;
			
			AttendanceStatus status = AttendanceStatus::ABSENT;
			bool hasAttendance = false;

			for (const auto& attendance : attendances) {
				if (attendance.registrationId == registration.registrationId) {
					status = attendance.attendanceStatus;
					hasAttendance = true;
					break;
				}
			}

			string attendanceStr;
			if (hasAttendance) {
				if (status == AttendanceStatus::PRESENT) {
					attendanceStr = "PRESENT";
				}
				else {
					attendanceStr = "ABSENT";
				}
			}
			else {
				attendanceStr = "NO RECORD";
			}
			
			string regStatus = (registration.registrationStatus == RegistrationStatus::COMPLETED ? "COMPLETED" : "ONGOING");
			
			row.push_back(registration.registrationId);
			row.push_back(registration.customerId);
			row.push_back(registration.eventId);
			row.push_back(std::to_string(registration.seats.size()));
			row.push_back(attendanceStr);
			row.push_back(regStatus);
			
			attendanceData.push_back(row);
		}
	}
	
	output.displayTable(headers, attendanceData);

	cout << "\nSelect Attendance Report Type: " << endl;
	cout << "1. Attendance Summary" << endl;
	cout << "2. Event-wise Attendance" << endl;
	cout << "3. Back to Main Menu" << endl;
	cout << "Enter your choice (1-3): ";
}

int EventReporting::getAttendanceReportChoice() {
	OutputManager output;
	string choice;
	getline(cin, choice);

	while (true) {
		if (choice.empty()) {
			output.print("Invalid input. Please enter 1-3! ", OutputManager::Color::RED);
			cout << "\n\nPress any key to continue...";
			_getch();
			displayAttendanceReportMenu();
			getline(cin, choice);
			continue;
		}

		bool isValid = true;
		for (char c : choice) {
			if (!isdigit(c)) {
				isValid = false;
				break;
			}
		}

		if (!isValid) {
			output.print("Invalid input. Please enter 1-3! ", OutputManager::Color::RED);
			cout << "\n\nPress any key to continue...";
			_getch();
			displayAttendanceReportMenu();
			getline(cin, choice);
			continue;
		}

		int numChoice = stoi(choice);

		if (numChoice >= 1 && numChoice <= 3) {
			return numChoice;
		}

		output.print("Invalid input. Please enter 1-3!", OutputManager::Color::RED);
		cout << "\n\nPress any key to continue...";
		_getch();
		displayAttendanceReportMenu();
		getline(cin, choice);
	}
}

void EventReporting::handleAttendanceReportChoice(int choice) {
	switch (choice) {
	case 1:
		generateAttendanceSummary();
		break;
	case 2:
		generateEventWiseAttendance();
		break;
	case 3:
		cout << "Returning to main menu..." << endl;
		break;
	default:
		cout << "Invalid choice! Please try again." << endl;
	}
}

void EventReporting::generateAttendanceSummary() {
	system("cls");
	OutputManager output;
	printMiddle(45, "ATTENDANCE SUMMARY REPORT");

	int totalRegistrations = static_cast<int>(registrations.size());
	int presentCount = 0;
	int absentCount = 0;
	int noRecordCount = 0;

	for (const auto& registration : registrations) {
		bool hasAttendance = false;

		for (const auto& attendance : attendances) {
			if (attendance.registrationId == registration.registrationId) {
				if (attendance.attendanceStatus == AttendanceStatus::PRESENT) {
					presentCount++;
				}
				else {
					absentCount++;
				}
				hasAttendance = true;
				break;
			}
		}

		if (!hasAttendance) {
			noRecordCount++;
		}
	}

	cout << left << setw(25) << "Total Registrations: " << totalRegistrations << endl;
	cout << left << setw(25) << "Present: " << presentCount << endl;
	cout << left << setw(25) << "Absent: " << absentCount << endl;
	cout << left << setw(25) << "No Record: " << noRecordCount << endl;

	if (totalRegistrations > 0) {
		double attendanceRate = (double)presentCount / totalRegistrations * 100;
		double absentRate = (double)absentCount / totalRegistrations * 100;
		double noRecordRate = (double)noRecordCount / totalRegistrations * 100;

		cout << endl;
		cout << left << setw(25) << "Attendance Rate: " << fixed << setprecision(1) << attendanceRate << "%" << endl;
		cout << left << setw(25) << "Absent Rate: " << fixed << setprecision(1) << absentRate << "%" << endl;
		cout << left << setw(25) << "No Record Rate: " << fixed << setprecision(1) << noRecordRate << "%" << endl;
	}
}

void EventReporting::generateEventWiseAttendance() {
	system("cls");
	OutputManager output;
	printMiddle(90, "EVENT-WISE ATTENDANCE REPORT");

	// Use displayTable instead of manual formatting
	std::vector<std::string> headers = { "Event ID", "Event Name", "Registered", "Present", "Absent", "Rate" };
	std::vector<std::vector<std::string>> eventAttendanceData;

	map<string, tuple<int, int, string>> eventStats;

	for (const auto& registration : registrations) {
		get<0>(eventStats[registration.eventId])++;

		for (const auto& event : events) {
			if (event.eventId == registration.eventId) {
				get<2>(eventStats[registration.eventId]) = event.eventName;
				break;
			}
		}

		for (const auto& attendance : attendances) {
			if (attendance.registrationId == registration.registrationId &&
				attendance.attendanceStatus == AttendanceStatus::PRESENT) {
				get<1>(eventStats[registration.eventId])++;
				break;
			}
		}
	}

	if (eventStats.empty()) {
		output.println("No event data available for attendance analysis.", OutputManager::Color::YELLOW);
		return;
	}

	for (const auto& stat : eventStats) {
		std::vector<std::string> row;
		
		int total = get<0>(stat.second);
		int present = get<1>(stat.second);
		string eventName = get<2>(stat.second);
		int absent = total - present;
		double rate = total > 0 ? (double)present / total * 100 : 0;

		if (eventName.length() > 19) {
			eventName = eventName.substr(0, 16) + "...";
		}

		stringstream rateStr;
		rateStr << fixed << setprecision(1) << rate << "%";
		
		row.push_back(stat.first);
		row.push_back(eventName);
		row.push_back(std::to_string(total));
		row.push_back(std::to_string(present));
		row.push_back(std::to_string(absent));
		row.push_back(rateStr.str());
		
		eventAttendanceData.push_back(row);
	}
	
	output.displayTable(headers, eventAttendanceData);
}

//--------------------------------Voting Report Function---------------------------------

void EventReporting::generateVotingReport() {
	int choice;

	do {
		displayVotingReportMenu();
		choice = getVotingReportChoice();
		handleVotingReportChoice(choice);

		if (choice != 3) {
			pauseScreen();
		}

	} while (choice != 3);
}

void EventReporting::displayVotingReportMenu() {
	system("cls");
	OutputManager output;
	printHeader("VOTING REPORT");

	// Use displayTable instead of manual formatting
	std::vector<std::string> headers = { "Rank", "Song ID", "Song Title", "Performer", "Votes", "Percentage" };
	std::vector<std::vector<std::string>> votingData;

	map<string, int> songVotes;
	map<string, string> songTitles = loadSongTitles();
	map<string, string> songPerformers;

	for (const auto& performance : performances) {
		for (const auto& song : performance.songs) {
			songVotes[song.songId] = 0;
			songTitles[song.songId] = song.title;
			songPerformers[song.songId] = performance.performerName;
		}
	}

	int totalVotes = 0;
	int totalVoters = 0;
	map<string, int> voterCount;

	for (const auto& vote : votes) {
		songVotes[vote.songId] += vote.voteWeight;
		totalVotes += vote.voteWeight;
		voterCount[vote.registrationId] = 1;
	}
	totalVoters = static_cast<int>(voterCount.size());

	vector<tuple<string, int, string, string>> sortedVotes;
	for (const auto& sv : songVotes) {
		if (sv.second > 0) {
			sortedVotes.push_back(make_tuple(sv.first, sv.second,
				songTitles[sv.first], songPerformers[sv.first]));
		}
	}

	sort(sortedVotes.begin(), sortedVotes.end(),
		[](const tuple<string, int, string, string>& a, const tuple<string, int, string, string>& b) -> bool {
			return get<1>(a) > get<1>(b);
		});

	if (sortedVotes.empty()) {
		output.println("No voting data available. ", OutputManager::Color::YELLOW);
	}
	else {
		for (size_t i = 0; i < sortedVotes.size(); i++) {
			std::vector<std::string> row;
			
			string songId = get<0>(sortedVotes[i]);
			int voteCount = get<1>(sortedVotes[i]);
			string title = get<2>(sortedVotes[i]);
			string performer = get<3>(sortedVotes[i]);
			double percentage = totalVotes > 0 ? (double)voteCount / totalVotes * 100 : 0;

			if (title.length() > 24) {
				title = title.substr(0, 21) + "...";
			}
			if (performer.length() > 19) {
				performer = performer.substr(0, 16) + "...";
			}

			stringstream percentageStr;
			percentageStr << fixed << setprecision(1) << percentage << "%";

			row.push_back(std::to_string(i + 1));
			row.push_back(songId);
			row.push_back(title);
			row.push_back(performer);
			row.push_back(std::to_string(voteCount));
			row.push_back(percentageStr.str());
			
			votingData.push_back(row);
		}
	}
	
	output.displayTable(headers, votingData);

	cout << "\nSelect Voting Report Type: " << endl;
	cout << "1. Voting Summary" << endl;
	cout << "2. Performance-wise Voting" << endl;
	cout << "3. Back to Main Menu" << endl;
	cout << "Enter your choice (1-3): ";
}

int EventReporting::getVotingReportChoice() {
	OutputManager output;
	string choice;
	getline(cin, choice);

	while (true) {
		if (choice.empty()) {
			output.print("Invalid input. Please enter 1-3! ", OutputManager::Color::RED);
			cout << "\n\nPress any key to continue...";
			_getch();
			displayVotingReportMenu();
			getline(cin, choice);
			continue;
		}

		bool isValid = true;
		for (char c : choice) {
			if (!isdigit(c)) {
				isValid = false;
				break;
			}
		}

		if (!isValid) {
			output.print("Invalid input. Please enter 1-3! ", OutputManager::Color::RED);
			cout << "\n\nPress any key to continue...";
			_getch();
			displayVotingReportMenu();
			getline(cin, choice);
			continue;
		}

		int numChoice = stoi(choice);

		if (numChoice >= 1 && numChoice <= 3) {
			return numChoice;
		}

		output.print("Invalid input. Please enter 1-3!", OutputManager::Color::RED);
		cout << "\n\nPress any key to continue...";
		_getch();
		displayVotingReportMenu();
		getline(cin, choice);
	}
}

void EventReporting::handleVotingReportChoice(int choice) {
	switch (choice) {
	case 1:
		generateVotingSummary();
		break;
	case 2:
		generatePerformanceWiseVoting();
		break;
	case 3:
		cout << "Returning to main menu..." << endl;
		break;
	default:
		cout << "Invalid choice! Please try again." << endl;
	}
}

void EventReporting::generateVotingSummary() {
	system("cls");
	printMiddle(45, "VOTING SUMMARY REPORT");

	map<string, int> songVotes;
	map<string, string> songTitles = loadSongTitles();
	map<string, string> songPerformers;

	for (const auto& performance : performances) {
		for (const auto& song : performance.songs) {
			songVotes[song.songId] = 0;
			songTitles[song.songId] = song.title;
			songPerformers[song.songId] = performance.performerName;
		}
	}

	int totalVotes = 0;
	int totalVoters = 0;
	map<string, int> voterCount;

	for (const auto& vote : votes) {
		songVotes[vote.songId] += vote.voteWeight;
		totalVotes += vote.voteWeight;
		voterCount[vote.registrationId] = 1;
	}
	totalVoters = static_cast<int>(voterCount.size());

	vector<tuple<string, int, string, string>> sortedVotes;
	for (const auto& sv : songVotes) {
		if (sv.second > 0) {
			sortedVotes.push_back(make_tuple(sv.first, sv.second,
				songTitles[sv.first], songPerformers[sv.first]));
		}
	}

	sort(sortedVotes.begin(), sortedVotes.end(),
		[](const tuple<string, int, string, string>& a, const tuple<string, int, string, string>& b) -> bool {
			return get<1>(a) > get<1>(b);
		});

	cout << left << setw(25) << "Total Votes Cast: " << totalVotes << endl;
	cout << left << setw(25) << "Total Voters: " << totalVoters << endl;
	cout << left << setw(25) << "Songs with Votes: " << sortedVotes.size() << endl;
	cout << left << setw(25) << "Total Songs: " << songTitles.size() << endl;

	if (!sortedVotes.empty()) {
		cout << left << setw(25) << "Winning Song: " << get<2>(sortedVotes[0])
			<< " (" << get<1>(sortedVotes[0]) << " votes)" << endl;
		cout << left << setw(25) << "Winning Performer: " << get<3>(sortedVotes[0]) << endl;
	}

	if (totalVoters > 0) {
		double avgVotesPerVoter = (double)totalVotes / totalVoters;
		cout << left << setw(25) << "Avg Votes per Voter: " << fixed << setprecision(1) << avgVotesPerVoter << endl;
	}
}

void EventReporting::generatePerformanceWiseVoting() {
	system("cls");
	OutputManager output;
	printMiddle(45, "PERFORMANCE-WISE VOTING BREAKDOWN");

	map<string, int> songVotes;
	map<string, string> songTitles = loadSongTitles();

	for (const auto& performance : performances) {
		for (const auto& song : performance.songs) {
			songVotes[song.songId] = 0;
			songTitles[song.songId] = song.title;
		}
	}

	int totalVotes = 0;
	for (const auto& vote : votes) {
		songVotes[vote.songId] += vote.voteWeight;
		totalVotes += vote.voteWeight;
	}

	for (const auto& performance : performances) {
		cout << "\nPerformance: " << performance.performerName
			<< " (ID: " << performance.performanceId << ")" << endl;
		cout << "Event:  " << performance.eventId << endl;

		int perfTotalVotes = 0;
		bool hasVotes = false;

		for (const auto& song : performance.songs) {
			int songVoteCount = songVotes[song.songId];
			perfTotalVotes += songVoteCount;

			if (songVoteCount > 0) {
				hasVotes = true;
				double songPercentage = totalVotes > 0 ? (double)songVoteCount / totalVotes * 100 : 0;
				cout << " " << song.title << ": " << songVoteCount
					<< " votes (" << fixed << setprecision(1) << songPercentage << "%)" << endl;
			}
		}

		if (!hasVotes) {
			output.println("No votes received for this performance.\n", OutputManager::Color::YELLOW);
		}
		else {
			double perfPercentage = totalVotes > 0 ? (double)perfTotalVotes / totalVotes * 100 : 0;
			cout << " Performance Total: " << perfTotalVotes
				<< " votes (" << fixed << setprecision(1) << perfPercentage << "%)\n" << endl;
		}

		printSeparator(45);
	}
}

string EventReporting::timeToString(time_t timeValue) {
	if (timeValue == 0) return "N/A";

	struct tm* timeinfo = localtime(&timeValue);
	char buffer[20];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", timeinfo);
	return string(buffer);
}

void EventReporting::printHeader(const string& title) {
	int width = 80;
	cout << string(width, '=') << endl;
	int padding = (width - static_cast<int>(title.length())) / 2;
	cout << string(padding, ' ') << title << endl;
	cout << string(width, '=') << endl;
}

void EventReporting::printSeparator(int width) {
	cout << string(width, '-') << endl;
}

string EventReporting::getPaymentStatusString(PaymentStatus status) {
	switch (status) {
	case PaymentStatus::COMPLETED: return "COMPLETED";
	case PaymentStatus::PENDING: return "PENDING";
	case PaymentStatus::FAILED:  return "FAILED";
	case PaymentStatus::REFUNDED: return "REFUNDS";
	default: return "UNKNOWN";
	}
}

map<string, string> EventReporting::loadSongTitles() {
	map<string, string> songTitles;

	ifstream songFile("song.txt");
	if (songFile.is_open()) {
		string line;
		while (getline(songFile, line)) {
			size_t pos1 = line.find('|');
			size_t pos2 = line.find('|', pos1 + 1);
			if (pos1 != string::npos && pos2 != string::npos) {
				string songId = line.substr(0, pos1);
				string title = line.substr(pos1 + 1, pos2 - pos1 - 1);
				songTitles[songId] = title;
			}
		}
		songFile.close();
	}

	return songTitles;
}

void EventReporting::printMiddle(int line_width, std::string title) {
	std::cout << std::string(line_width, '=') << std::endl;
	int padding = (line_width - static_cast<int>(title.length())) / 2;
	std::cout << std::string(padding, ' ') << title << std::endl;
	std::cout << std::string(line_width, '=') << std::endl;
}