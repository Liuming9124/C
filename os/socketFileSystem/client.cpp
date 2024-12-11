#include "Socket.h"
#include <iostream>
#include <sstream>

using namespace std;

void sendEditCommand(Connection& conn, const string& command) {
    cout << "Editing file. Type your lines below (use :wq to save and exit):" << endl;

    // Send the write command to server
    conn.tx(command);
    string response = conn.rx();
    cout << response << endl;

    while (true) {
        string line;
        getline(cin, line); // Get user input line-by-line
        conn.tx(line);      // Send each line to the server
        if (line == ":wq") {
            response = conn.rx(); // Receive save confirmation
            cout << response << endl;
            break;
        }
    }
}


void sendCommand(Connection& conn, const string& command) {
    // Determine if the command is "write" with a valid mode
    if (command.substr(0, 5) == "write") {
        sendEditCommand(conn, command);
    } else {
        cout << "Sending command: " << command << endl;
        conn.tx(command);
        string response = conn.rx();
        cout << "Server response: " << response << endl;
    }
}

int main() {
    try {
        Connection conn("127.0.0.1", 8080);

        string user, group;
        cout << "Enter your username: ";
        getline(cin, user);
        cout << "Enter your group: ";
        getline(cin, group);

        // Combine user and group into a single message
        string userGroupMessage = user + ":" + group;
        conn.tx(userGroupMessage);

        cout << "Connected to the server. Type your commands below:" << endl;

        string command;
        while (true) {
            cout << "> ";
            getline(cin, command);

            if (command == "exit") {
                cout << "Exiting the client..." << endl;
                break;
            }

            if (command.empty()) {
                cout << "Please enter a valid command." << endl;
                continue;
            }

            sendCommand(conn, command);
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return 0;
}
