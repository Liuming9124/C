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
#include <ctime>
#include <sys/stat.h>

using namespace std;

// Capability list structure
struct FilePermissions {
    string owner;
    string group;
    string permissions; // rwr---
    size_t fileSize;
    string lastModified;
};

// Global variables
unordered_map<string, FilePermissions> capabilityList;
unordered_map<string, shared_mutex> fileLocks; // File locks
mutex capabilityListMutex;

// Get file metadata
void updateFileMetadata(const string& fileName, FilePermissions& filePerm) {
    struct stat fileStat;
    if (stat(fileName.c_str(), &fileStat) == 0) {
        filePerm.fileSize = fileStat.st_size;

        // Convert last modified time to string
        char buffer[30];
        strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", localtime(&fileStat.st_mtime));
        filePerm.lastModified = string(buffer);
    } else {
        filePerm.fileSize = 0;
        filePerm.lastModified = "N/A";
    }
}

// Utility function to check permissions
bool checkPermission(const string& user, const string& group, const string& operation, const string& fileName) {
    lock_guard<mutex> lock(capabilityListMutex);

    // Check if file exists in the capability list
    if (capabilityList.find(fileName) == capabilityList.end())
        return false; // File does not exist

    const FilePermissions& filePerm = capabilityList[fileName];
    int index;

    // Determine which permission set to check
    if (user == filePerm.owner) {
        // Owner permissions
        index = 0;
    } else if (group == filePerm.group) {
        // Group permissions
        index = 2; // Group permissions start at index 2
    } else {
        // Others permissions
        index = 4; // Others permissions start at index 4
    }

    // Determine the operation type
    if (operation == "read") {
        index += 0; // First position is read
    } else if (operation == "write") {
        index += 1; // Second position is write
    } else {
        return false; // Invalid operation
    }

    // Check if the permission is allowed
    return filePerm.permissions[index] == 'r' || filePerm.permissions[index] == 'w';
}



// Debug function to print the current capability list
void printCapabilityList() {
    lock_guard<mutex> lock(capabilityListMutex);
    cout << "Current Capability List:" << endl;
    for (const auto& entry : capabilityList) {
        cout << "File: " << entry.first
             << ", Owner: " << entry.second.owner
             << ", Group: " << entry.second.group
             << ", Permissions: " << entry.second.permissions
             << ", Size: " << entry.second.fileSize
             << " bytes, Last Modified: " << entry.second.lastModified << endl;
    }
}

// Function to handle client commands
void handleClient(Connection conn) {
    try {
        // Step 1: Receive user and group from client
        string userGroupMessage = conn.rx();  // Receive the combined message
        size_t delimiterPos = userGroupMessage.find(":");

        // Extract user and group
        string user = userGroupMessage.substr(0, delimiterPos);
        string group = userGroupMessage.substr(delimiterPos + 1);

        cout << "Client connected as User: " << user << ", Group: " << group << endl;

        while (true) {
            string command = conn.rx();
            if (command.empty()) break;

            vector<string> tokens;
            istringstream stream(command);
            string token;
            while (stream >> token) tokens.push_back(token);

            if (tokens[0] == "create" && tokens.size() == 3) {
                // Command: create <filename> <permissions>
                lock_guard<mutex> lock(capabilityListMutex);

                // Check if the file already exists
                if (capabilityList.find(tokens[1]) != capabilityList.end()) {
                    conn.tx("File already exists");
                    continue;
                }

                // Create an empty physical file
                ofstream file(tokens[1]);
                if (!file) { // If file creation fails
                    conn.tx("Failed to create the file on the server");
                    continue;
                }
                file.close();

                // Add file metadata to the capability list
                FilePermissions newFile = {user, group, tokens[2], 0, "N/A"};
                capabilityList[tokens[1]] = newFile;

                // Update metadata: size and timestamp
                updateFileMetadata(tokens[1], capabilityList[tokens[1]]);
                fileLocks[tokens[1]]; // Initialize file lock

                conn.tx("File created successfully");
            } else if (tokens[0] == "read" && tokens.size() == 2) {
                // Command: read <filename>
                string fileName = tokens[1];

                if (!checkPermission(user, group, "read", fileName)) {
                    conn.tx("Permission denied: User lacks read permissions");
                    continue;
                }

                shared_lock<shared_mutex> lock(fileLocks[fileName]);
                ifstream file(fileName);
                if (file.is_open()) {
                    if (file.peek() == ifstream::traits_type::eof()) {
                        conn.tx("File is empty");
                    } else {
                        stringstream buffer;
                        buffer << file.rdbuf();
                        conn.tx(buffer.str());
                    }
                    file.close();
                    updateFileMetadata(fileName, capabilityList[fileName]);
                } else {
                    conn.tx("File not found");
                }
            } else if (tokens[0] == "write" && tokens.size() >= 3) {
                string fileName = tokens[1];
                string mode = tokens[2];

                if (!checkPermission(user, group, "write", fileName)) {
                    conn.tx("Permission denied: User lacks write permissions");
                    continue;
                }

                unique_lock<shared_mutex> lock(fileLocks[fileName]); // Exclusive lock for writing

                conn.tx("Editing file. Enter lines and use :wq to save and exit.");

                ios::openmode openMode = (mode == "o") ? ios::trunc : ios::app;
                ofstream file(fileName, openMode);
                if (!file.is_open()) {
                    conn.tx("Failed to open file for writing");
                    continue;
                }

                while (true) {
                    string line = conn.rx(); // Receive a line from client
                    cout << "Received line: " << line << endl; // Debug output

                    if (line == ":wq") {
                        conn.tx("File saved and exited writing mode.");
                        break; // Exit loop when :wq is received
                    }
                    file << line << endl; // Write line to file
                }

                file.close();
                updateFileMetadata(fileName, capabilityList[fileName]); // Update file metadata
            } else if (tokens[0] == "mode" && tokens.size() == 3) {
                // Command: mode <filename> <new-permissions>
                string fileName = tokens[1];
                string newPermissions = tokens[2];

                lock_guard<mutex> lock(capabilityListMutex);
                if (capabilityList.find(fileName) == capabilityList.end()) {
                    conn.tx("File not found");
                    continue;
                }

                if (capabilityList[fileName].owner != user) {
                    conn.tx("Permission denied: Only the owner can change permissions");
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
        int port;

        // 請用戶輸入埠號
        cout << "Enter port number to listen on: ";
        cin >> port;
        // 建立監聽器
        PortListener listener(port);

        while (true) {
            Connection conn = listener.waitForConnection();
            thread(handleClient, move(conn)).detach();
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
}
