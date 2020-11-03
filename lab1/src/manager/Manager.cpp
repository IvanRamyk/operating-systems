//
// Created by Ivan Ramyk on 10/24/20.
//

#include "Manager.hpp"

int Manager::run() {
    std::cout << "Processing...\n";
    auto start_of_computation = clock();
    f_x_computation = std::future<int>(std::async(&Manager::get_function_computation_result_from_port, port_for_f));
    g_x_computation = std::future<int>(std::async(&Manager::get_function_computation_result_from_port, port_for_g));
    auto cancellation = std::future<void>(std::async(&Manager::run_keyboard_listener, this));
    f_pid = run_function_computation(f, port_for_f);
    g_pid = run_function_computation(g, port_for_g);
    int f_x_value, g_x_value;
    while(!f_is_ready || !g_is_ready) {
        if (cancellation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            is_finished = true;
            terminate_unfinished();
            std::cout << "Computations were canceled.\n";
            return -1;
        }
        if (!f_is_ready && f_x_computation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            f_is_ready = true;
            f_x_value = f_x_computation.get();
            std::cout << "Computation of f(x) value is finished. f value is " << f_x_value << "\n";
            if (f_x_value == 0) {
                std::cout << "f(x) has a zero value, so computation of g(x) value will be terminated immediately\n";
                terminate_unfinished();
                is_finished = true;
                print_function_computation_result(0);
                return 0;
            }
        }
        if (!g_is_ready && g_x_computation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            g_is_ready = true;
            g_x_value = g_x_computation.get();
            std::cout << "Computation of g(x) value is finished. g value is " << g_x_value << "\n";
            if (g_x_value == 0) {
                std::cout << "g(x) has a zero value, so computation of f(x) value will be terminated immediately\n";
                terminate_unfinished();
                is_finished = true;
                print_function_computation_result(0);
                return 0;
            }
        }
    }
    is_finished = true;
    int result = f_x_value * g_x_value;
    print_function_computation_result(result);
    return result;
}

Manager::Manager(int _x, int (*_f)(int), int (*_g)(int)) : x(_x), g(_g), f(_f) {}

void Manager::print_function_computation_result(int result) {
    std::cout << "Computations are finished. Result: " << result << "\n\n";
}

termios Manager::set_terminal_for_cancellation_mode() {// returns old terminal properties
    termios old_properties{};
    tcgetattr(STDIN_FILENO, &old_properties);
    termios new_properties{old_properties};
    new_properties.c_lflag &= ~ICANON & ~ECHO;
    oldfl = fcntl(0, F_GETFL);
    fcntl(0, F_SETFL, O_NONBLOCK);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_properties);
    return old_properties;
}

void Manager::restore_terminal(const termios &terminal) {
    if (oldfl == -1) {
        return;
    }
    fcntl(0, F_SETFL, oldfl & ~O_NONBLOCK);
    tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
}

void Manager::run_keyboard_listener() {
    auto old_terminal_properties = set_terminal_for_cancellation_mode();
    while (!is_finished) {
        char c = getchar();
        if (c == 'q')
            break;
    }
    restore_terminal(old_terminal_properties);
}

void Manager::terminate_unfinished() {
    if (!f_is_ready) {
        send_message("0", port_for_f);
        kill(f_pid, SIGKILL);
    }
    if (!g_is_ready) {
        send_message("0", port_for_g);
        kill(g_pid, SIGKILL);
    }
}

void Manager::send_message(char *message, int port) {
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "\n Socket creation error \n";
        return;
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "\nInvalid address/ Address not supported \n";
        return;
    }
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "\nConnection Failed \n";
        return;
    }
    send(sock, message, strlen(message), 0);
}

int Manager::run_function_computation(int (*function)(int), int port) {
    pid_t pid = fork();
    if (pid == 0) {
        std::string result = std::to_string(function(x));
        char message[20];
        strcpy(message, result.c_str());
        send_message(message, port);
        exit(0);
    }
    else if (pid < 0)
        std::cout << "fork creation failed!" << std::endl;
    else
        return pid;
}

int Manager::get_function_computation_result_from_port(int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[20] = {0};
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cout << "socket failed";
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) == -1)
        std::cout << "setsockopt";

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        std::cout << "bind failed";
        return -1;
    }
    if (listen(server_fd, 3) < 0)
    {
        std::cout << "listen";
        return -1;
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0)
    {
        std::cout << "\n accept error\n";
        return -1;
    }
    read(new_socket , buffer, 20);
    close(new_socket);
    close(server_fd);
    return std::atoi(buffer);

}
