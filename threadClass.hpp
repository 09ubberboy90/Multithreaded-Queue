#include <list>
#include <string>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

// struct customWorkQ
// {
// private:
//     std::list<std::string> workQ;

// public:
//     int get_size()
//     {
//         std::lock_guard<std::mutex> lock{global_mutex};
//         return workQ.size();
//     }

//     void push(std::string value)
//     {
//         std::unique_lock<std::mutex> lock{global_mutex};
//         workQ.push_back(std::move(value));
//         lock.unlock();
//     }

//     std::string get_pop_front()
//     {
//         std::lock_guard<std::mutex> lock{global_mutex};
//         if (workQ.size() > 0)
//         {
//             std::string filename = workQ.front();
//             workQ.pop_front();
//             return filename;
//         }
//         return "";
//     }
// };

struct customTable
{
private:
    std::unordered_map<std::string, std::list<std::string>> theTable;
    std::mutex mu;

public:
    void insert(std::string key, std::list<std::string> value)
    {
        std::lock_guard<std::mutex> lock{mu};
        theTable.insert({key, value});
    }
    bool find(std::string name)
    {
        std::lock_guard<std::mutex> lock{mu};
        return theTable.find(name) != theTable.end();
    }
    std::list<std::string> * get(std::string key)
    {
        std::lock_guard<std::mutex> lock{mu};
        return &theTable[key];
    }
};

