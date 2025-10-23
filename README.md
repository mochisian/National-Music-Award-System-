# SPC Event Management System

A console-based application for managing events, built with C++. It provides a range of functionalities for event organizers and attendees, including event creation, registration, payment processing, and reporting.

## Features

*   **User Management**: Login system for different user roles (e.g., Customer, Organizer).
*   **Event Lifecycle**: Create, schedule, and manage events.
*   **Registration & Booking**: Allows users to register for events and book their spots.
*   **Payments**: Includes functionality for payment checkout and refund processing.
*   **Dashboards**: Separate dashboards for Customers and Organizers to view relevant information.
*   **Reporting**: Generate reports on events, attendance, and finances.
*   **Voting System**: A feature for users to vote on specific items, likely related to events.

## Prerequisites

*   Visual Studio 2019 or later.
*   A C++ compiler that supports C++17 or later.

## Getting Started

### 1. Download and Extract the Project Zip File

1. Right-click the zip file and select Extract All (Windows) or Extract Here (Linux/macOS).
2. After extraction, you may see an extra folder created (e.g., SPC_Assignment/SPC_Assignment/).
3. Open the inner folder that contains the actual project files (including the .sln file). This is the correct project root.

### 2. Open the Solution in Visual Studio

1. Start Microsoft Visual Studio.
2. Go to File → Open → Project/Solution.
3. Browse to the extracted folder and select the SPC_Assignment.sln file.
4. Visual Studio will load the project and you’ll see it in the Solution Explorer.

### 3. Configure Project Properties

To ensure the project can find the necessary libraries, you need to configure the include directories.

1.  In the **Solution Explorer**, right-click on the **SPC_Assignment** project and select **Properties**.
2.  Ensure the **Configuration** is set to **All Configurations** and **Platform** is set to **All Platforms**.
3.  Navigate to **Configuration Properties** -> **VC++ Directories**.
4.  In the **Include Directories** field, click the dropdown arrow and select **<Edit...>**.
5.  Click the 'New Line' icon and add the following path:
    ```
    $(SolutionDir)include
    ```
6.  Click **OK** to save the changes.

### 4. Set C++ Language Standard

1.  In the same **Properties** window, navigate to **Configuration Properties** -> **C/C++** -> **Language**.
2.  Set the **C++ Language Standard** to **ISO C++17 Standard (/std:c++17)** or a newer version.
3.  Click **Apply** and then **OK**.

## Building and Running

*   **Build the Solution**: From the Visual Studio top menu, select `Build` > `Build Solution`.
*   **Run the Application**: To start the program, select `Debug` > `Start Without Debugging` or press `Ctrl+F5`.

## Dependencies

This project utilizes the following third-party library:

*   **[nlohmann/json](https://github.com/nlohmann/json)**: A powerful and modern C++ library for JSON manipulation. The necessary header file (`json.hpp`) is included in the `include/` directory.
