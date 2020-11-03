#include <bits/stdc++.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <unistd.h>
#include <bits/shared_ptr_atomic.h>
#include "concurrentqueue/concurrentqueue.h"

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
        printf("Max num: %d\n", num);
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
    void clear()
    {
        int t;
        while (queue.try_dequeue(t))
            ;
    }

private:
    moodycamel::ConcurrentQueue<int> queue;
};

atomic<int> state;
shared_ptr<bar> ptr_0, ptr_1, ptr_2;
atomic<int> using_cnt;
int max_get_times = 1e3;
int max_set_times = 1e5;
int max_reset_times = 200;
int st;

void get_func()
{
    usleep(max_set_times / (max_get_times * 60));
    for (int k = 0; k <= max_get_times; k++)
    {
        switch (state.load())
        {
        case 0:
            ptr_0->get_num();
            state.store(1);
            break;
        case 1:
            ptr_1->get_num();
            state.store(2);
            break;
        case 2:
            state.store(0);
            break;
        }
    }
}

void set_func()
{
    for (int k = 0; k <= max_set_times; k++)
    {
        vector<int> temp;
        temp.clear();
        int cnt = rand() % 10 + 1;
        for (int i = 0; i < cnt; i++)
        {
            temp.push_back(rand() % 100);
        }
        switch (state.load())
        {
        case 0:
            ptr_0->update(temp);
            break;
        case 1:
            ptr_1->update(temp);
            break;
        case 2:
            break;
        }
    }
}

void reset_func()
{
    usleep(max_set_times / (max_reset_times * 60));
    for (int k = 0; k <= max_reset_times; k++)
    {
        state.store(2);
        ptr_0->clear();
        ptr_1->clear();
        state.store(0);
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
    std::thread t3(reset_func);
    std::thread t4(set_func);
    t4.join();
    t3.join();
    t2.join();
    t1.join();
}

// g++ mini_hotkey.cpp -o mini_hotkey -pthread -fsanitize=thread -fPIE -pie -g -std=c++11 && ./mini_hotkey
