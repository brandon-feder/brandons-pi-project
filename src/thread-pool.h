#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <stdint.h>
#include <queue>
#include <future>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <unordered_set>

using namespace std;

#define DEFAULT_SLEEP_TIME 50000 // in microseconds

class ThreadPool
{
private:
    struct SubData
    {
        function<void(void *)> func;
        uint64_t sub_id;
        void *data;
    };
    
    const uint32_t max_threads;
    const useconds_t sleep_time;

    queue<SubData> sub_queue;
    mutex sub_mutex;

    mutex comp_mutex;
    unordered_map<uint64_t, uint64_t> sub_completion;
    condition_variable comp_cond_var;

    atomic_bool quit_threads = false;

    jthread *threads;

    void worker(uint32_t thr_id);

public:
    ThreadPool(uint32_t max_threads_);
    ThreadPool(uint32_t max_threads_, useconds_t sleep_time_);
    ~ThreadPool();
    
    void submit(function<void(void *)> func, void *data, uint64_t sub_id);
    uint64_t wait_for_any(vector<uint64_t> sub_ids);
    void complete_all();

    size_t n_subs();
    size_t n_comps();
};

#endif