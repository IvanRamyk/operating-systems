#include "src/manager/ManagerDemoInterface.hpp"

int main(int argc, char **argv)
{
    auto manager_demo_interface = ManagerDemoInterface();
    ManagerDemoInterface::run();
    return 0;
}
