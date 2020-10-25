//
// Created by Ivan Ramyk on 10/24/20.
//
#ifndef LAB1_PIDTESTCLASS_HPP
#define LAB1_PIDTESTCLASS_HPP

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>


class PidTestClass {
private:
    int x;
    int (*f)(int);
    int (*g)(int);
public:
    PidTestClass(int _x, int (*_f)(int), int (*_g)(int)): x(_x), g(_g), f(_f) {}
    void calculate(int (*function)(int)) {
        std::cout << function(x) << "\n";
    }
    void run() {
        pid_t pid = fork();

        if (pid == 0)
        {
            // child process
            sleep(1);
            calculate(f);
        }
        else if (pid > 0)
        {
            sleep(100);
            calculate(g);
        }
        else
        {
            // fork failed
            std::cerr << "fork() failed!" << std::endl;
        }
    }
};


#endif //LAB1_PIDTESTCLASS_HPP
