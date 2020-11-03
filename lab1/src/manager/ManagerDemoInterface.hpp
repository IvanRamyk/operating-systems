//
// Created by Ivan Ramyk on 11/3/20.
//

#ifndef LAB1_MANAGERDEMOINTERFACE_HPP
#define LAB1_MANAGERDEMOINTERFACE_HPP

#include "Manager.hpp"
#include "../../demofuncs/demofuncs.h"

#include <iostream>


class ManagerDemoInterface {
private:
    static int zero_function(int x) {
        sleep(1);
        return 0;
    }

    static int sqr(int x) {
        sleep(3);
        return x * x;
    }

    static int endless(int x) {
        while (true) {}
    }

    static int div2(int x) {
        sleep(5);
        return x / 2;
    }

    static std::pair<Manager, int> manager_with_custom_functions(int test_case) {// returns a pair with manager and expected result
        switch (test_case) {
            case 0:
                return {Manager(3, sqr, div2), 9};
            case 1:
                return {Manager(3, div2, sqr), 9};
            case 2:
                return {Manager(5, zero_function, endless), 0};
            case 3:
                return {Manager(5, endless, zero_function), 0};
            case 4:
                return {Manager(5, div2, endless), -1};
            case 5:
                return {Manager(5, endless, sqr), -1};
        }
    }

public:
    static void run() {
        while (true) {
            std::cout << "Enter 1 to start testing with demofuncs library\n"
                         "Enter 2 to start testing with custom functions\n"
                         "Or 'exit' to finish program";
            std::string command;
            std::cin >> command;
            if (command == "exit")
                break;
            if (command == "1" || command == "2") {
                while (true) {
                    std::cout << "Please choose test-case (0 - 5). Enter any other number to exit\n";
                    int test_number;
                    std::cin >> test_number;
                    if (test_number >= 0 && test_number < 6) {
                        if (command == "1") {
                            Manager manager(test_number, spos::lab1::demo::f_func<spos::lab1::demo::INT>,
                                            spos::lab1::demo::g_func<spos::lab1::demo::INT>);
                            manager.run();
                        }
                        else  {
                            auto manager_and_result = manager_with_custom_functions(test_number);
                            int result = manager_and_result.first.run();
                            assert(result == manager_and_result.second);
                        }
                    }
                    else
                        break;
                }
            }
            else
                std::cout << "Try again\n";
        }
    }
};

#endif //LAB1_MANAGERDEMOINTERFACE_HPP
