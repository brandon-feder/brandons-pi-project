#include "chunky-fio.h"
#include "debug.h"

// https://github.com/axboe/liburing/issues/571#issuecomment-1106480309
// Use IORING_SETUP_ATTACH_WQ
// Use O_DIRECT
// With/without IORING_SETUP_SQPOLL

void ParallelFIO::sync_completions()
{
    prepare_submit();

    // read from completion queues and resubmit if needed

    submit();
}

ParallelFIO::ParallelFIO(uint32_t n_threads_, uint32_t entries_, uint32_t idle_time_) :
n_threads(n_threads_),
entries(entries_),
idle_time(idle_time_),
rings(new io_uring[n_threads])
{
    io_uring_params ring_params;
    #if USE_SQPOLL
        ring_params.flags |= IORING_SETUP_SQPOLL;
        ring_params.sq_thread_idle = idle_time;
    #endif

    // Setup rings
    for(uint32_t i = 0; i < n_threads; ++i) {
        int r = io_uring_queue_init_params(entries, &rings[i], &ring_params);
        if(r < 0)
            throw runtime_error("Could not init io_uring queue!");
    }
}

ParallelFIO::~ParallelFIO()
{
    for(uint32_t i = 0; i < n_threads; ++i) {
        io_uring_unregister_buffers(&rings[i]);
        io_uring_queue_exit(&rings[i]);
    }
}

void ParallelFIO::register_buffer(void *buf, size_t len)
{
    iovec iov;
    iov.iov_base = buf;
    iov.iov_len = len;

    for(uint32_t i = 0; i < n_threads; ++i) {
        int r = io_uring_register_buffers(&rings[i], &iov, 1);
        if(r < 0)
            throw runtime_error("Could not init io_uring queue!");
    }
}

void ParallelFIO::prepare_submit()
{
    sub_mutex.lock();
}

void ParallelFIO::queue(SubData *s_data)
{
    sub_queue.push(s_data);
}

void ParallelFIO::submit()
{
    sub_mutex.unlock();
}

SubData *ParallelFIO::get_completion()
{
    if(comp_queue.size() == 0) // queue is empty
        sync_completions();
    
    if(comp_queue.size() == 0) // still empty even after syncing
        return NULL;
    else { // if not empty, return oldest completion
        SubData *comp = comp_queue.front();
        comp_queue.pop();
        return comp;
    }
}