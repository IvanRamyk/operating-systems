//
// Created by Ivan Ramyk on 10/24/20.
//
#ifndef LAB1_PIDTESTCLASS_HPP
#define LAB1_PIDTESTCLASS_HPP

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <future>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


class Manager {
private:
    int x;
    int (*f)(int);
    int (*g)(int);
    int port_for_f = 4030;
    int port_for_g = 4031;
    std::future<int> f_x_computation;
    std::future<int> g_x_computation;
public:
    Manager(int _x, int (*_f)(int), int (*_g)(int)): x(_x), g(_g), f(_f) {}

    void send_message(char* message, int port) {
        int sock = 0;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
            exit(0);
        }
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            exit(0);
        }

        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            printf("\nConnection Failed \n");
            exit(0);
        }
        send(sock, message, strlen(message), 0);
    }

    int calculate(int (*function)(int), int port) {
        pid_t pid = fork();
        if (pid == 0) {
            std::string result = std::to_string(function(x));
            char message[100];
            strcpy(message, result.c_str());
            send_message(message, port);
            exit(0);
        }
        else if (pid < 0)
            std::cerr << "fork() failed!" << std::endl;
        else return pid;
    }


    int get_result_from_port(int port) {
        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[1024] = {0};

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) == -1)
            perror("setsockopt");
        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *)&address,
                 sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        read(new_socket , buffer, 1024);
        close(new_socket);
        close(server_fd);
        return std::atoi(buffer);

    }
    int run() {
        f_x_computation = std::future<int>(std::async(&Manager::get_result_from_port, this, port_for_f));
        g_x_computation = std::future<int>(std::async(&Manager::get_result_from_port, this, port_for_g));
        int f_pid = calculate(f, port_for_f);
        int g_pid = calculate(g, port_for_g);
        bool f_is_ready = false, g_is_ready = false;
        int f_x_value, g_x_value;
        while(!f_is_ready || !g_is_ready) {
            if (!f_is_ready && f_x_computation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                f_is_ready = true;
                f_x_value = f_x_computation.get();
                if (f_x_value == 0) {
                    kill(g_pid, SIGKILL);
                    send_message("0", port_for_g);
                    return 0;
                }
            }
            if (!g_is_ready && g_x_computation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                g_is_ready = true;
                g_x_value = g_x_computation.get();
                if (g_x_value == 0) {
                    kill(f_pid, SIGKILL);
                    send_message("0", port_for_f);
                    return 0;
                }
            }
        }
        return f_x_value * g_x_value;
    }
};


#endif //LAB1_PIDTESTCLASS_HPP
