#ifndef __SEQ_BATCH_IO__
#define __SEQ_BATCH_IO__

#include <stdint.h>
#include <liburing.h>
#include <stdexcept>
#include <string.h>
#include <string>
#include <assert.h>
#include <list>

using namespace std;

#define MAX_IO_SIZE (size_t)6
#define READ 0
#define WRITE 1

class ChunkyFIOHandler
{
    private:
        io_uring ring;

        struct SubData
        {
            uint32_t sub_id;
            uint32_t batch_id;
            uint8_t op;
            iovec iov;
        };

        const uint32_t nb; // Number of batches
        const uint32_t nf; // Number of files
        const size_t bpfpb; // Bytes per file per batch
        const uint32_t nbao; // Number of batches being processed at any one time
        const uint32_t ncpfpb; // Number of chunks per file per batch
        const bool dont_read;
        const uint32_t n_subs;
        uint32_t *const remaining;
        SubData *const sub_data;

        uint32_t cur_batch_id = 0;
        uint32_t nxt_bat_queue = 0;

        const int *fds;
        uint8_t *const buf;

        inline uint32_t buf_indx(uint32_t sub_id);
        inline uint32_t file_indx(uint32_t sub_id);
        inline uint32_t chunk_len(uint32_t sub_id);
        inline const uint8_t *chunk_ptr(uint32_t sub_id);
        inline uint32_t file_offset(uint32_t sub_id);

    public:
        ChunkyFIOHandler(
            const uint32_t nf_, const uint32_t nb_,
            const size_t bpfpb_, const uint32_t nbao_, 
            const int *fds_, const uint8_t *buf_
        );
        
        ~ChunkyFIOHandler();
};

#endif