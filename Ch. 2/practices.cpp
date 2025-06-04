#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
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
    std::cout << "\n=== Practice 1: Basic Thread Creation ===\n";
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

void bg_work(std::string task_name, unsigned duration) {
    std::cout << task_name << " executing for " << duration/1000 << " second(s)" << std::endl;
    short_sleep(duration);
    std::cout << task_name << " complete." << std::endl;
}

void practice_2() {
    std::cout << "\n=== Practice 2: Thread Detachment ===\n";
    std::thread t1(bg_work, "Task 1", 1000);
    std::thread t2(bg_work, "Task 2", 2000);

    t1.detach();
    t2.detach();

    std::cout << "Main thread about to sleep for 3 seconds." << std::endl;
    short_sleep(3000); 
    std::cout << "Main thread exiting." << std::endl;
}

class ThreadGuard {
private:
    std::thread t;
public:
    // constructor
    ThreadGuard(std::thread _t) noexcept : t(std::move(_t)) {}
    
    // destructor
    ~ThreadGuard() {
        if (t.joinable()) {
            t.join();
        }
    }

    // copy constructor
    ThreadGuard(ThreadGuard&& other) noexcept : t(std::move(other.t)) {}

    // copy assignment
    ThreadGuard& operator=(ThreadGuard&& other) noexcept {
        if (t.joinable()) {
            t.join();
        }
        t = std::move(other.t);
        return *this;
    }
};

void throw_except() {
    throw std::runtime_error("Some risky operation.");
}

void practice_3() {
    std::cout << "\n=== Practice 3: RAII Thread Management ===\n";

    try {
        // Thread guard gets deleted first when practice_3 goes out of scope.
        // so ThreadGuard's destructor joins t safely.
        // std::thread t(bg_work, "Worker task", 3000);
        // ThreadGuard guard(std::move(t));
        ThreadGuard guard(std::thread(bg_work, "Worker task", 3000));

        // exception thrown, guard goes out of scope, so thread joins before we go to the except block.
        // throw_except();

        std::cout << "Main doing some work." << std::endl;

        std::cout << "Work completed successfully\n";
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        std::cout << "Thread was safely joined by ThreadGuard" << std::endl;
    }
}

int main() {
    std::cout << "Main running:\n";
    practice_1();
    practice_2();
    practice_3();
}