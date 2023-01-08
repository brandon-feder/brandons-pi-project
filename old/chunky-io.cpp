#include "chunky-io.h"

using namespace std;

inline uint32_t ChunkyFIOHandler::buf_indx(uint32_t subid)
{ return subid/(nf*ncpfpb);}

inline uint32_t ChunkyFIOHandler::file_indx(uint32_t subid)
{ return (subid/ncpfpb)%nf; }

inline const uint8_t *ChunkyFIOHandler::chunk_ptr(uint32_t subid)
{ return buf + subid*MAX_IO_SIZE - (subid/ncpfpb)*(MAX_IO_SIZE - bpfpb%MAX_IO_SIZE); }

inline uint32_t ChunkyFIOHandler::chunk_len(uint32_t subid)
{ return ((subid+1)%ncpfpb == 0) ? bpfpb%MAX_IO_SIZE : MAX_IO_SIZE; }

inline uint32_t ChunkyFIOHandler::file_offset(uint32_t subid)
{ return cur_batch_id*nf*bpfpb + (subid%ncpfpb)*MAX_IO_SIZE; }

// ============== DEFINE CONSTRUCTORS AND DESTRUCTORS ==============

ChunkyFIOHandler::ChunkyFIOHandler(
    const uint32_t nf_, const uint32_t nb_,
    const size_t bpfpb_, const uint32_t nbao_, 
    const int *fds_, const uint8_t *buf_
)
: nf(nf_), 
nb(nb_), 
bpfpb(bpfpb_), 
nbao(nbao_), 
fds(fds_), 
buf(buf_), 
ncpfpb(bpfpb/MAX_IO_SIZE + (bpfpb%MAX_IO_SIZE != 0)),
n_subs(ncpfpb*nf*nbao),
remaining(new uint32_t[nbao]),
sub_data(new SubData[ncpfpb*nf*nbao])
{
    // ======================================================
    // ==================== CHECK PARAMS ====================
    // ======================================================

    assert(nf_ > 0 && nb_ > 0 && bpfpb_ > 0 && nbao_ > 0);
    assert((uint64_t)nbao*nf*ncpfpb == nbao*nf*ncpfpb); // No overflow

    // ======================================================
    // =================== SETUP IOUring ====================
    // ======================================================

    int r;
    uint32_t i;

    // Init rings
    r = io_uring_queue_init(nf, &ring, 0);
    if(r < 0) throw runtime_error(string("Could not init IOUring queue: ")+strerror(-r));

    // Register files
    r = io_uring_register_files(&ring, fds, nf);
    if(r < 0) throw runtime_error(string("Could not register files: ")+strerror(-r));
    
    // Initial setup of sub_data array
    for(i = 0; i < ncpfpb*nf*nbao; ++i)
        sub_data[i].sub_id = i;

    // Setup fixed buffers
    iovec *buf_vec = new iovec[n_subs];
    for(uint32_t i = 0; i < n_subs; ++i) {
        buf_vec[i].iov_base = (void *)chunk_ptr(i);
        buf_vec[i].iov_len = chunk_len(i);
    }
    r = io_uring_register_buffers(&ring, buf_vec, 1);
    delete buf_vec;
    if(r < 0) throw runtime_error(string("Could not register buffers: ")+strerror(-r));
}

ChunkyFIOHandler::~ChunkyFIOHandler()
{
    io_uring_unregister_files(&ring);
    io_uring_unregister_buffers(&ring);
    io_uring_queue_exit(&ring);
    delete remaining;
    delete sub_data;
}

// const void ChunkyFIOHandler::queue_file_read(uint32_t bid, uint32_t findx)
// {
//     for(uint32_t i = 0; i < ncpfpb-1; ++i)
//     {

//     }
// }

// inline const uint8_t *ChunkyFIOHandler::indx_buf(uint32_t bid, uint32_t findx, uint32_t chunk)
// {
//     return buf + (bid%nbao)*bpfpb*nf + findx*bpfpb + chunk*MAX_IO_SIZE;
// }

// inline const size_t ChunkyFIOHandler::chunk_len(uint32_t chunk)
// {
//     return (chunk == ncpfpb-1) ? bpfpb%MAX_IO_SIZE : MAX_IO_SIZE;
// }

// void ChunkyFIOHandler::queue_batch_read()
// {
//     if(!dont_read && nxt_bat_queue < nb)
//     {
//         for(uint32_t i = 0; i < nf; ++i) // For each file
//         {
//             io_uring_sqe *sqe = io_uring_get_sqe(&ring);
//             if(!sqe)
//                 throw runtime_error("Could not get submission queue.");
            
//             io_uring_prep_read_fixed(
//                 sqe, i,
//                 (void *)indx_buf(nxt_bat_queue, i),
//                 bpfpb, nxt_bat_queue*bpfpb, i + nf*(nxt_bat_queue%nbao)
//             );

//             io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);
//             io_uring_sqe_set_data(sqe, (void *)(uint64_t)nxt_bat_queue);
//         }

//         int ret = io_uring_submit(&ring);
//         if(ret < 0)
//             throw runtime_error(string("Could not submit updated queue: ")+strerror(-ret));

//         remaining[nxt_bat_queue%nbao] = nf;
//         ++nxt_bat_queue;
//     }
// }

// void ChunkyFIOHandler::read()
// {
//     io_uring_cqe *cqe;
//     while(cur_batch_id < nb && remaining[cur_batch_id%nbao] > 0)
//     {
//         int ret = io_uring_wait_cqe(&ring, &cqe);

//         if (cqe->res < 0)
//             throw runtime_error(string("io_uring submission failed:") + strerror(-cqe->res));
        
//         uint64_t bid = (uint64_t)io_uring_cqe_get_data(cqe);
//         // printf("Read %d for %lu\n", cqe->res, bid);
//         remaining[bid%nbao]--;
//         io_uring_cqe_seen(&ring, cqe);
//     }
// }

// void ChunkyFIOHandler::next()
// {
//     queue_batch_read();
//     cur_batch_id++;
// }
