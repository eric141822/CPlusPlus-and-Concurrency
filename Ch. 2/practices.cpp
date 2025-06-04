#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <functional>
#include <stdexcept>

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

void bg_work(const std::string& task_name, unsigned duration) {
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

    // Move constructor
    ThreadGuard(ThreadGuard&& other) noexcept : t(std::move(other.t)) {}

    // Move assignment
    ThreadGuard& operator=(ThreadGuard&& other) noexcept {
        if (t.joinable()) {
            t.join();
        }
        t = std::move(other.t);
        return *this;
    }

    ThreadGuard(const ThreadGuard& other) = delete;
    ThreadGuard& operator=(const ThreadGuard& other) = delete;
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

void print_data(int val, const std::string& str) {
    std::cout << "Value: " << val << ", Text: " << str << std::endl;
}

void modify_data(int& val) {
    val *= 2;
}

void process_vec(std::vector<int> vec) {
    for (auto& v : vec) v *= 2;
    std::cout << "Vector processed in thread (by value)." << std::endl;
}

void process_vec_ref(std::vector<int>& vec) {
    for (auto& v : vec) v *= 2;
    std::cout << "Vector processed in thread (by reference)." << std::endl;
}

void print_vec(const std::vector<int>& vec) {
    for (auto& v : vec) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

class MoveOnlyObj {
private:
    std::unique_ptr<int> data;
public:
    // constructor
    explicit MoveOnlyObj(int val) : data(std::make_unique<int>(val)) {}

    // Move constructor
    // Note: In general, never make Move constructors explicit
    MoveOnlyObj(MoveOnlyObj&& other) : data(std::move(other.data)) {}

    // Move assignment
    MoveOnlyObj& operator=(MoveOnlyObj&& other) noexcept {
        data = std::move(other.data);
        return *this;
    }

    void print() const {
        if (data) std::cout << "Data in object: " << *data << std::endl;
    }
    
    // Disable copy assigment and constructors.
    MoveOnlyObj(const MoveOnlyObj& other) = delete;
    MoveOnlyObj& operator=(const MoveOnlyObj& other) = delete;
};

void print_obj(MoveOnlyObj obj) {
    obj.print();
}

void print_unique_ptr(std::unique_ptr<int> p) {
    std::cout << "Value in unique ptr: " << *p << std::endl;
}

void practice_4() {
    std::cout << "\n=== Practice 4: Passing Arguments to Threads ===\n";
    std::thread t1(print_data, 12, "some data");
    t1.join();

    int val = 3;
    std::cout << "Value before modification in thread: " << val << std::endl;
    std::thread t2(modify_data, std::ref(val));
    t2.join();
    std::cout << "Value after modification in thread: " << val << std::endl;

    std::vector<int> vec1 = {1,2,3,4,5};
    std::vector<int> vec2 = {1,2,3,4,5};

    std::thread t3(process_vec, vec1);
    t3.join();
    
    std::thread t4(process_vec_ref, std::ref(vec2));
    t4.join();

    std::cout << "Original vec1 after by-value processing: ";
    print_vec(vec1);

    std::cout << "Original vec2 after by-value processing: ";
    print_vec(vec2);

    std::unique_ptr<int> p(std::make_unique<int>(10));
    std::thread t5(print_unique_ptr, std::move(p));
    t5.join();

    MoveOnlyObj obj(20);
    std::thread t6(print_obj, std::move(obj));
    t6.join();
}

void bg_work_with_id(unsigned work_num) {
    unsigned duration = work_num*100;
    std::cout << "Worker " << work_num <<", Thread ID=" << std::this_thread::get_id() << " executing for " << duration << " millisecond(s)" << std::endl;
    short_sleep(duration);
    std::cout << "Worker " << work_num <<", Thread ID=" << std::this_thread::get_id() << " complete." << std::endl;
}

void practice_5() {
    std::cout << "\n=== Practice 5: Thread Identification ===\n";

    std::cout << "Main thread ID: " << std::this_thread::get_id() << std::endl;

    unsigned int num_threads(std::thread::hardware_concurrency());
     // fallback if hardware_concurrency not supported;
    if (num_threads == 0) {
        num_threads = 2;
        std::cout << "std::thread::hardware_concurrency not supported, setting num_thread=2" << std::endl;
    } else {
        std::cout << "Maximum hardware supported thread num=" << num_threads << std::endl;
    }

    std::vector<std::thread> threads;

    for (unsigned i = 0; i < num_threads; ++i) {
        threads.emplace_back(bg_work_with_id, i+1);
    }

    for (auto &t : threads) {
        std::cout << "Created thread ID: " << t.get_id() << std::endl;
    }

    for (auto &t : threads) t.join();
}

int main() {
    std::cout << "Main running:\n";
    practice_1();
    practice_2();
    practice_3();
    practice_4();
    practice_5();
}