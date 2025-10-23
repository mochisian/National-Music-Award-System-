#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <ctime>
#include "Model.h"
#include "OutputManager.h"
#include "DataManager.h"

class EventReporting {
public: 
	EventReporting();
	void run();

private: 
	DataManager dm;
	std::vector<Payment> payments;
	std::vector<Registration> registrations;
	std::vector<Attendance> attendances;
	std::vector<Vote> votes;
	std::vector<User> users;
	std::vector<Event> events;
	std::vector<Performance> performances;
	std::vector<Refund> refunds;

	void loadAllData();
	void displayReportMenu();
	int getUserChoice();
	void handleMenuChoice(int choice);
	void pauseScreen();

	// Main report functions
	void generateFinancialReport();
	void generateAttendanceReport();
	void generateVotingReport();

	// Financial report submenu functions
	void displayFinancialReportMenu();
	int getFinancialReportChoice();
	void handleFinancialReportChoice(int choice);
	void generateFinancialSummary();
	void generatePaymentMethodBreakdown();

	void displayAttendanceReportMenu();
	int getAttendanceReportChoice();
	void handleAttendanceReportChoice(int choice);
	void generateAttendanceSummary();
	void generateEventWiseAttendance();

	void displayVotingReportMenu();
	int getVotingReportChoice();
	void handleVotingReportChoice(int choice);
	void generateVotingSummary();
	void generatePerformanceWiseVoting();

	// Utility functions
	std::string timeToString(time_t timeValue);
	void printHeader(const std::string& title);
	void printSeparator(int width = 80);
	std::string getPaymentStatusString(PaymentStatus status);
	std::map<std::string, std::string> loadSongTitles();
	void printMiddle(int line_width, std::string title);

	// Data loading functions
	void loadPayments();
	void loadRegistrations();
	void loadAttendances();
	void loadVotes();
	void loadUsers();
	void loadEvents();
	void loadPerformances();
	void loadRefunds();
};