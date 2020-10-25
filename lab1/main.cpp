#include "src/test_classes/PidTestClass.hpp"

#include <iostream>

int sqr(int x) {
    sleep(1);
    return x * x;
}

int zero(int x) {
    sleep(4);
    return 0;
}

int div2(int x) {
    sleep(5000);
    return x / 2;
}


int main(int argc, char **argv)
{
    Manager testClass = Manager(3, div2, zero);
    std::cout << testClass.run() << "\n";
    assert(testClass.run() != 13);
    exit(0);
}
