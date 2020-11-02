#include "src/test_classes/PidTestClass.hpp"

#include <iostream>

int sqr(int x) {
    sleep(1);
    return x * x;
}

int zero(int x) {
    return 0;
}

int div2(int x) {
    sleep(5000);
    return x / 2;
}


int main(int argc, char **argv)
{
    sleep(10);
    Manager testClass = Manager(3, div2, zero);
    std::cout << testClass.run() << "\n";
    return 0;
}
