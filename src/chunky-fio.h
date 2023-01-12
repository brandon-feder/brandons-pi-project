#ifndef __CHUNKY_FIO__
#define __CHUNKY_FIO__

#include <liburing.h>
#include <stdexcept>
#include <vector>
#include <string.h>
#include <list>
#include <map>
#include <algorithm>

using namespace std;

#define MAX_U32 !(uint32_t)0
#define MAX_COMPLETION_TRIES 3

#define OP_READ 0
#define OP_WRITE 1

// TODO: Look into using unique_pointer for sub data
class ChunkyFIO
{
private:
    struct SubData
    {
        uint32_t tries;
        size_t len;
        int fd;
        uint8_t *buf;
        uint32_t buf_indx;
        uint8_t op;
        uint32_t buf_section_indx;
    };

    const uint32_t n_chunks;
    const uint32_t n_files_per_chunk;
    const uint32_t n_files;
    const size_t file_size;
    const size_t chunk_size;
    const uint32_t n_buf_sections;

    uint8_t *const buf;
    uint32_t *const remaining;
    bool *const is_buf_section_free;
    list<uint32_t> chunk_marked_done;
    map<uint32_t, uint32_t> chunk_to_buf_section;

    const int *const fds;
    io_uring ring;

    inline uint8_t *const nth_buf_section(uint32_t n);
    inline const uint32_t nth_buf_section_indx(uint32_t n);
    inline const int nth_chunk_first_file(uint32_t chunk);
    inline uint8_t *const chunks_buf_section(uint32_t chunk);
    inline const uint32_t chunks_buf_section_indx(uint32_t chunk);

    void submit_sqe(SubData *s_data);

    void free_buf_section(const uint32_t chunk_id);
public:
    // Uses sequential `n_files_per_chunk_` files as chunks 
    ChunkyFIO(
        int *const fds_,
        const uint32_t n_files,
        const size_t file_size_,
        const uint32_t n_files_per_chunk_,
        uint8_t *const buf,
        size_t buf_size);

    ~ChunkyFIO();

    uint8_t *const assign_buf_section(const uint32_t chunk_id);
    void queue_read(const uint32_t chunk_id);
    void queue_write(const uint32_t chunk_id);
    void mark_done(const uint32_t chunk_id);
    const uint32_t wait_for_completion();
    
    // For debugging
    void print_chunk_buf_sections();
};

#endif