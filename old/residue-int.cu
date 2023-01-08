#include <stdio.h>
#include <fcntl.h>

// #include <cuda_runtime.h>
// #include <cufile.h>

#include "moduli-set.h"
#include "residue-int.h"

__host__
void ri_test()
{
    printf("ri_test here!\n");
}


// int ri_res_path(uint32_t i, char *path, struct ri *r)
// { return sprintf(path, "%s/%u.32b", r->dir, r->mset->moduli[i]) >= 0 ? 0 : -1; }

// int ri_init(char *dir, struct ms *mset, uint32_t digit_count, struct ri *r)
// {
//     // Set Structure Variables
//     r->dir = dir;
//     r->mset = mset;
//     r->nd = digit_count;
//     r->nm = mset->n;
//     r->ds = 4*r->nm; 

//     // Create files and allocate enough space
//     char path[RI_MAX_PATH_SIZE];
//     for(uint32_t i = 0; i < r->nm; ++i)
//     {
//         ri_res_path(i, path, r);
//         int fd = open(path, O_RDWR | O_CREAT, 0666);
//         if(fd < 0) return -1;
        
//         if(posix_fallocate(fd, 0, r->ds) != 0)
//             return -1;
        
//         close(fd);
//     }

//     return 0;
// }

// int ri_set_digit(uint32_t i, uint32_t k, struct ri *r)
// {
//     char path[RI_MAX_PATH_SIZE];
//     uint32_t n;
//     for(uint32_t j = 0; j < r->nm; ++j)
//     {
//         ri_res_path(j, path, r);
        
//         int fd = open(path, O_WRONLY);
//         if(fd < 0) return -1;

//         n = k%r->mset->moduli[j];
//         if(pwrite(fd, &n, 4, 4*i) == -1)
//             return -1;

//         close(fd);
//     }

//     return 0;
// }

// int ri_read_digit(uint32_t i, uint32_t *buf, struct ri *r)
// {
//     char path[RI_MAX_PATH_SIZE];
//     for(uint32_t j = 0; j < r->nm; ++j)
//     {
//         ri_res_path(j, path, r);
        
//         int fd = open(path, O_RDONLY);
//         if(fd == -1) return -1;

//         if(pread(fd, buf+j, 4, 4*i) == -1)
//             return -1;

//         close(fd);
//     }

//     return 0;
// }

// int ri_to_mrns(uint32_t fi, uint32_t si, struct ri *r)
// {
//     CUfileDescr_t cfd[r->nm];
//     CUfileHandle_t cfh[r->nm];
// }

// int ri_cufile_dig_setup(uint32_t n, CUfileDescr_t **cfd, CUfileDescr_t **cfd)
// {
    
// }
// {
//    memset((void *)cfd, 0, MAX_BATCH_IOS * sizeof(CUfileDescr_t));
// }

// int ri_cufile_dig_set(uint32_t fi, uint32_t si)
// {
    
// }

// int ri_cufile_dig_quit()
// {

// }

// int fds[r.nm]; // Posix file descriptors
// CUfileDescr_t cfd[r.nm]; // cuFile Descriptors
// CUfileHandle_t cfh[r.nm]; // cuFile handeler
// CUfileIOParams_t batch_parms[r.nm];
// CUfileBatchHandle_t batch_id;
// uint32_t *dev_buf;

// memset((void *)cfd, 0, r.nm*sizeof(CUfileDescr_t));
// for(uint32_t i = 0; i < r.nm; i++) {
//     fds[i] = open(residue_path(i, r).c_str(), O_RDWR | O_DIRECT);
//     posixCheck(fds[i]);
//     cfd[i].handle.fd = fds[i];
//     cfd[i].type = CU_FILE_HANDLE_TYPE_OPAQUE_FD;
//     cuFileCheck(cuFileHandleRegister(&cfh[i], &cfd[i]));
// }

// // Setup device buffer
// cudaCheck(cudaMalloc(&dev_buf, r.nm*(fi - si)));
// cuFileCheck(cuFileBufRegister(dev_buf, r.nm*(fi - si), 0));

// // Setup batch IO
// for(uint32_t i = 0; i < r.nm; i++) {
//     batch_parms[i].mode = CUFILE_BATCH;
//     batch_parms[i].fh = cfh[i];
//     batch_parms[i].u.batch.devPtr_base = dev_buf;
//     batch_parms[i].u.batch.devPtr_offset = fi-si;
//     batch_parms[i].u.batch.file_offset = 4*si;
//     batch_parms[i].u.batch.size = 4*fi;
//     batch_parms[i].opcode = CUFILE_READ;
// }
// cuFileCheck(cuFileBatchIOSetUp(&batch_id, r.nm));