#include <iostream>
#include <thread>

void hello() {
    std::cout << "Hello world (concurrently)!\n";
}

int main() {
    std::thread t(hello);
    t.join();
}