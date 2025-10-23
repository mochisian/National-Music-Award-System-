#pragma once
#ifndef VOTE_H
#define VOTE_H

#include "Model.h"
#include "DataManager.h"
#include "OutputManager.h"
#include "Validation.h"
#include "TimeUtils.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>

namespace Voting {

    const std::string SONG_FILE = "song.txt";
    const std::string EXPORT_FILE = "voting_results.txt";
    const int MIN_MENU_OPTION = 1;
    const int MAX_MENU_OPTION = 6;
    const int QUICK_VOTE_DISPLAY_COUNT = 5;
    const int VIEW_ALL_SONGS_OPTION = 6;
    const int BACK_OPTION = 6;

    const int RANK_WIDTH = 4;
    const int TITLE_WIDTH = 15;
    const int PERFORMER_WIDTH = 12;
    const int VOTES_WIDTH = 6;
    const int WEIGHT_WIDTH = 8;
    const int PERCENTAGE_WIDTH = 10;

    const int DEFAULT_VOTE_WEIGHT = 1;
    const int MAX_TOP_SONGS_DISPLAY = 10;
    const int ZERO_VOTES = 0;
    const double ZERO_PERCENTAGE = 0.0;

    const std::string ROCK_CATEGORY = "Rock/Alternative";
    const std::string POP_CATEGORY = "Pop/Ballad";
    const std::string JPOP_CATEGORY = "J-Pop/Anime";
    const std::string GENERAL_CATEGORY = "General";

    const std::string MAYDAY_ARTIST = "MAYDAY";
    const std::string JJ_LIN_ARTIST = "JJ Lin";
    const std::string YOASUBI_ARTIST = "YOASUBI";

    const std::string NO_REGISTRATIONS_MSG = "You don't have any event registrations.";
    const std::string NO_ELIGIBLE_REGISTRATIONS_MSG = "No eligible registrations found.";
    const std::string PAYMENT_NOT_COMPLETED_MSG = "Reason: Payment not completed or no valid registrations.";
    const std::string ALL_VOTES_CAST_MSG = "You have already cast all available votes for this registration.";
    const std::string NO_SONGS_AVAILABLE_MSG = "No songs available for voting.";
    const std::string VOTING_CANCELLED_MSG = "Voting cancelled.";
    const std::string VOTE_CAST_SUCCESS_MSG = "Vote cast successfully!";
    const std::string ALL_VOTES_COMPLETE_MSG = "All remaining votes cast successfully!";
    const std::string THANK_YOU_MSG = "Thank you for participating in the voting!";

    struct SongInfo {
        std::string songId;
        std::string title;
        std::string performer;
        int popularity = ZERO_VOTES;
    };

    struct VoteResult {
        std::string songId;
        std::string title;
        std::string performer;
        std::string category;
        int totalVotes = ZERO_VOTES;
        int totalVoteWeight = ZERO_VOTES;
        double percentage = ZERO_PERCENTAGE;
        int rank = ZERO_VOTES;
    };

    struct VotingSession {
        std::string sessionId;
        std::string eventId;
        time_t startTime;
        time_t endTime;
        bool isActive;
        std::string sessionType;
    };

    struct UserVoteStats {
        std::string userId;
        int totalVotes;
        int totalWeight;
        std::vector<std::string> votedSongs;
        time_t lastVoteTime;
    };

    class VoteSystem {
    public:
        VoteSystem(const std::string& userId, DataManager& dm);
        void run();
        void run(time_t currentDateTime);
        void runForSpecificEvent(const std::string& eventId, const std::string& registrationId, time_t currentDateTime);
        void castVotes(time_t currentDateTime);
        void viewVotingResults();
        void viewMyVotes();
        void voteAnalytics();
        void exportVotingResults();
        

    private:
        std::string userId;
        DataManager& dm;
        OutputManager output;
        Validation validation;
        bool isRealTimeActive = false;

        void showMainMenu(time_t currentDateTime);
        void showEventSpecificMenu(const std::string& eventId, const std::string& registrationId, time_t currentDateTime);
        void processVotingForSpecificRegistration(const std::string& registrationId, const std::string& eventId, time_t currentDateTime);

        std::vector<Registration> getUserRegistrations();
        std::vector<SongInfo> loadSongsFromFile();
        std::vector<SongInfo> loadSongsFromPerformances();
        std::vector<SongInfo> loadSongsFromPerformancesForEvent(const std::string& eventId);

        bool hasCompletedPayment(const std::string& registrationId);
        bool hasAlreadyVoted(const std::string& registrationId);
        bool isValidInteger(const std::string& str);
        bool canVoteForEvent(const std::string& eventId, time_t currentDateTime);
        bool isRegistrationRefunded(const std::string& registrationId);

        void displayRegistrations(const std::vector<Registration>& registrations);
        void displaySongs(const std::vector<SongInfo>& songs);
        void displayVoteResults(const std::vector<VoteResult>& results);
        void displayMyVotingHistory();

        void processVotingForRegistration(const Registration& registration, time_t currentDateTime);

        std::vector<VoteResult> calculateVoteResults();
        std::string generateVoteId();
        Event getEventById(const std::string& eventId);

        std::vector<std::pair<Event, int>> getEventsWithVotes();
        void viewEventSpecificResults(const Event& event);
        void viewCombinedResults();
        std::vector<VoteResult> calculateVoteResultsForEvent(const std::string& eventId);
        std::vector<Vote> getActiveVotesForUser();
    };

}
#endif
