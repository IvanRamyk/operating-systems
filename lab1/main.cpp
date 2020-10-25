#include "src/test_classes/PidTestClass.hpp"

#include <future>
#include <iostream>

int sqr(int x) {
    std::cout << "processing...\n";
    return x * x;
}
int five() {
    std::cout << "processing...\n";
    sleep(5);
    return 5;
}

int div2(int x) {
    sleep(1);
    return x / 2;
}


int main(int argc, char **argv)
{
//    PidTestClass testClass = PidTestClass(3, sqr, div2);
//    testClass.run();
    std::future<int> result(std::async(div2, 3));

    //sleep(10);
    while(result.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
        std::cout << "Not finished yet\n";
    }
    std::cout << result.get();
    return 0;
}
