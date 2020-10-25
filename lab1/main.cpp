#include "src/test_classes/PidTestClass.hpp"

#include <future>
#include <iostream>

int sqr(int x) {
    std::cout << "processing...\n";
    return x * x;
}

int div2(int x) {
    sleep(1);
    return x / 2;
}


int main(int argc, char **argv)
{
    PidTestClass testClass = PidTestClass(3, sqr, div2);
    testClass.run();
    std::cout << "Hey!\n";
    return 0;
}
