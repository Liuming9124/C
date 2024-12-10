#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

using namespace std;

struct File {
    string owner;
    string group;
    string permissions; // Format: "rwxrwxrwx"
    string content;
    mutex file_mutex;
};

unordered_map<string, File> files;
mutex files_mutex;

void handleClient(SOCKET client_socket) {
    char buffer[1024] = {0};
    while (true) {
        int bytes_read = recv(client_socket, buffer, 1024, 0);
        if (bytes_read <= 0) break;

        string command(buffer);
        memset(buffer, 0, sizeof(buffer));

        string response;
        {
            lock_guard<mutex> guard(files_mutex);
            // Parse command and perform actions
            vector<string> tokens;
            size_t pos;
            while ((pos = command.find(' ')) != string::npos) {
                tokens.push_back(command.substr(0, pos));
                command.erase(0, pos + 1);
            }
            tokens.push_back(command);

            if (tokens[0] == "create" && tokens.size() == 3) {
                string filename = tokens[1];
                string permissions = tokens[2];

                if (files.find(filename) != files.end()) {
                    response = "Error: File already exists.";
                } else {
                    files[filename] = {"", "AOS-students", permissions, ""};
                    response = "File created successfully.";
                }
            } else if (tokens[0] == "read" && tokens.size() == 2) {
                string filename = tokens[1];

                if (files.find(filename) == files.end()) {
                    response = "Error: File does not exist.";
                } else {
                    File &file = files[filename];
                    lock_guard<mutex> file_guard(file.file_mutex);

                    if (file.permissions[0] != 'r') {
                        response = "Error: No read permission.";
                    } else {
                        response = "File content: " + file.content;
                    }
                }
            } else if (tokens[0] == "write" && tokens.size() >= 3) {
                string filename = tokens[1];
                string mode = tokens[2];
                string content = tokens.size() > 3 ? tokens[3] : "";

                if (files.find(filename) == files.end()) {
                    response = "Error: File does not exist.";
                } else {
                    File &file = files[filename];
                    lock_guard<mutex> file_guard(file.file_mutex);

                    if (file.permissions[1] != 'w') {
                        response = "Error: No write permission.";
                    } else {
                        if (mode == "o") {
                            file.content = content;
                        } else if (mode == "a") {
                            file.content += content;
                        } else {
                            response = "Error: Invalid write mode.";
                            continue;
                        }
                        response = "File written successfully.";
                    }
                }
            } else if (tokens[0] == "mode" && tokens.size() == 3) {
                string filename = tokens[1];
                string permissions = tokens[2];

                if (files.find(filename) == files.end()) {
                    response = "Error: File does not exist.";
                } else {
                    File &file = files[filename];
                    file.permissions = permissions;
                    response = "Permissions updated successfully.";
                }
            } else {
                response = "Error: Invalid command.";
            }
        }

        send(client_socket, response.c_str(), response.size(), 0);
    }

    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed." << endl;
        return -1;
    }

    SOCKET server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cerr << "Socket creation failed." << endl;
        WSACleanup();
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        cerr << "Bind failed." << endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        cerr << "Listen failed." << endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    cout << "Server is running on port " << PORT << endl;

    while (true) {
        SOCKET new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) == INVALID_SOCKET) {
            cerr << "Accept failed." << endl;
            closesocket(server_fd);
            WSACleanup();
            return -1;
        }

        thread client_thread(handleClient, new_socket);
        client_thread.detach();
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
