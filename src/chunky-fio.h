#ifndef __CHUNKY_FIO__
#define __CHUNKY_FIO__

#include <liburing.h>
#include <stdexcept>
#include <vector>
#include <string.h>

using namespace std;

#define MAX_U32 !(uint32_t)0
// #define MAX_IO_SIZE 1024;

class ChunkyFIOHandler
{
private:
    const uint32_t n_chunks;
    const uint32_t n_files_per_chunk;
    const uint32_t n_files;
    const size_t file_size;
    const size_t chunk_size;
    const uint32_t n_buf_sections;

    uint8_t *const buf;
    uint32_t *const remaining;
    uint32_t *const chunk_buf_section;
    const int *const fds;

    io_uring ring;

    // Returns a pointer to the start of the buffer section
    inline uint8_t *const nth_buf_section(uint32_t n);

    // Returns a pointer to the first file in a chunk
    inline const int nth_chunk_file(uint32_t chunk, uint32_t n);

public:
    // Uses sequential `n_files_per_chunk_` files as chunks 
    ChunkyFIOHandler(
        int *const fds_,
        const uint32_t n_files,
        const size_t file_size_,
        const uint32_t n_files_per_chunk_,
        uint8_t *const buf,
        uint32_t buf_size);

    ~ChunkyFIOHandler();

    uint8_t *const assign_buf_section(const uint32_t chunk_id);
    void queue_read(const uint32_t chunk_id);
    void queue_write(const uint32_t chunk_id);
    void wait_for_completions(const uint32_t chunk_id);
    void free_buf_section(const uint32_t chunk_id);
};

#endif