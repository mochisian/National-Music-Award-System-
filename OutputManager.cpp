#include <string>
#include <iomanip>
#include <iostream>
#include "OutputManager.h"

void OutputManager::setColour(Color colour) {
    std::cout << "\033[" << static_cast<int>(colour) << "m";
}

void OutputManager::reset() {
	std::cout << "\033[" << static_cast<int>(OutputManager::Color::RESET) << "m";
}
 
void OutputManager::print(const std::string& str, Color colour) {
	setColour(colour);
	std::cout << str;
	reset();
}

void OutputManager::println(const std::string& str, Color colour) {
	print(str, colour);
	std::cout << std::endl;
}

void OutputManager::showMainMenu() {
        clearScreen();

    println(R"(
     /$$   /$$  /$$$$$$  /$$$$$$$$ /$$$$$$  /$$$$$$  /$$   /$$  /$$$$$$  /$$      
    | $$$ | $$ /$$__  $$|__  $$__/|_  $$_/ /$$__  $$| $$$ | $$ /$$__  $$| $$      
    | $$$$| $$| $$  \ $$   | $$     | $$  | $$  \ $$| $$$$| $$| $$  \ $$| $$      
    | $$ $$ $$| $$$$$$$$   | $$     | $$  | $$  | $$| $$ $$ $$| $$$$$$$$| $$      
    | $$  $$$$| $$__  $$   | $$     | $$  | $$  | $$| $$  $$$$| $$__  $$| $$      
    | $$\  $$$| $$  | $$   | $$     | $$  | $$  | $$| $$\  $$$| $$  | $$| $$      
    | $$ \  $$| $$  | $$   | $$    /$$$$$$|  $$$$$$/| $$ \  $$| $$  | $$| $$$$$$$$
    |__/  \__/|__/  |__/   |__/   |______/ \______/ |__/  \__/|__/  |__/|________/

     /$$      /$$ /$$   /$$  /$$$$$$  /$$$$$$  /$$$$$$         /$$$$$$  /$$      /$$  /$$$$$$  /$$$$$$$  /$$$$$$$ 
    | $$$    /$$$| $$  | $$ /$$__  $$|_  $$_/ /$$__  $$       /$$__  $$| $$  /$ | $$ /$$__  $$| $$__  $$| $$__  $$
    | $$$$  /$$$$| $$  | $$| $$  \__/  | $$  | $$  \__/      | $$  \ $$| $$ /$$$| $$| $$  \ $$| $$  \ $$| $$  \ $$
    | $$ $$/$$ $$| $$  | $$|  $$$$$$   | $$  | $$            | $$$$$$$$| $$/$$ $$ $$| $$$$$$$$| $$$$$$$/| $$  | $$
    | $$  $$$| $$| $$  | $$ \____  $$  | $$  | $$            | $$__  $$| $$$$_  $$$$| $$__  $$| $$__  $$| $$  | $$
    | $$\  $ | $$| $$  | $$ /$$  \ $$  | $$  | $$    $$      | $$  | $$| $$$/ \  $$$| $$  | $$| $$  \ $$| $$  | $$
    | $$ \/  | $$|  $$$$$$/|  $$$$$$/ /$$$$$$|  $$$$$$/      | $$  | $$| $$/   \  $$| $$  | $$| $$  | $$| $$$$$$$/
    |__/     |__/ \______/  \______/ |______/ \______/       |__/  |__/|__/     \__/|__/  |__/|__/  |__/|_______/ )", Color::CYAN);

        println("\n\n");
}

void OutputManager::printHeader(const std::string& str, Color colour) {
    for (int i = 0; i < str.size() + 10; i++) {
        print("=", colour);
    }

    std::cout << std::endl;
    println(std::string("     ") + str, colour);

    for (int i = 0; i < str.size() + 10; i++) {
        print("=", colour);
    }
    std::cout << std::endl << std::endl;
}

void OutputManager::printMenu(const std::vector<std::string> menu) {
    int count = 1;
    for (int i = 0; i < menu.size(); i += 2) {
        std::cout << " [" << count << "] " << menu[i] << ": " << menu[i + 1] << std::endl;
        count++;
    }
    std::cout << std::endl;
}

void OutputManager::drawHorizontalLine(const std::vector<size_t>& columnWidths, unsigned char left, unsigned char mid, unsigned char right) {
    const unsigned char horizontal = 205;

    std::cout << left;
    for (size_t i = 0; i < columnWidths.size(); ++i) {
        std::cout << std::string(columnWidths[i], horizontal);
        if (i < columnWidths.size() - 1) {
            std::cout << mid;
        }
    }
    std::cout << right << "\n";
}

void OutputManager::displayTable(const std::vector<std::string>& header, const std::vector<std::vector<std::string>>& rows) {
    const unsigned char connerLU = 201; 
    const unsigned char connerRU = 187; 
    const unsigned char connerLB = 200; 
    const unsigned char connerRB = 188; 
    const unsigned char straight = 186; 
    const unsigned char connectL = 204; 
    const unsigned char connectR = 185;
    const unsigned char connerMid = 203;
    const unsigned char BMid = 202;     
    const unsigned char Mid = 206;   

    const size_t padding = 2;
    size_t numColumns = header.size();
    std::vector<size_t> columnWidths(numColumns);

    for (size_t i = 0; i < numColumns; ++i) {
        columnWidths[i] = header[i].length();
    }

    for (const auto& row : rows) {
        if (row.size() != numColumns) {
            std::cerr << "Warning: Row size mismatch. Skipping row." << std::endl;
            continue;
        }
        for (size_t i = 0; i < numColumns; ++i) {
            if (row[i].length() > columnWidths[i]) {
                columnWidths[i] = row[i].length();
            }
        }
    }

    for (size_t& width : columnWidths) {
        width += padding;
    }

    drawHorizontalLine(columnWidths, connerLU, connerMid, connerRU);

    std::cout << straight;
    for (size_t i = 0; i < numColumns; ++i) {
        std::cout << " " << std::left << std::setw(columnWidths[i] - 1) << header[i] << straight;
    }
    std::cout << "\n";

    if (rows.empty()) {
        drawHorizontalLine(columnWidths, connerLB, BMid, connerRB);
    }
    else {
        drawHorizontalLine(columnWidths, connectL, Mid, connectR);
    }

    for (size_t i = 0; i < rows.size(); ++i) {
        const auto& row = rows[i];
        if (row.size() != numColumns) continue;

        std::cout << straight;
        for (size_t j = 0; j < numColumns; ++j) {
            std::cout << " " << std::left << std::setw(columnWidths[j] - 1) << row[j] << straight;
        }
        std::cout << "\n";

        if (i < rows.size() - 1) {
            drawHorizontalLine(columnWidths, connectL, Mid, connectR);
        }
    }

    if (!rows.empty()) {
        drawHorizontalLine(columnWidths, connerLB, BMid, connerRB);
    }
    std::cout << std::endl;
}

void OutputManager::printSeatingChart(const std::vector<std::vector<bool>>& seats) {
    print("      Seat\nRow   ", OutputManager::Color::CYAN);
    setColour(OutputManager::Color::CYAN);
    for (int i = 0; i < seats[0].size(); ++i) {
        std::cout << std::setw(2) << i + 1;
    }
    std::cout << "\n--------------------------\n";
    reset();

    for (int i = 0; i < seats.size(); ++i) {
        setColour(OutputManager::Color::CYAN);
        std::cout << std::setw(3) << i + 1 << " | ";
        reset();

        setColour(OutputManager::Color::CYAN);
        for (int j = 0; j < seats[i].size(); ++j) {
            if (seats[i][j]) {
                print(" X", OutputManager::Color::RED);
            }
            else {
                print(" O", OutputManager::Color::GREEN);
            }
        }
        std::cout << std::endl;
    }

    print("\nLegend: ");
    print("O = Available, ", OutputManager::Color::GREEN);
    println("X = Occupied", OutputManager::Color::RED);
}

void OutputManager::printAttendanceSeat(const std::vector<std::vector<int>>& seats) {
    print("      Seat\nRow   ", OutputManager::Color::CYAN);
    setColour(OutputManager::Color::CYAN);
    for (int i = 0; i < seats[0].size(); ++i) {
        std::cout << std::setw(2) << i + 1;
    }
    std::cout << "\n--------------------------\n";
    reset();

    for (int i = 0; i < seats.size(); ++i) {
        setColour(OutputManager::Color::CYAN);
        std::cout << std::setw(3) << i + 1 << " | ";
        reset();

        for (int j = 0; j < seats[i].size(); ++j) {
            if (seats[i][j] == 1) {
                print(" P", OutputManager::Color::GREEN);  // Present
            }
            else if (seats[i][j] == 0) {
                print(" A", OutputManager::Color::RED);    // Absent (registered but not present)
            }
            else {
                print(" -", OutputManager::Color::WHITE);  // Not registered/available
            }
        }
        std::cout << std::endl;
    }

    print("\nLegend: ");
    print("P = Present, ", OutputManager::Color::GREEN);
    print("A = Absent, ", OutputManager::Color::RED);
    println("- = Not Registered", OutputManager::Color::WHITE);
}

void OutputManager::clearScreen() {
    system("cls");
}