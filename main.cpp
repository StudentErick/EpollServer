#include <iostream>
#include "MyServer.h"

int main() {
    MyServer myServer(10);
    std::cout << "set server ok...\n\n";
    if (!myServer.listen(8001)) {
        throw std::runtime_error("listen error\n");
    }
    std::cout << "start listening...\n";
    if (!myServer.startService()) {
        std::runtime_error("start service error\n");
    }
    return 0;
}