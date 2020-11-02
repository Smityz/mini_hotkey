#include <bits/stdc++.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <unistd.h>
#include <bits/shared_ptr_atomic.h>

using namespace std;

class bar
{
public:
    bar() : num(-1)
    {
        v.clear();
    }
    explicit bar(vector<int> &in) : num(-1)
    {
        for (auto iter : in)
            v.push_back(iter);
    }
    int get_num()
    {
        for (auto iter : v)
        {
            num = max(iter, num);
        }
        //usleep(10);
        if (num >= 100)
        {
            printf("Fault %d\n", num);
        }
        return num;
    }
    void update(vector<int> &in)
    {
        for (auto iter : in)
            v.push_back(iter);
    }

private:
    int num;
    vector<int> v;
};

atomic<bool> flag;
shared_ptr<bar> now_using, ptr_0, ptr_1, ptr_2;
int max_read_times = 1e7, max_write_times = 200;
int st;

void get_func()
{
    for (int k = 0; k <= max_read_times; k++)
    {
        auto temp =
            atomic_load_explicit(&now_using, std::memory_order_seq_cst);
        temp->get_num();
    }
}

void set_func()
{
    for (int k = 0; k <= max_write_times; k++)
    {
        if (k % 3 == 0)
        {
            std::atomic_exchange(&now_using, ptr_0);
        }
        if (k % 3 == 1)
        {
            std::atomic_exchange(&now_using, ptr_1);
        }
        if (k % 3 == 2)
        {
            std::atomic_exchange(&now_using, ptr_2);
        }
    }
}

int main()
{
    srand((unsigned)time(NULL));
    vector<int> v0 = {2, 1, 3};
    vector<int> v1 = {1, 2, 3, 4, 5};
    vector<int> v2 = {};
    ptr_0 = std::make_shared<bar>(v0);
    ptr_1 = std::make_shared<bar>(v1);
    ptr_2 = std::make_shared<bar>(v2);
    now_using = ptr_0;
    std::thread t1(get_func);
    std::thread t2(set_func);
    t2.join();
    t1.join();
}

// g++ atomic_exchange.cpp -o atomic_exchange -pthread && ./atomic_exchange > atomic_exchange.out
