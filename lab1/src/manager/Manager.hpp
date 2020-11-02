//
// Created by Ivan Ramyk on 10/24/20.
//
#ifndef LAB1_MANAGER_HPP
#define LAB1_MANAGER_HPP

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <future>
#include <utility>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

int oldfl = 0;

termios replace_terminal(){

    termios old_tio{};      // for storing settings from old terminal
    tcgetattr(STDIN_FILENO, &old_tio);  // save old terminal

    termios new_tio{old_tio};
    new_tio.c_lflag &= (~ICANON & ~ECHO);// disable canonical mode (buffered i/o) and local echo
    oldfl = fcntl(0, F_GETFL);
    fcntl(0, F_SETFL, O_NONBLOCK);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    /* set the new settings */

    return old_tio;
}

void set_terminal(const termios &terminal){

    if (oldfl == -1) {
        return;
    }
    fcntl(0, F_SETFL, oldfl & ~O_NONBLOCK);
    tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
}


class Manager {
private:
    int x;
    int (*f)(int);
    int (*g)(int);
    int port_for_f = 4030;
    int port_for_g = 4031;
    int f_pid;
    int g_pid;
    bool f_is_ready = false;
    bool g_is_ready = false;
    std::future<int> f_x_computation;
    std::future<int> g_x_computation;
    std::string status;

public:
    Manager(int _x, int (*_f)(int), int (*_g)(int)): x(_x), g(_g), f(_f) {}

    int run_keyboard_listener() {
        std::cout << "task is run\n";
        auto terminal = replace_terminal();
        std::cout << "some magic is happened\n" << status << "\n";
        while (status == "running") {
            sleep(1);
            std::cout << "bla-bla-bla\n";
            std::cout << "wait " << std::endl;
            char c = getchar();
            std::cout << "c = " << c << " \nPam-Pam\n";
            if (c == 'q') {
                {
                    std::cout << "bye!" << std::endl;
                }
                break;
            }
        }
        std::cout << "while is over\n";
        set_terminal(terminal);
        return -1;
    }

    void set_state(std::string new_value) {
        status = std::move(new_value);
    }

    void terminateUnfinished() {
        if (!f_is_ready) {
            send_message("0", port_for_f);
            kill(f_pid, SIGKILL);
        }
        if (!g_is_ready) {
            send_message("0", port_for_g);
            kill(g_pid, SIGKILL);
        }
    }

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
        status = "running";
        f_x_computation = std::future<int>(std::async(&Manager::get_result_from_port, this, port_for_f));
        g_x_computation = std::future<int>(std::async(&Manager::get_result_from_port, this, port_for_g));
        auto cancellation = std::future<int>(std::async(&Manager::run_keyboard_listener, this));
        f_pid = calculate(f, port_for_f);
        g_pid = calculate(g, port_for_g);
        std::cout << f_pid << " " << g_pid << "\n";
        int f_x_value, g_x_value;
        bool cancelled = false;
        while((!f_is_ready || !g_is_ready) && !cancelled) {
            if (cancellation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                cancelled = true;
                status = "cancelled";
                terminateUnfinished();
            }
            if (!f_is_ready && f_x_computation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                f_is_ready = true;
                f_x_value = f_x_computation.get();
                if (f_x_value == 0) {
                    terminateUnfinished();
                    set_state("finished");
                    return 0;
                }
            }
            if (!g_is_ready && g_x_computation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                g_is_ready = true;
                g_x_value = g_x_computation.get();
                if (g_x_value == 0) {
                    terminateUnfinished();
                    set_state("finished");
                    return 0;
                }
            }
        }
        std::cout << "something\n";
        return f_x_value * g_x_value;
    }
};


#endif //LAB1_MANAGER_HPP
