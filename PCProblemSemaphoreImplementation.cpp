//(Producer-Consumer)
#include <iostream>
#include <cstdlib>  //rand
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>  //sleep
using namespace std;

class semaphore {
public:
    semaphore(int init) : Value(init) {  }
    void down();  //wait
    void up();  //signal
private:
    int Value;
    mutex mutexc;
    condition_variable cond;
};

void semaphore::down() {                 
    unique_lock<mutex> ul(mutexc);  //ul.lock();
    if (--Value < 0)
        cond.wait(ul);
}

void semaphore::up() {                   
    unique_lock<mutex> ul(mutexc);  //ul.lock();
    if (++Value <= 0)
        cond.notify_one();
}

#define bufferSize 1000;
vector <int> buffer;  //We Use This Vector As FIFO Buffer
semaphore mtx  = 1;
semaphore full = 0;
semaphore empt = bufferSize;

void produceData() {
    int input = (rand() % 10) + 1; // 1-10
    buffer.push_back(input);
    cout << "<- Produced: " << input << " \n";
}

void consumeData() {
    int output = buffer.front();
    buffer.erase(buffer.begin());
    cout << "-> Consumed: " << output << " \n";
}

void producer() {
    while (true) {
        empt.down();
        mtx.down();
        produceData();
        mtx.up();
        full.up();
    }
}

void consumer() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        full.down();
        mtx.down();
        consumeData();
        mtx.up();
        empt.up();
    }
}

void producerThread() {
    producer();
}

void consumerThread() {
    consumer();
}

int main() {
    thread t1(producerThread);
    thread t2(consumerThread);
    t1.join();
    t2.join();
    return 0;
}