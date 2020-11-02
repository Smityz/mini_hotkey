#include <bits/stdc++.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <unistd.h>
#include <bits/shared_ptr_atomic.h>
#include "readerwriterqueue/readerwriterqueue.h"

using namespace std;

class bar
{
public:
    int get_num()
    {
        int num = -1;
        int sz = 5;
        int t = 0;
        while (queue.try_dequeue(t) && (--sz))
        {
            num = max(t, num);
        }
        //usleep(10);
        if (num >= 100 || num < -1)
        {
            printf("Fault %d\n", num);
        }
        return num;
    }
    void update(vector<int> &in)
    {
        for (auto iter : in)
            queue.try_enqueue(iter);
    }

private:
    moodycamel::ReaderWriterQueue<int> queue;
};

atomic<int> state;
shared_ptr<bar> ptr_0, ptr_1, ptr_2;
atomic<int> using_cnt;
int max_read_times = 1e5, max_write_times = 1e5;
int st;

void get_func()
{
    for (int k = 0; k <= max_read_times; k++)
    {
        switch (state.load())
        {
        case 0:
            ptr_0->get_num();
            state.store(1);
            return;
        case 1:
            ptr_1->get_num();
            state.store(2);
            return;
        case 2:
            ptr_2->get_num();
            state.store(0);
            return;
        }
    }
}

void set_func()
{
    for (int k = 0; k <= max_write_times; k++)
    {
        vector<int> temp;
        temp.clear();
        int cnt = rand() % 10;
        for (int i = 0; i < cnt; i++)
        {
            temp.push_back(rand() % 100);
        }
        switch (state.load())
        {
        case 0:
            ptr_0->update(temp);
            return;
        case 1:
            ptr_1->update(temp);
            return;
        case 2:
            ptr_2->update(temp);
            return;
        }
    }
}

int main()
{
    srand((unsigned)time(NULL));
    ptr_0 = std::make_shared<bar>();
    ptr_1 = std::make_shared<bar>();
    ptr_2 = std::make_shared<bar>();
    std::thread t1(get_func);
    std::thread t2(set_func);
    t2.join();
    t1.join();
}

// g++ mini_hotkey.cpp -o mini_hotkey -pthread && ./atomic_exchange > atomic_exchange.out
