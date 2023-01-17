#ifndef __CHUNKY_FIO__
#define __CHUNKY_FIO__

#include <liburing.h>
#include <stdexcept>
#include <vector>
#include <string.h>
#include <list>
#include <map>
#include <algorithm>
#include <queue>
#include <mutex>

using namespace std;

#define MAX_U32 !(uint32_t)0

#define OP_READ 0
#define OP_READ_FIXED 1

#define OP_WRITE 2
#define OP_WRITE_FIXED 3

#define OP_QUIT 4

#define USE_SQPOLL true

// struct to hold submission data
struct SubData
{
    uint32_t tries; // Number of tries to perform operation before failing
    int fd; // File descriptor to read data from
    uint32_t buf_indx; // The index of that buffer
    iovec iov; // tells where to perform operation to/from
    uint8_t op; // the actual operation being performed
    void *data_ptr; // pointer to user defined data
};

class ParallelFIO
{
private:
    io_uring *const rings; // array of rings; one per thread
    const uint32_t n_threads; // number of threads
    const uint32_t entries; // number of entries for a ring per thread
    const uint32_t idle_time; // time to idle if USE_SQPOLL is true

    queue<SubData *> sub_queue; // the submission queue
    mutex sub_mutex; // mutex used 

    queue<SubData *> comp_queue;

    void sync_completions();

public:
    ParallelFIO(uint32_t n_threads_, uint32_t entries_, uint32_t idle_time_);
    ~ParallelFIO();

    void register_buffer(void *buf, size_t len);

    // submission functions
    void prepare_submit();
    void queue(SubData *s_data);
    void submit();

    // completion functions
    SubData *get_completion();
};

#endif