#include "chunky-fio.h"

void ChunkyFIO::toggle_sec_stat(void *ptr)
{
    sec_stat_mut.lock();
    if(sec_statuses[STATUS_RESERVED].count(ptr) == 1) {
        sec_statuses[STATUS_RESERVED].erase(ptr);
        sec_statuses[STATUS_UNRESERVED].insert(ptr);
    } else {
        sec_statuses[STATUS_UNRESERVED].erase(ptr);
        sec_statuses[STATUS_RESERVED].insert(ptr);
    }
    sec_stat_mut.unlock();
}

void *ChunkyFIO::wait_reserve_sec()
{
    sec_stat_mut.lock();

    // If waiting will hang forever, throw error
    if(sec_statuses[STATUS_UNRESERVED].size() == 0) {
        sec_stat_mut.unlock();
        throw runtime_error("No sections are marked unreserved.");
    }

    // wait for an unreserved section to be ready for reservation
    uint64_t i = 0;
    vector<uint64_t> jobs;
    jobs.resize(sec_statuses[STATUS_UNRESERVED].size());
    for(auto sec : sec_statuses[STATUS_UNRESERVED]) {
        jobs[i] = sec_id_frwd[sec];
        ++i;
    }
    sec_stat_mut.unlock();
    uint64_t sub_id = thread_pool.wait_for_any(jobs);

    // get the sections pointer
    void *res = sec_id_bkwd[sub_id];

    // mark it as reserved
    toggle_sec_stat(res);

    return res;
}

ChunkyFIO::ChunkyFIO(
    uint64_t n_threads_, vector<int> fds_, 
    uint64_t n_chunks_, uint64_t chunks_at_once_,
    uint64_t n_blocks_per_chunk_, size_t block_size_,
    function<pair<int, off_t>(uint64_t)> block_loc_
) :
n_threads(n_threads_),
fds(fds_),
n_chunks(n_chunks_),
chunks_at_once(chunks_at_once_),
n_blocks_per_chunk(n_blocks_per_chunk_),
block_size(block_size_),
block_loc(block_loc_),
chunk_size(block_size*n_blocks_per_chunk),
thread_pool(ThreadPool(n_threads))
{
    // reserve areas of virtual mem to use for mem-maping
    void *addr;
    for(uint64_t i = 0; i < chunks_at_once; ++i) {
        addr = mmap(NULL, block_size*n_blocks_per_chunk, 
            PROT_WRITE | PROT_READ, 
            MAP_PRIVATE | MAP_ANONYMOUS, 
            -1, 0
        );

        if(addr == MAP_FAILED)
            throw runtime_error(str_fprintf(
                "mmap failed: %s",
                strerror(errno)
            ));

        printf("%s start, end: %lu, %lu\n", 
            INF,
            (uint64_t)addr, 
            (uint64_t)((uint8_t *)addr + block_size*n_blocks_per_chunk)
        );


        sec_statuses[STATUS_UNRESERVED].insert(addr);
        sec_id_frwd[addr] = i;
        sec_id_bkwd[i] = addr;
    }
}

ChunkyFIO::~ChunkyFIO() 
{
    thread_pool.complete_all();

    // unmap all reserved areas of virtual memory
    for(auto sec : sec_id_bkwd)
        munmap(sec.second, chunk_size);
}

void ChunkyFIO::map_chunk(uint64_t chunk_id)
{
    // get the part of virtual-memory that the chunk can use
    void *sec = wait_reserve_sec();

    // assign section to that chunk
    chunk_sec_mut.lock();
    chunk_to_sec[chunk_id] = sec; 
    chunk_sec_mut.unlock();

    // for every block in the chunk
    for(uint64_t i = 0; i < n_blocks_per_chunk; ++i) {
        // get the blocks file descriptor and offset
        uint64_t block_id = chunk_id*n_blocks_per_chunk + i;
        pair<int, uint64_t> block = block_loc(block_id);
        int fd = block.first;
        off_t offset = block.second*block_size;

        // queue the block's memory mapping
        madvise(sec, chunk_size, MADV_SEQUENTIAL | MADV_WILLNEED );
        thread_pool.submit([=, this](void *data) {
            void *ptr = mmap(
                (void *)((uint8_t *)sec + i*block_size), block_size, 
                PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_FIXED | MAP_POPULATE,
                fd, offset
            );

            if(ptr == MAP_FAILED)
                throw runtime_error(
                    str_fprintf("mmap failed: %s", strerror(errno))
                );
        }, NULL, sec_id_frwd[sec]);
    }
}

void ChunkyFIO::unmap_chunk(uint64_t chunk_id)
{
    if(chunk_id >= n_chunks)
        throw runtime_error("Invalid chunk id.");

    // get the chunks mem-mapped section
    chunk_sec_mut.lock();
    void *sec = chunk_to_sec[chunk_id];
    chunk_sec_mut.unlock();

    // queue each block to be synced
    // thread_pool.submit([=, this](void *data) {
    //     for(uint64_t i = 0; i < n_blocks_per_chunk; ++i) {
    //         msync((void *)((uint8_t *)sec + i*block_size), block_size, MS_SYNC);
    //     }
    // }, NULL, sec_id_frwd[sec]); // only first 52-bits are in use for memory addr.

    toggle_sec_stat(sec);
}

void *ChunkyFIO::wait_for_map(uint64_t chunk_id)
{
    // get the chunks section
    chunk_sec_mut.lock();
    void *sec = chunk_to_sec[chunk_id];
    chunk_sec_mut.unlock();

    // wait for jobs
    vector<uint64_t> jobs = {sec_id_frwd[sec]};
    thread_pool.wait_for_any(jobs);

    return sec;
}

void ChunkyFIO::wait_all()
{
    thread_pool.complete_all();
}