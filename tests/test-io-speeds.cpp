#include <stdio.h>
#include <future>
#include <cstdlib>
#include <condition_variable>
#include <list>
#include <fcntl.h>
#include <string>
#include <cstring>
#include <exception>
#include <unordered_set>
#include <unordered_map>
#include <sys/mman.h>
#include <list>
#include <string>
#include <sys/time.h>
#include <valgrind/callgrind.h>
#include <assert.h>

#include "debug.h"
#include "thread-pool.h"
#include "chunky-fio.h"

using namespace std;

const uint32_t max_threads = 1;

const string devices[1] = {
    "/dev/sdb"
};

const uint32_t n_devices = 1;
const uint32_t n_threads = 1;
const uint32_t n_chunks = 64;
const uint32_t chunks_at_once = 1;
const uint32_t n_blocks_per_chunk = 1;
const size_t block_size = 32*1024*getpagesize();

void test_read_write(ChunkyFIO &C);

int main()
{
    printf("%s started io speed test\n", SUC);
    printf("%s # threads: %u\n", INF, n_threads);
    printf("%s # chunks: %d\n", INF, n_chunks);
    printf("%s page size: %d\n", INF, getpagesize());
    printf("%s chunk size: %fGB\n", 
        INF, (float)block_size*n_blocks_per_chunk/1e9
    );
    printf("%s total size: %fGB\n", 
        INF, (float)block_size*n_chunks*n_blocks_per_chunk/1e9
    );

    // ============================================
    // ================ Open File =================
    // ============================================
    
    vector<int> fds;
    fds.resize(n_devices);
    uint32_t i, j;

    try {
        for(i = 0; i < n_devices; ++i) {
            fds[i] = open(devices[i].c_str(), O_RDWR | O_DIRECT | O_SYNC);
            if(fds[i] == -1)
                throw runtime_error(str_fprintf(
                    "Could not open file %s: %s\n",
                    devices[i].c_str(),
                    strerror(errno)
                ));

            printf("%s opened %s\n", SUC, devices[i].c_str());
            printf(
                "%s%s has size %.5gTB\n", 
                TAB, INF, (float)get_file_size(fds[i])/1e9
            );
        
            ++i;
        }
    } catch(...) {
        for(uint32_t j = 0; j < i; ++j)
            close(fds[i]);

        throw;
    }

    //============================================
    //========= Create ChunkyFIO Object ==========
    //============================================

    ChunkyFIO C(
        n_threads, fds, 
        n_chunks, chunks_at_once,
        n_blocks_per_chunk, block_size,
        [=](uint64_t block_id) {
            return pair<int, off_t>(
                fds[block_id%n_devices], 
                block_id/n_devices
            ); 
        }
    );

    // ============================================
    // ============== Start Reading! ==============
    // ============================================

    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t start = tp.tv_sec*1000 + tp.tv_usec / 1000;
    test_read_write(C);
    gettimeofday(&tp, NULL);
    uint64_t end = tp.tv_sec*1000 + tp.tv_usec / 1000;

    printf("throughput: %fMB/s\n", 
        ((float)block_size*n_chunks*n_blocks_per_chunk/1e6)/
        ((float)(end-start)/1000)
    );

    // ============================================
    // ================= Clean Up =================
    // ============================================

    // for(i = 0; i < n_devices; ++i)
    //     close(fds[i]);
}

void test_read_write(ChunkyFIO &C)
{
    uint64_t i;
    for(i = 0; i < chunks_at_once; ++i)
        C.map_chunk(i);

    for(i = 0; i < n_chunks; ++i)
    {
        printf("%s read/writing chunk %lu\n", INF, i);

        if(i >= chunks_at_once)
            C.map_chunk(i);
        
        uint8_t *ptr = (uint8_t *)C.wait_for_map(i);

        printf("%s %u\n", TAB, (uint32_t)ptr[0]);

        memset(ptr, i, block_size*n_blocks_per_chunk);

        for(uint32_t j = 0; j < block_size; ++j)
        {
            // assert(ptr[i] == i);
        }

        C.unmap_chunk(i);
    }

    C.wait_all();
}