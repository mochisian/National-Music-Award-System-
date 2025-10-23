#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>


class OutputManager {
public:
    enum class Color {
        BLACK = 30,
        RED = 31,
        GREEN = 32,
        YELLOW = 33,
        BLUE = 34,
        MAGENTA = 35,
        CYAN = 36,
        WHITE = 37,
        RESET = 39, 
        BRIGHT_BLACK = 90,
        BRIGHT_RED = 91,
        BRIGHT_GREEN = 92,
        BRIGHT_YELLOW = 93,
        BRIGHT_BLUE = 94,
        BRIGHT_MAGENTA = 95,
        BRIGHT_CYAN = 96,
        BRIGHT_WHITE = 97
    };

    void setColour(Color colour);
    void reset();
    void print(const std::string& str, Color colour = Color::RESET);
    void println(const std::string& str, Color colour = Color::RESET);
    void showMainMenu();
    void printHeader(const std::string& str, Color colour = Color::RESET);
    void printMenu(const std::vector<std::string> menu);
    void drawHorizontalLine(const std::vector<size_t>& columnWidths, unsigned char left, unsigned char mid, unsigned char right);
    void displayTable(const std::vector<std::string>& header, const std::vector<std::vector<std::string>>& rows);
    void printSeatingChart(const std::vector<std::vector<bool>>& seats);
    void printAttendanceSeat(const std::vector<std::vector<int>>& seats);
    void clearScreen();
};