#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void short_sleep(unsigned int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Practice 1: Thread Creation and Joining
// Create a simple program that spawns multiple threads to perform different tasks (e.g., one thread prints numbers, another prints letters)
// Practice using std::thread constructor with different callable types (functions, lambdas, function objects)
// Ensure proper joining of all threads before main() exits
void do_work() {
    std::cout << "Function thread id: " << std::this_thread::get_id() << " doing some work!\n";
}
std::thread func_thread() {
    std::thread t(do_work);
    return t;
}
std::thread lambda_thread() {
    std::thread t([]{
        std::cout << "Lambda thread id: " << std::this_thread::get_id() << " doing some work!\n";
    });
    return t;
}

class functor {
    public:
        void operator()() const {
            std::cout << "Functor thread id: " << std::this_thread::get_id() << " doing some work!\n";
        }
};

std::thread functor_thread() {
    std::thread t{functor()};
    return t;
}

void practice_1() {
    std::thread t1, t2, t3;
    t1 = func_thread();
    short_sleep(100);
    t2 = lambda_thread();
    short_sleep(100);
    t3 = functor_thread();
    short_sleep(100);
    t1.join();
    t2.join();
    t3.join();
}

int main() {
    std::cout << "Main running:\n";
    practice_1();
}