#ifndef __CHUNKY_FIO__
#define __CHUNKY_FIO__

#include <stdint.h>
#include <vector>
#include <utility>
#include <sys/mman.h>
#include <functional>
#include <map>
#include <list>
#include <mutex>

#include "debug.h"
#include "thread-pool.h"

#define STATUS_RESERVED 0
#define STATUS_UNRESERVED 1
#define MAP_HUGE_2MB (21 << MAP_HUGE_SHIFT)

// FUTURE: Implement another thread to update section statuses

#define NO_MULTITHREAD true

class ChunkyFIO
{
private:
    const uint64_t n_threads;
    const vector<int> fds;

    const uint64_t n_chunks;
    const size_t chunk_size;
    const uint64_t chunks_at_once;

    const uint64_t n_blocks_per_chunk;
    const size_t block_size;

    // these are set once in constructor than never changed
    unordered_map<void *, uint64_t> sec_id_frwd;
    unordered_map<uint64_t, void *> sec_id_bkwd;

    ThreadPool thread_pool;
    function<pair<int, uint64_t>(uint64_t)> block_loc;
    
    // map from status to set of chunks with that status
    map<int, unordered_set<void *>> sec_statuses;
    mutex sec_stat_mut;

    // the mmapped section associated with each chunk
    unordered_map<uint64_t, void *> chunk_to_sec;
    mutex chunk_sec_mut;

    // toggles a sections status
    void toggle_sec_stat(void *ptr);

    // waits for an unreserved section to finish. 
    // Then marks it reserved
    void *wait_reserve_sec();
public:
    ChunkyFIO(
        uint64_t n_threads_, vector<int> fds_, 
        uint64_t n_chunks_, uint64_t chunks_at_once_,
        uint64_t n_blocks_per_chunk_, size_t block_size_,
        function<pair<int, off_t>(uint64_t)> block_loc_
    );
    ~ChunkyFIO();

    // wait for a section to be ready, then start
    // mapping stuff into the chunk
    void map_chunk(uint64_t chunk_id);

    // start unmapping an already mapped chunk
    void unmap_chunk(uint64_t chunk_id);

    // wait for a mapped chunk to be ready
    // returns a pointer to the chunk's section
    void *wait_for_map(uint64_t chunk_id);

    void wait_all();
};

#endif