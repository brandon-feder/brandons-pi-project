#include "chunky-fio.h"

inline uint8_t *const ChunkyFIOHandler::nth_buf_section(uint32_t n)
{ return buf + n*chunk_size; }

inline const int ChunkyFIOHandler::nth_chunk_file(uint32_t chunk, uint32_t n)
{ return n + chunk*n_files_per_chunk; }

ChunkyFIOHandler::ChunkyFIOHandler(
    int *fds_,
    uint32_t n_files_,
    size_t file_size_,
    uint32_t n_files_per_chunk_,
    uint8_t *buf_,
    uint32_t buf_size) :
n_chunks(n_files_/n_files_per_chunk_),
n_files_per_chunk(n_files_per_chunk_),
n_files(n_files_),
file_size(file_size_),
chunk_size(file_size*n_files_per_chunk_),
n_buf_sections(buf_size/chunk_size),
buf(buf_),
remaining(new uint32_t[n_buf_sections]),
fds(fds_),
chunk_buf_section(new uint32_t[n_buf_sections])
{
    // Check parameters
    if(n_files > 0 || file_size > 0 || n_files_per_chunk_ || 0)
        throw invalid_argument("n_files, file_size, and n_files_per_chunk_ must be greater than 0");
    if(n_buf_sections == 0)
        throw invalid_argument("Not enough space in the buffer for even a single section.");
    if(file_size >= 0x40000000) // For buffer to work with io_uring
        throw invalid_argument("file_size must be less than 1073741824 bytes.");

    int r;
    uint32_t i;

    // Setup iouring ring strcture
    r = io_uring_queue_init(n_files, &ring, 0);
    if(r < 0) throw runtime_error(string("Could not init IOUring queue: ")+strerror(-r));

    // Register files
    r = io_uring_register_files(&ring, fds, n_files);
    if(r < 0) throw runtime_error(string("Could not register files: ")+strerror(-r));
    
    // Register buffers
    iovec *buf_vec = new iovec[n_buf_sections*n_files_per_chunk];
    for(i = 0; i < n_buf_sections*n_files_per_chunk; ++i) {
        buf_vec[i].iov_base = (void *)nth_buf_section(i);
        buf_vec[i].iov_len = chunk_size;
    }
    r = io_uring_register_buffers(&ring, buf_vec, n_buf_sections);
    delete buf_vec;
    if(r < 0) throw runtime_error(string("Could not register buffers: ")+strerror(-r));

    // Mark all buffer sections as free
    for(i = 0; i < n_buf_sections; ++i)
        chunk_buf_section[i] = MAX_U32;
}

ChunkyFIOHandler::~ChunkyFIOHandler()
{
    io_uring_unregister_files(&ring);
    io_uring_unregister_buffers(&ring);
    io_uring_queue_exit(&ring);
    delete remaining;
    delete chunk_buf_section;
}

uint8_t *const ChunkyFIOHandler::assign_buf_section(const uint32_t chunk_id)
{
    for(uint32_t i = 0; i < n_buf_sections; ++i)
        if(chunk_buf_section[i] == MAX_U32) {
            chunk_buf_section[i] = chunk_id;
            return nth_buf_section(i);
        }

    return NULL;
}

void ChunkyFIOHandler::queue_read(const uint32_t chunk_id)
{
    for(uint32_t i = 0; i < n_files; ++i)
    {
        io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        if(!sqe)
            throw runtime_error("Could not get submission queue.");
        
        io_uring_prep_read_fixed(
            sqe, nth_chunk_file(chunk_id, i),
            (void *)nth_buf_section(nxt_bat_queue, i),
            bpfpb, nxt_bat_queue*bpfpb, i + nf*(nxt_bat_queue%nbao)
        );

        io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);
        io_uring_sqe_set_data(sqe, (void *)(uint64_t)nxt_bat_queue);
    }
}


// bool queue_read(const uint32_t chunk_id);
// bool queue_write(const uint32_t chunk_id);
// void wait_for_completions(const uint32_t chunk_id);
// void free_buf_section(const uint32_t chunk_id);