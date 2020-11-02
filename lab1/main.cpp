#include "src/manager/Manager.hpp"

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
    Manager manager = Manager(3, div2, zero);
    std::cout << manager.run() << "\n";
    return 0;
}
