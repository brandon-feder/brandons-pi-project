#include <sys/time.h>
#include <thread>

#include "debug.h"
#include "chunky-fio.h"

// 1.6GB/s for NVME

#define CHUNKY_FIO_DEBUG_MODE 1

// Small Test
// const uint32_t n_chunks = 16;
// const uint32_t n_files_per_chunk = 32;
// const size_t file_size = 1<<15;
// const uint32_t n_chunks_at_once = 4;

// Large Test
const uint32_t n_chunks = 128;
const uint32_t n_files_per_chunk = 256;
const size_t file_size = 1<<20;
const uint32_t n_chunks_at_once = 12;


static_assert(file_size%4 == 0);

const size_t chunk_size = file_size*n_files_per_chunk;
const size_t buf_size =  chunk_size*n_chunks_at_once;
const uint32_t n_files = n_chunks * n_files_per_chunk;

int * fds;
uint8_t *buf;

void test_chunky_fio_write(ChunkyFIOHandler &C)
{
    // Queue up the first few writes

    // Saturate cache
    for(uint32_t i = 0; i < n_chunks/2; ++i)
    {
        uint8_t *sec = C.assign_buf_section(i);
        memset(sec, i, chunk_size);
        C.queue_write(i);
        C.mark_done(i);
        printf("%s completed chunk %d/%d\n", SUC, i, n_chunks);
    }    

    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    for(uint32_t i = n_chunks/2; i < n_chunks; ++i)
    {
        uint8_t *sec = C.assign_buf_section(i);
        memset(sec, i, chunk_size);
        C.queue_write(i);
        C.mark_done(i);
        printf("%s completed chunk %d/%d (timed)\n", SUC, i, n_chunks);
    }

    gettimeofday(&tp, NULL);
    uint64_t end = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    printf("took %lus to write %luGB; %lfGB/s\n", 
        (end-start)/1000, 
        (chunk_size*(n_chunks - n_chunks/2))/1000000,
        (double)0.000001*chunk_size*(n_chunks - n_chunks/2)/(end-start)
    );
}

int main()
{
    test_env_vars("BPP_TEST_DIR_PATH");

    printf("%s started ChunkyFIO test\n", SUC);
    printf(
        "%s # chunks: %d\n\
        %s # files/chunk: %d\n\
        %s file size: %ld\n\
        %s total size: %ldGB\n",
        INF, n_chunks, 
        INF, n_files_per_chunk,
        INF, file_size, 
        INF, n_files_per_chunk*file_size*n_chunks/1000000000
    );

    // ========= CREATE TEST FILES =========
    create_n_test_files(n_files, file_size);
    printf("%s created test files\n", SUC);

    // ========== OPEN TEST FILES ==========
    fds = new int[n_files];
    try {
        c_open_test_files(n_files, fds);
    } catch(...) {
        c_close_files(n_files, fds);
        delete fds;
        throw;
    }

    // ====== CREATE ChunkyFIO Object ======
    buf = new uint8_t[buf_size];
    ChunkyFIOHandler C(
        fds,
        n_files, file_size, n_files_per_chunk, 
        buf, buf_size
    );
    printf("%s created ChunkyFIOHandler Object\n", SUC);

    try
    {
        test_chunky_fio_write(C);
    } catch(...)
    {
        printf("%s error writing to files\n", ERR);
        c_close_files(n_files, fds);
        delete fds;
        delete buf;
        throw;
    }
    printf("%s wrote to files\n", SUC);

    // ============= CLEAN UP ==============
    c_close_files(n_files, fds);
    delete fds;
    delete buf;
}