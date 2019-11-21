#include <list>
#include <string>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

class customWorkQ
{
private:
    std::list<std::string> workQ;
    std::mutex mu;
    std::condition_variable conditionVar;

public:
    int get_size()
    {
        std::lock_guard<std::mutex> lock{mu};
        return workQ.size();
    }

    void push(std::string value)
    {
        std::unique_lock<std::mutex> lock{mu};
        workQ.push_back(std::move(value));
        lock.unlock();
        conditionVar.notify_one();
    }

    std::string get_pop_front()
    {
        std::lock_guard<std::mutex> lock{mu};
        std::string filename = workQ.front();
        workQ.pop_front();
        return filename;
    }
};

class customTable
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

