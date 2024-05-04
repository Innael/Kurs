#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <windows.h>


class safe_queue {
public:
    std::queue<std::function<void()>> f_queue;
    std::mutex safe_queue_mutex;
    std::condition_variable sq_convar;
    std::atomic<bool> noty = false;
     

    void push(std::function<void()> func) {
        std::lock_guard<std::mutex> lk(safe_queue_mutex);
        f_queue.push(func);
        //std::cout << "Функция помещена в очередь\n";
        noty = true;
        sq_convar.notify_one(); 
    }

    std::function<void()> pop() {
        std::function<void()> temp;
        std::unique_lock<std::mutex> ulk(safe_queue_mutex);       
        sq_convar.wait(ulk, [&] { return noty == true; });
        temp = f_queue.front();
        f_queue.pop();
        if (f_queue.empty() == true) noty = false;       
        return temp;
    }

};


class thread_pool {
public:
    std::vector<std::thread> tr_vec;
    safe_queue tp_sq;
    int core_count = 1;
    std::atomic<int> pool = 1;
    std::atomic<bool> ready = false;
    std::atomic<bool> stop = false;

    thread_pool() {
        core_count = std::thread::hardware_concurrency();
        pool = core_count;
        for (int i = 0; i < pool ; ++i) {
            tr_vec.push_back(std::thread(&thread_pool::work, this));            
            //std::cout << "Поток создан\n";
        }
        ready = true;
    }

    void work() {      
        while (!ready) {
            std::this_thread::yield();
        }
        for (int i = 0; i <2;) {                
               if (tp_sq.f_queue.empty() == false) {
                    tp_sq.pop()();
                    std::cout << "Поток " << std::this_thread::get_id << " обработал функцию\n\n";                                                 
                }
               Sleep(1000);
                
               if (tp_sq.f_queue.empty() == true && stop == true) i = 2;                
         }        
    }

    void submit(std::function<void()> func) {
        tp_sq.push(func);
    }


    ~thread_pool() {
        for (auto& elem : tr_vec) {
            if (elem.joinable()) {
                elem.join();
            }
        }
    }
};

void func1() {
    Sleep(500);
    std::cout << "Функция " << __FUNCTION__ << " завершила работу\n";
}

void func2() {
    Sleep(1500);
    std::cout << "Функция " << __FUNCTION__ << " завершила работу\n";
}

void func3() {
    Sleep(2000);
    std::cout << "Функция " << __FUNCTION__ << " завершила работу\n";
}

void func4() {
    Sleep(2500);
    std::cout << "Функция " << __FUNCTION__ << " завершила работу\n";
}

int main()
{
    SetConsoleCP (1251);
    SetConsoleOutputCP(1251);        

    std::cout << "Поток " << std::this_thread::get_id << " является основным потоком.\n\n";
    std::function<void()> f1(func1);
    std::function<void()> f2(func2);
    std::function<void()> f3(func3);
    std::function<void()> f4(func4);    



    std::vector<std::function<void()>> vf{f1, f2, f3, f4, f1, f3, f2, f3, f4, f4, f2, f1};       

    {
        thread_pool th_pool;

        safe_queue sq;
        for (int i = 0; i < 12; i += 2) {
            Sleep(1000);
            th_pool.submit(vf[i]);
            th_pool.submit(vf[i + 1]);
        }
        th_pool.stop = true;
    }

    return 0;
}

