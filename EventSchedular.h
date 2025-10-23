#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Model.h"
#include "DataManager.h"

class EventSchedular {
public:
	EventSchedular();
	EventSchedular(const User& user);
	void run();
	void runForCustomer(const std::string& eventId);

private:
	User currentUser;
	DataManager dm;
	std::vector<Performance> performances;
	std::vector<Event> events;
	std::string selectedEventId;
	int performanceCounter;
	int globalSongCounter;

	void displayMenu();
    void managePerformancesForEvent();
    void displayPerformanceManagementMenu();
    int getPerformanceMenuChoice();
    void handlePerformanceMenuChoice(int choice);
    void pauseScreen();

    // Performance management
    void addPerformance();
    void displayPerformance();
    void displayPerformanceDetails();
    void updatePerformance();
    void deletePerformance();

    // Update functions with event validation
    void updatePerformerName(Performance& performance);
    void updateStartTime(Performance& performance, const Event& event);
    void updateEndTime(Performance& performance, const Event& event);
    void updateSongs(Performance& performance);
    void updateAllFields(Performance& performance, const Event& event);

    // Delete functions
    void deleteSpecificPerformanceForEvent();
    void deleteAllPerformancesForEvent();

    // Utility functions
    std::string generatePerformanceId();
    time_t getValidatedTimeInputWithESC(const std::string& prompt, const Event& event, bool& wasCancelled);
    bool getInputWithESC(const std::string& prompt, std::string& input);
    bool addSongToPerformanceWithESC(Performance& performance);

    // File I/O
    void savePerformance(const Performance& performance);
    void loadPerformances();
    void savePerformancesToFile();
    void loadEvents();

    // Time utilities
    std::string timeToString(time_t timeValue);
    time_t stringToTime(const std::string& timeStr);

    // Display utilities
    void printMiddle(int line_width, std::string title);
    void displayCurrentEvents();
    bool selectEvent(const std::string& eventId);
    Event getEventById(const std::string& eventId);
    void displayPerformanceForEvent();

    // Song management
    void loadGlobalSongCounter();
    void saveGlobalSongCounter();
    std::string generateSongId();
    void saveSongToFile(const std::string& songId, const std::string& title, const std::string& performerName);
    void deleteSongsFromFile(const std::vector<Song>& songs);
    void deleteAllSongsFromFile();
};
