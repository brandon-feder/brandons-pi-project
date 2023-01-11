#include "debug.h"
#include "chunky-fio.h"

#define CHUNKY_FIO_DEBUG_MODE 1

const uint32_t n_chunks = 10;
const uint32_t n_files_per_chunk = 8;
const size_t file_size = 10;
const uint32_t n_chunks_at_once = 4;

const size_t buf_size =  file_size*n_files_per_chunk*n_chunks_at_once;
const uint32_t n_files = n_chunks * n_files_per_chunk;

int * fds;
uint8_t * buf;

void fill_with_test_data(ChunkyFIOHandler &C)
{
    for(uint32_t i = 0; i < buf_size; ++i)
        buf[i] = i;

    C.assign_buf_section(0);
    C.queue_write(0);
    C.mark_done(0);
    C.wait_for_completion();
    C.print_chunk_buf_sections();
}

int main()
{
    test_env_vars("BPP_TEST_DIR_PATH");
    
    // Do edit

    // Do NOT edit

    printf("%s started ChunkyFIO test\n", SUC);
    printf(
        "%s # chunks: %d\n%s # files/chunk: %d\n%s file size: %d\n",
        INF, n_chunks, INF, n_files_per_chunk, INF, file_size
    );

    // ========= CREATE TEST FILES =========
    create_n_test_files(10, 10);
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
        fill_with_test_data(C);
        printf("%s F\n", ERR);
    } catch(...)
    {
        printf("%s error filling with test data\n", ERR);
        c_close_files(n_files, fds);
        delete fds;
        delete buf;
        throw;
    }
    printf("%S filled files with test data\n", SUC);

    // ============= CLEAN UP ==============
    c_close_files(n_files, fds);
    delete fds;
    delete buf;
}