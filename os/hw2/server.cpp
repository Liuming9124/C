#include "Socket.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <thread>

using namespace std;

// Capability list structure
struct FilePermissions {
    string owner;
    string permissions; // rwxrwxrwx (owner, group, others)
};

// Global variables
unordered_map<string, FilePermissions> capabilityList;
unordered_map<string, shared_mutex> fileLocks; // File locks
mutex capabilityListMutex;

// Utility function to check permissions
bool checkPermission(const string& user, const string& operation, const string& fileName) {
    lock_guard<mutex> lock(capabilityListMutex);

    if (capabilityList.find(fileName) == capabilityList.end())
        return false; // File does not exist

    const FilePermissions& filePerm = capabilityList[fileName];
    int index = (user == filePerm.owner) ? 0 : 3; // Owner vs others
    if (operation == "read") index += 0;
    if (operation == "write") index += 1;

    return filePerm.permissions[index] == 'r' || filePerm.permissions[index] == 'w';
}

// Debug function to print the current capability list
void printCapabilityList() {
    lock_guard<mutex> lock(capabilityListMutex);
    cout << "Current Capability List:" << endl;
    for (const auto& entry : capabilityList) {
        cout << "File: " << entry.first
             << ", Owner: " << entry.second.owner
             << ", Permissions: " << entry.second.permissions << endl;
    }
}

// Function to handle client commands
void handleClient(Connection conn) {
    try {
        while (true) {
            string command = conn.rx();
            if (command.empty()) break;

            vector<string> tokens;
            istringstream stream(command);
            string token;
            while (stream >> token) tokens.push_back(token);

            if (tokens[0] == "create" && tokens.size() == 4) {
                // Command: create <filename> <permissions> <owner>
                lock_guard<mutex> lock(capabilityListMutex);
                capabilityList[tokens[1]] = {tokens[3], tokens[2]};
                fileLocks[tokens[1]]; // Initialize file lock
                conn.tx("File created successfully");

            } else if (tokens[0] == "read" && tokens.size() == 3) {
                // Command: read <filename> <user>
                string fileName = tokens[1];
                string user = tokens[2];

                if (!checkPermission(user, "read", fileName)) {
                    conn.tx("Permission denied");
                    continue;
                }

                shared_lock<shared_mutex> lock(fileLocks[fileName]);
                ifstream file(fileName);
                if (file.is_open()) {
                    stringstream buffer;
                    buffer << file.rdbuf();
                    conn.tx(buffer.str());
                } else {
                    conn.tx("File not found");
                }

            } else if (tokens[0] == "write" && tokens.size() >= 3) {
                // Command: write <filename> <user> <content>
                string fileName = tokens[1];
                string user = tokens[2];
                string content;
                for (size_t i = 3; i < tokens.size(); ++i) {
                    if (i > 3) content += " ";
                    content += tokens[i];
                }

                if (!checkPermission(user, "write", fileName)) {
                    conn.tx("Permission denied");
                    continue;
                }

                unique_lock<shared_mutex> lock(fileLocks[fileName]);
                ofstream file(fileName, ios::app);
                if (file.is_open()) {
                    file << content << endl;
                    conn.tx("File written successfully");
                } else {
                    conn.tx("File not found");
                }

            } else if (tokens[0] == "mode" && tokens.size() == 4) {
                // Command: mode <filename> <new-permissions> <user>
                string fileName = tokens[1];
                string newPermissions = tokens[2];
                string user = tokens[3];

                lock_guard<mutex> lock(capabilityListMutex);
                if (capabilityList.find(fileName) == capabilityList.end()) {
                    conn.tx("File not found");
                    continue;
                }

                if (capabilityList[fileName].owner != user) {
                    conn.tx("Permission denied");
                    continue;
                }

                capabilityList[fileName].permissions = newPermissions;
                conn.tx("Permissions updated");

            } else {
                conn.tx("Invalid command");
            }

            printCapabilityList(); // Print capability list after each command
        }
    } catch (const exception& e) {
        cerr << "Error handling client: " << e.what() << endl;
    }
}

int main() {
    try {
        PortListener listener(8080);
        while (true) {
            Connection conn = listener.waitForConnection();
            thread(handleClient, move(conn)).detach();
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
}
