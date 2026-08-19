
#include<list>
#include<algorithm>
#include<chrono>
#include<stdexcept>
#include<iostream>
#include<thread>
#include<unordered_map>

using namespace std;
using Clock = chrono::steady_clock;

class LRUCache {
    private:
        struct Entry {
            string value;
            Clock::time_point expiry;
            list<string>::iterator pos;
        };

        unordered_map<string, Entry> data;
        list<string> lru;

        size_t capacity;

        void evict() {
            if(lru.empty())
                return;
            
            string key = lru.back();
            lru.pop_back();
            data.erase(key);
        }

        void refresh(unordered_map<string, Entry>::iterator itr) {
            lru.erase(itr->second.pos);
            lru.push_front(itr->first);

            itr->second.pos =  lru.begin();
        }
    public:
        LRUCache(size_t capacity): capacity(capacity) {};

        string get(string key) {
            auto it = data.find(key);
            if(it == data.end()) {
                throw runtime_error("unable to find the key");
            }

            refresh(it);
            return it->second.value;
        }

        void put(string key, string value, chrono::milliseconds ttl) {
            auto it = data.find(key);
            if (it != data.end()) {
                it->second.value = value;
                it->second.expiry = Clock::now() + ttl;

                refresh(it);

                return;
            }

            if(lru.size() >= capacity) {
                evict();
            }


            lru.push_front(key);            
            data[key] = {value, Clock::now() + ttl, lru.begin()};
        }

        size_t size() {
            return lru.size();
        }

        void printLRU() {
            for(auto it = lru.begin(); it != lru.end(); ++it)
                cout << *it << " ";
            cout << "\n";
        }
};

int main() {
    LRUCache lru(10);

    lru.put("1", "1", std::chrono::milliseconds(10000));
    lru.put("2", "1", std::chrono::milliseconds(12000));
    lru.put("3", "1", std::chrono::milliseconds(14000));
    lru.put("4", "1", std::chrono::milliseconds(16000));
    lru.put("5", "1", std::chrono::milliseconds(18000));

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    lru.printLRU();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    lru.get("1");
    lru.printLRU();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    lru.get("2");
    lru.printLRU();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    lru.get("3");
    lru.printLRU();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    lru.get("4");
    lru.printLRU();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    lru.printLRU();


    return 0;
}