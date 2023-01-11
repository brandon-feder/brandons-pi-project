#include "chunky-fio.h"
#include "debug.h"

inline uint8_t *const ChunkyFIOHandler::nth_buf_section(uint32_t n)
{ return buf + n*chunk_size; }

inline const uint32_t ChunkyFIOHandler::nth_buf_section_indx(uint32_t n)
{ return n*chunk_size; }

inline const int ChunkyFIOHandler::nth_chunk_first_file(uint32_t chunk)
{ return chunk*n_files_per_chunk; }

inline uint8_t *const ChunkyFIOHandler::chunks_buf_section(uint32_t chunk_id)
{ return nth_buf_section(chunk_to_buf_section[chunk_id]); }

inline const uint32_t ChunkyFIOHandler::chunks_buf_section_indx(uint32_t chunk_id)
{ return nth_buf_section_indx(chunk_to_buf_section[chunk_id]); }

void ChunkyFIOHandler::free_buf_section(const uint32_t chunk_id)
{
    is_buf_section_free[chunk_to_buf_section[chunk_id]] = true;
    chunk_to_buf_section.erase(chunk_id);
}

const uint32_t ChunkyFIOHandler::wait_for_completion()
{
    typedef list<uint32_t>::iterator u32_iter;
    
    // Make sure there is at least one chunk marked done
    #if CHUNKY_FIO_DEBUG_MODE
        if(chunk_marked_done.size() == 0)
            throw runtime_error("No chunks are marked done!");
    #endif

    uint32_t n_cqe;
    u32_iter chunk, min_remaining_chunk;
    io_uring_cqe *cqe;

    while(1) {
        // Get the minimum remaining
        min_remaining_chunk = chunk_marked_done.begin();

        for(chunk = chunk_marked_done.begin(); 
            chunk != chunk_marked_done.end(); 
            ++chunk) {

            if(remaining[*chunk] < remaining[*min_remaining_chunk])
                min_remaining_chunk = chunk;
        }

        n_cqe = remaining[*min_remaining_chunk];

        // If it has 0 remaining e.g. it is done, return it
        if(n_cqe == 0)
        {
            uint32_t b_sec = chunk_to_buf_section[*min_remaining_chunk];
            free_buf_section(*min_remaining_chunk);
            chunk_marked_done.erase(min_remaining_chunk);
            return b_sec;
        }

        printf("%s n_cqe: %u\n", INF, n_cqe);

        for(uint32_t i = 0; i < n_cqe; ++i)
        {
            int ret = io_uring_wait_cqe(&ring, &cqe);
            if(ret < 0)
                throw runtime_error(str_fprintf(
                    "io_uring_wait_cqe failed: %s", strerror(-ret)
                ));

            if (cqe->res < 0)
                throw runtime_error(string("io_uring submission failed:") + strerror(-cqe->res));

            SubData *s_data = (SubData *)io_uring_cqe_get_data(cqe);
            remaining[s_data->buf_section_indx]--;
            io_uring_cqe_seen(&ring, cqe);
            delete s_data;
        }
    }
}

ChunkyFIOHandler::ChunkyFIOHandler(
    int *fds_,
    uint32_t n_files_,
    size_t file_size_,
    uint32_t n_files_per_chunk_,
    uint8_t *buf_,
    size_t buf_size) :
n_chunks(n_files_/n_files_per_chunk_),
n_files_per_chunk(n_files_per_chunk_),
n_files(n_files_),
file_size(file_size_),
chunk_size(file_size*n_files_per_chunk_),
n_buf_sections(buf_size/chunk_size),
buf(buf_),
remaining(new uint32_t[n_buf_sections]),
fds(fds_),
is_buf_section_free(new bool[n_buf_sections])
{
    // Check parameters
    if(n_files == 0 || file_size == 0 || n_files_per_chunk_ == 0)
        throw invalid_argument("n_files, file_size, and n_files_per_chunk_ must be greater than 0");
    if(n_buf_sections == 0)
        throw invalid_argument("Not enough space in the buffer for even a single section.");
    if(file_size >= 0x40000000) // For buffer to work with io_uring
        throw invalid_argument("file_size must be less than 1073741824 bytes.");

    int r;
    uint32_t i;

    // Setup iouring ring strcture
    r = io_uring_queue_init(n_files_per_chunk, &ring, 0);
    if(r < 0) throw runtime_error(string("Could not init IOUring queue: ")+strerror(-r));

    // Register files
    r = io_uring_register_files(&ring, fds, n_files);
    if(r < 0) throw runtime_error(string("Could not register files: ")+strerror(-r));
    
    // Register buffers
    iovec *buf_vec = new iovec[n_buf_sections*n_files_per_chunk];
    uint8_t *buf_ptr = buf;
    for(i = 0; i < n_buf_sections*n_files_per_chunk; ++i) {
        buf_vec[i].iov_base = (void *)buf_ptr;
        buf_vec[i].iov_len = file_size;
        buf_ptr += file_size;
    }
    r = io_uring_register_buffers(&ring, buf_vec, n_buf_sections);
    delete buf_vec;
    if(r < 0) throw runtime_error(string("Could not register buffers: ")+strerror(-r));

    // Mark all buffer sections as free
    for(i = 0; i < n_buf_sections; ++i)
        is_buf_section_free[i] = true;

    // Set the remaining to 0
    for(i = 0; i < n_buf_sections; ++i)
        remaining[i] = 0;
}

ChunkyFIOHandler::~ChunkyFIOHandler()
{
    io_uring_unregister_files(&ring);
    io_uring_unregister_buffers(&ring);
    io_uring_queue_exit(&ring);
    delete remaining;
    delete is_buf_section_free;
}

uint8_t *const ChunkyFIOHandler::assign_buf_section(const uint32_t chunk_id)
{
    // Try to find a free buf section
    for(uint32_t i = 0; i < n_buf_sections; ++i)
        if(is_buf_section_free[i]) {
            is_buf_section_free[i] = false;
            chunk_to_buf_section.insert(pair<uint32_t, uint32_t>(chunk_id, i));
            return nth_buf_section(i);
        }

    // If one is not free, wait for a completion
    uint32_t bsec_indx = wait_for_completion();

    is_buf_section_free[bsec_indx] = false;
    chunk_to_buf_section.insert(pair<uint32_t, uint32_t>(
        chunk_id, bsec_indx
    ));

    return nth_buf_section(bsec_indx);
}

void ChunkyFIOHandler::submit_sqe(SubData *s_data)
{
    if(s_data->op == OP_READ)
    {
        io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        if(!sqe)
            throw runtime_error("Could not get submission queue.");
        
        io_uring_prep_read_fixed(
            sqe, 
            s_data->fd, 
            (void *)s_data->buf, s_data->len, 
            0, s_data->buf_indx);
        io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);
        io_uring_sqe_set_data(sqe, (void *)s_data);
    } else if(s_data->op == OP_WRITE)
    {
        io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        if(!sqe)
            throw runtime_error("Could not get submission queue.");
        
        io_uring_prep_write_fixed(
            sqe, 
            s_data->fd, 
            (void *)s_data->buf, s_data->len, 
            0, s_data->buf_indx);
        io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);
        io_uring_sqe_set_data(sqe, (void *)s_data);
    }
}

void ChunkyFIOHandler::queue_read(const uint32_t chunk_id)
{
    int fd = nth_chunk_first_file(chunk_id);
    uint8_t *buf_ptr = chunks_buf_section(chunk_id);
    uint32_t buf_indx = chunks_buf_section_indx(chunk_id);

    for(uint32_t i = 0; i < n_files_per_chunk; ++i)
    {
        SubData *s_data = new SubData;
        s_data->tries = MAX_COMPLETION_TRIES;
        s_data->len = file_size;
        s_data->fd = fd;
        s_data->buf = buf_ptr;
        s_data->buf_indx = buf_indx;
        s_data->op = OP_READ;
        s_data->buf_section_indx = chunk_to_buf_section[chunk_id];

        submit_sqe(s_data);

        ++fd; ++buf_indx;
        buf_ptr += file_size;
    }

    int r = io_uring_submit(&ring);
    if(r < 0)
        throw runtime_error(str_fprintf(
            "Could not submit new reads to io_uring: %s", 
            strerror(-errno)));
}

void ChunkyFIOHandler::queue_write(const uint32_t chunk_id)
{
    int fd = nth_chunk_first_file(chunk_id);
    uint8_t *buf_ptr = chunks_buf_section(chunk_id);
    uint32_t buf_indx = chunks_buf_section_indx(chunk_id);

    for(uint32_t i = 0; i < n_files_per_chunk; ++i)
    {
        SubData *s_data = new SubData;
        s_data->tries = MAX_COMPLETION_TRIES;
        s_data->len = file_size;
        s_data->fd = fd;
        s_data->buf = buf_ptr;
        s_data->buf_indx = buf_indx;
        s_data->op = OP_WRITE;
        s_data->buf_section_indx = chunk_to_buf_section[chunk_id];

        printf("%s s_data->buf_indx: %lu\n", INF, s_data->buf_indx);
        submit_sqe(s_data);

        ++fd; ++buf_indx;
        buf_ptr += file_size;
    }

    remaining[chunk_to_buf_section[chunk_id]] += n_files_per_chunk;
    int r = io_uring_submit(&ring);
    if(r < 0)
        throw runtime_error(str_fprintf(
            "Could not submit new writes to io_uring: %s", 
            strerror(-errno)));
}

void ChunkyFIOHandler::mark_done(const uint32_t chunk_id)
{ 
    if(chunk_to_buf_section.find(chunk_id) == chunk_to_buf_section.end())
        throw runtime_error("The chunk_id marked for completion is not assigned to a buffer section.");
    else
        chunk_marked_done.push_back(chunk_id); 
}

void ChunkyFIOHandler::print_chunk_buf_sections()
{
    map<uint32_t, uint32_t>::iterator i = chunk_to_buf_section.begin();

    printf("%s chunk, buf section, marked done\n", INF);
    for(; i != chunk_to_buf_section.end(); ++i)
        printf("%s %d,     %d,           %s\n", 
            TAB, (*i).first, 
            (*i).second,
            find(
                chunk_marked_done.begin(), 
                chunk_marked_done.end(),
                (*i).first
            ) != chunk_marked_done.end() ? "yes" : "no"
        );
}