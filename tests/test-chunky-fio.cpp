#include <sys/time.h>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "debug.h"
#include "chunky-fio.h"

// threads/chunks at once
// 1/4: 521, 324
// 1/8: 529, 733
//      

// 2/4: 748, 466
// 2/8: 502, 455

#define CHUNKY_FIO_DEBUG_MODE 1
#define USE_MANY_THREADS true

#define TEST_WRITE true
#define TEST_READ false

#define N_DRIVES 4

using namespace std;

void test_write(
    int *fds, uint32_t n_files, 
    size_t file_size, uint32_t n_files_per_chunk, 
    uint8_t *buf, size_t buf_size,
    uint32_t n_chunks, size_t chunk_size
);

void test_read(
    int *fds, uint32_t n_files, 
    size_t file_size, uint32_t n_files_per_chunk, 
    uint8_t *buf, size_t buf_size,
    uint32_t n_chunks, size_t chunk_size
);

int main()
{
    // =============================================================
    // ==================== IMPORTANT VARIABLES ====================
    // =============================================================

    #if USE_MANY_THREADS
    const uint32_t n_threads = 4;
    #else
    const uint32_t n_threads = 1;
    #endif

    const string dirs[4] = {
        "/home/koala/Documents/brandons-pi-program/data/A",
        "/home/koala/Documents/brandons-pi-program/data/B",
        "/home/koala/Documents/brandons-pi-program/data/C",
        "/home/koala/Documents/brandons-pi-program/data/D"
    };

    // all of these variables are per thread
    const uint32_t n_chunks =  64/n_threads;
    const uint32_t n_files_per_chunk = 512;
    const size_t file_size = 1<<20;
    const uint32_t n_chunks_at_once = 4;

    const size_t chunk_size = file_size*n_files_per_chunk;
    const size_t buf_size =  chunk_size*n_chunks_at_once;
    const uint32_t n_files = n_chunks * n_files_per_chunk;

    static_assert(!(chunk_size%N_DRIVES));

    int *fds;
    uint8_t *buf;
    thread test_threads[n_threads];

    // =============================================================
    // ======================= INITIAL SETUP =======================
    // =============================================================

    printf("%s started ChunkyFIO test\n", SUC);
    printf(
        "%s # threads: %d\n"
        "%s # chunks per thread: %d\n"
        "%s IO per thread: %f GB\n"
        "%s Total IO: %f GB\n",
        INF, n_threads,
        INF, n_chunks,
        INF, (float)file_size*n_files_per_chunk*n_chunks/1000000000,
        INF, (float)n_threads*file_size*n_files_per_chunk*n_chunks/1000000000
    );

    buf = new uint8_t[buf_size*n_threads];
    fds = new int[n_threads*n_files];

    // =============================================================
    // ===================== CREATE/OPEN FILES =====================
    // =============================================================

    try {
        for(uint32_t i = 0; i < n_files*n_threads; ++i)
        {
            fds[i] = open(
                str_fprintf("%s/%u.32b", dirs[i%N_DRIVES].c_str(), i).c_str(),
                O_RDWR | O_CREAT, 0644
            );

            if(fds[i] < 0)
                throw runtime_error(str_fprintf("Could not create/open test files: %s", strerror(errno)));
            
            if(ftruncate(fds[i], file_size) < 0)
                throw runtime_error(str_fprintf("Could not set file size: %s", strerror(errno)));
        }
    } catch(...) {
        for(uint32_t i = 0; i < n_files*n_threads; ++i)
            close(fds[i]);

        delete fds;
        delete buf;
        throw;
    }

    // =============================================================
    // =================== LAUNCH WRITE THREADS ====================
    // =============================================================
    
    timeval tp;
    uint64_t start, end, write_time, read_time, read_write_time;

    #if TEST_WRITE
    // struct timeval tp;

    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    // Create and launch threads
    try {
    #if USE_MANY_THREADS
        for(uint32_t i = 0; i < n_threads; ++i) {
            test_threads[i] = thread(
                test_write, 
                fds + i*n_files, n_files, 
                file_size, n_files_per_chunk, 
                buf + i*buf_size, buf_size,
                n_chunks, chunk_size
            );
        }

        printf("%s launched all threads\n", SUC);

        // Join threads back together
        for(uint32_t i = 0; i < n_threads; ++i) {
            test_threads[i].join();
        }
        printf("%s joined all threads\n", SUC);
    #else
        test_write(
            fds, n_files, 
            file_size, n_files_per_chunk, 
            buf, buf_size,
            n_chunks, chunk_size
        );
    #endif
    } catch(...) {
        for(uint32_t i = 0; i < n_files*n_threads; ++i)
            close(fds[i]);
        delete fds;
        delete buf;
        throw;
    }

    gettimeofday(&tp, NULL);
    end = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    
    write_time = end - start;
    #endif

    // =============================================================
    // =================== LAUNCH READ THREADS =====================
    // =============================================================

    #if TEST_READ
    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    // Create and launch threads
    try {
    #if USE_MANY_THREADS
        for(uint32_t i = 0; i < n_threads; ++i) {
            test_threads[i] = thread(
                test_read, 
                fds + i*n_files, n_files, 
                file_size, n_files_per_chunk, 
                buf + i*buf_size, buf_size,
                n_chunks, chunk_size
            );
        }

        printf("%s launched all threads\n", SUC);

        // Join threads back together
        for(uint32_t i = 0; i < n_threads; ++i) {
            test_threads[i].join();
        }
        printf("%s joined all threads\n", SUC);
    #else
        test_read(
            fds, n_files, 
            file_size, n_files_per_chunk, 
            buf, buf_size,
            n_chunks, chunk_size
        );
    #endif
    } catch(...) {
        for(uint32_t i = 0; i < n_files*n_threads; ++i)
            close(fds[i]);
        delete fds;
        delete buf;
        throw;
    }

    gettimeofday(&tp, NULL);
    end = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    read_time = end - start;
    #endif

    // =============================================================
    // ================ LAUNCH READ/WRITE THREADS ==================
    // =============================================================

    // gettimeofday(&tp, NULL);
    // start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    // // Create and launch threads
    // try {
    // #if USE_MANY_THREADS
    //     for(uint32_t i = 0; i < n_threads; ++i) {
    //         test_threads[i] = thread(
    //             test_read, 
    //             fds + i*n_files, n_files, 
    //             file_size, n_files_per_chunk, 
    //             buf + i*buf_size, buf_size,
    //             n_chunks, chunk_size
    //         );
    //     }

    //     printf("%s launched all threads\n", SUC);

    //     // Join threads back together
    //     for(uint32_t i = 0; i < n_threads; ++i) {
    //         test_threads[i].join();
    //     }
    //     printf("%s joined all threads\n", SUC);
    // #else
    //     test_read(
    //         fds, n_files, 
    //         file_size, n_files_per_chunk, 
    //         buf, buf_size,
    //         n_chunks, chunk_size
    //     );
    // #endif
    // } catch(...) {
    //     for(uint32_t i = 0; i < n_files*n_threads; ++i)
    //         close(fds[i]);
    //     delete fds;
    //     delete buf;
    //     throw;
    // }

    // gettimeofday(&tp, NULL);
    // end = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    // read_write-time = end - start;

    // =============================================================
    // ========================= SUMMARIZE =========================
    // =============================================================

    #if TEST_WRITE
    printf(
    "%s total write time: %lds\n"
    "%s thread write throughput: %fGB/s\n"
    "%s total write throughput: %fGB/s\n\n",
        INF, write_time/1000,
        INF, (float) (n_chunks*chunk_size) / (1000000*write_time),
        INF, (float) (n_threads*n_chunks*chunk_size) / (1000000*write_time)
    );
    #endif

    #if TEST_READ
    printf(
    "%s total read time: %lds\n"
    "%s thread read throughput: %fGB/s\n"
    "%s total read throughput: %fGB/s\n",
        INF, read_time/1000,
        INF, (float) (n_chunks*chunk_size) / (1000000*read_time),
        INF, (float) (n_threads*n_chunks*chunk_size) / (1000000*read_time)
    );
    #endif

    // printf(
    // "%s total read/write time: %lds\n"
    // "%s thread read/write throughput: %fGB/s\n"
    // "%s total read/write throughput: %fGB/s\n",
    //     INF, read_time/1000,
    //     INF, (float) 2*(n_chunks*chunk_size) / (1000000*read_time),
    //     INF, (float) 2*(n_threads*n_chunks*chunk_size) / (1000000*read_time)
    // );

    // =============================================================
    // ========================= CLEAN UP ==========================
    // =============================================================

    for(uint32_t i = 0; i < n_files*n_threads; ++i)
        close(fds[i]);
    delete fds;
    delete buf;
}

void test_write(
    int *fds, uint32_t n_files, 
    size_t file_size, uint32_t n_files_per_chunk, 
    uint8_t *buf, size_t buf_size,
    uint32_t n_chunks, size_t chunk_size
) 
{
    ChunkyFIO C(fds, n_files, file_size, n_files_per_chunk, buf, buf_size);

    printf("%s starting writes\n", INF);
    for(uint32_t i = 0; i < n_chunks; ++i)
    {
        printf("%s writing %d\n", INF, i);

        uint8_t *sec = C.assign_buf_section(i);
        memset(sec, 1, chunk_size);

        C.queue_write(i);
        C.mark_done(i);
    } 
}

void test_read(
    int *fds, uint32_t n_files, 
    size_t file_size, uint32_t n_files_per_chunk, 
    uint8_t *buf, size_t buf_size,
    uint32_t n_chunks, size_t chunk_size
) 
{
    ChunkyFIO C(fds, n_files, file_size, n_files_per_chunk, buf, buf_size);

    printf("%s starting reads\n", INF);
    for(uint32_t i = 0; i < n_chunks; ++i)
    {
        printf("%s reading chunk %d\n", INF, i);
        uint8_t *sec = C.assign_buf_section(i);
        C.queue_read(i);
        
        // for(uint32_t j = 0; j < chunk_size/4; ++j)
        //     assert(sec[j] == 1);

        C.mark_done(i);
    } 
}
