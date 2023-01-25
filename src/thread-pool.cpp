#include "thread-pool.h"

using namespace std;

ThreadPool::ThreadPool(uint32_t max_threads_, useconds_t sleep_time_) :
max_threads(max_threads_),
sleep_time(sleep_time_),
threads((jthread *)malloc(sizeof(jthread)*max_threads))
{
    for(uint32_t i = 0; i < max_threads; ++i)
        threads[i] = jthread(&ThreadPool::worker, this, i);
}

ThreadPool::ThreadPool(uint32_t max_threads_) :
ThreadPool::ThreadPool(max_threads_, DEFAULT_SLEEP_TIME) {}

ThreadPool::~ThreadPool() 
{
    quit_threads = true;
    for(uint32_t i = 0; i < max_threads; ++i)
        threads[i].join();

    free(threads);
}

void ThreadPool::submit(function<void(void *)> func, void *data, uint64_t sub_id)
{
    SubData new_s_data;
    new_s_data.func = func;
    new_s_data.data = data;
    new_s_data.sub_id = sub_id;

    sub_mutex.lock();
    sub_queue.push(new_s_data);
    sub_completion[sub_id] += 1;
    sub_mutex.unlock();
}

void ThreadPool::complete_all()
{
    while(sub_completion.size() != 0)
        wait_for_any({sub_completion.begin()->first});
}

size_t ThreadPool::n_subs()
{
    size_t size = sub_queue.size();
    return size;
}

void ThreadPool::worker(uint32_t thr_id)
{
    while(true)
    {
        if(quit_threads)
            break;

        // Get the next submission
        sub_mutex.lock();
        if(sub_queue.size() == 0) // queue is empty, sleep for some time
        {
            sub_mutex.unlock();
            usleep(sleep_time);
            continue;
        } else // there is something to do
        {
            SubData s_data = sub_queue.front();
            sub_queue.pop();
            sub_mutex.unlock();

            // Call the function (blocks until completion)
            s_data.func(s_data.data);

            // Mark completed
            comp_mutex.lock();
            sub_completion[s_data.sub_id] -= 1;
            comp_mutex.unlock();

            // Notify threads waiting for a change in the completion set
            comp_cond_var.notify_all();
        }
    }
}

uint64_t ThreadPool::wait_for_any(vector<uint64_t> sub_ids)
{
    // Wait for something to happen and sub_id to be in set
    unique_lock lk(comp_mutex);
    uint64_t completed_sub_id;
    comp_cond_var.wait(lk, [&]{
        for(vector<uint64_t>::iterator it = sub_ids.begin(); it != sub_ids.end(); ++it) {
            if(sub_completion[*it] == 0) {
                completed_sub_id = *it;
                return true;
            }
        }

        return false;
    });

    sub_completion.erase(completed_sub_id);

    lk.unlock();

    return completed_sub_id;
}