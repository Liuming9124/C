#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdexcept>


class Socket {
        int sock;
public:
        Socket(int socket) : sock(socket) {
                if (sock<0) throw std::runtime_error("Socket creation error");
        }
        Socket() : Socket(socket(AF_INET, SOCK_STREAM, 0)) {}
        std::string rx() {
                char buffer[1024] = {0};
                int n = read( sock , buffer, sizeof(buffer));
                return std::string(buffer,n);
        }
        void tx(std::string s) {
                send(sock , s.c_str() , s.length() , 0);
        }
        int getSocket() {
                return sock;
        }
};

class Connection: public Socket {
public:
        Connection(int socket) : Socket(socket) {}
        Connection(std::string address,unsigned short port): Socket()
        {
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port);
                // Convert IPv4 and IPv6 addresses from text to binary form
                if(inet_pton(
                                AF_INET,
                                address.c_str(),
                                &serv_addr.sin_addr
                ) <= 0) throw std::runtime_error("Invalid address: Address not supported");

                if (connect(
                                getSocket(),
                                (struct sockaddr *)&serv_addr,
                                sizeof(serv_addr)
                ) < 0) throw std::runtime_error("\nConnection Failed \n");
        }
};

class PortListener {
        Socket server; // fd is created in default Socket constructor
        struct sockaddr_in address;
        int opt = 1;
public:
        PortListener(unsigned short port) {

                // Forcefully attaching socket to the port 8080
                if (setsockopt(
                                server.getSocket(),
                                SOL_SOCKET,
                                SO_REUSEADDR | SO_REUSEPORT,
                                &opt,
                                sizeof(opt)
                )) throw std::runtime_error("setsockopt");

                address.sin_family = AF_INET;
                address.sin_addr.s_addr = INADDR_ANY;
                address.sin_port = htons( port );

                // Forcefully attaching socket to the port 8080
                if (bind(
                                server.getSocket(),
                                (struct sockaddr *)&address,
                                sizeof(address)
                ) < 0) throw std::runtime_error("bind failed");


                if (listen(server.getSocket(), 3) < 0) {
                        throw std::runtime_error("listen");
                }
        }
        Connection waitForConnection() {
                int new_socket;
                int addrlen = sizeof(struct sockaddr_in);
                new_socket = accept(
                                server.getSocket(),
                                (struct sockaddr *)&address,
                                (socklen_t*)&addrlen
                );
                if (new_socket<0) throw std::runtime_error("accept");
                return Connection(new_socket);
        }
};