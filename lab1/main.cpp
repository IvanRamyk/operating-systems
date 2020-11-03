#include "src/manager/Manager.hpp"
#include "demofuncs/demofuncs.h"

#include <iostream>

int sqr(int x) {
    sleep(1);
    return x * x;
}

int zero(int x) {
    sleep(10);
    return 0;
}

int div2(int x) {
    sleep(5000);
    return x / 2;
}


int main(int argc, char **argv)
{
    while (true) {
        std::cout << "Enter 1 to start testing with demofuncs library\nEnter 2 to start testing with custom functions\n";
        std::string command;
        std::cin >> command;
        if (command == "exit")
            break;
        if (command == "1") {
            while (true) {
                std::cout << "Please choose test-case (0 - 5). Enter any other number to exit\n";
                int test_number;
                std::cin >> test_number;
                if (test_number >= 0 && test_number < 6) {
                    Manager manager(test_number, spos::lab1::demo::f_func<spos::lab1::demo::INT>,
                                    spos::lab1::demo::g_func<spos::lab1::demo::INT>);
                    manager.run();
                }
                else
                    break;
            }
        } else if (command == "2") {

        }
        else
            std::cout << "Try again\n";
    }
    return 0;
}
