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

class Manager {

public:
    Manager(int _x, int (*_f)(int), int (*_g)(int));

    int run();

private:
    static void print_function_computation_result(int result);

    termios set_terminal_for_cancellation_mode();

    void restore_terminal(const termios &terminal);

    void run_keyboard_listener();

    void terminate_unfinished();

    static void send_message(char* message, int port);

    int run_function_computation(int (*function)(int), int port);

    static int get_function_computation_result_from_port(int port);

    int x;
    int (*f)(int);
    int (*g)(int);
    int port_for_f = 4030;
    int port_for_g = 4031;
    int f_pid{};
    int g_pid{};
    bool f_is_ready = false;
    bool g_is_ready = false;
    std::future<int> f_x_computation;
    std::future<int> g_x_computation;
    bool is_finished = false;
    int oldfl = 0;
};


#endif //LAB1_MANAGER_HPP
