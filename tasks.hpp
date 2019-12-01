
#include <string>
#include <vector>
#include <list>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <thread>
#include <condition_variable>
#include "threadClass.hpp"

struct notification_queue 
{

private:
    std::list<std::string> q;        // this is our list of tasks; each task is a function.
    bool done = false;
    std::mutex m; // a mutex to protect the queue
    std::condition_variable ready; // a condition variable indicating available tasks
public:
    std::string try_pop() 
    {
        std::unique_lock<std::mutex> lock(m, std::try_to_lock); // do not wait if the mutex is locked
        if (!lock || q.empty()) return ""; // we have to(!) check if the lock was acquired
        auto str = q.front();
        q.pop_front();
        return str;
    }

    bool try_push(std::string str) 
    {
        {
            std::unique_lock<std::mutex> lock(m, std::try_to_lock);
            if (!lock) return false; // return false to indicate that the push has not been
            // successful
            q.push_back(str);
        }
        ready.notify_one();
        return true; // return true if the task was successfully added to the queue
    }

    std::string pop() 
    {
        std::unique_lock<std::mutex> lock(m);
        ready.wait(lock, [this] {return (!q.empty()) || done;}); // if done is set we will not wait
        if (q.empty()) return ""; // if the queue is empty (because done was set) we return 'nothing'
        auto f = q.front();
        q.pop_front();
        return f;
    }

    void push(std::string str) 
    { // push adds a task to the queue
        {
            std::unique_lock<std::mutex> lock(m);           // enter critical section to protect the std::list
            q.push_back(str);   // add task f to the queue
        }                   // exit critical section
        ready.notify_one(); // notify a waiting thread that a task is in the queue
    }
    void setDone() 
    {
        {
            std::unique_lock<std::mutex> lock(m);      // enter critical region to protect 'done'
            done = true; // set done
        }
        ready.notify_all(); // Notify all threads that they should finish
    }
};

struct task_system 
{

private:
    int count = 0;
    
    std::vector<std::thread> threads; // a vector to store all the worker threads
    std::vector<notification_queue> q;
    // this 'atomic' int can be incremented thread-safely without requiring a lock
    std::atomic<int> index;

public:
    task_system(std::function<void(std::vector<notification_queue>&, int, int, task_system*)> f, int nbWorker)
    { // This is the constructor
        index = 0;
        count = nbWorker;
        printf("Start task system with %d threads\n", count);
        q = std::vector<notification_queue>(count);
        for (auto n = 0; n != count; n++)
        { // for each hardware core ...
            threads.emplace_back([&, n]() { f(q, n, count, this); });
        } // ... we create a thread executing the given function
    }
    void async(std::string str) 
    {
        auto i = index++;
        // first try to insert in all other queues
        for (auto n = 0; n != count; ++n) 
        {
            // if an insertion is successful, return from the function
            if (q[(i + n) % count].try_push(str))
            return;
        }
        // only if no other insertion was successful, add in our own queue
        // and potentially block
        q[i % count].push(str);
    }
    void setDone()
    {
        for (auto n = 0; n != count; n++)
        {
            q[n].setDone();
        } // indicate each queue to finish
        for (auto n = 0; n != count; n++)
        {
            threads[n].join();
        }
    }
};
