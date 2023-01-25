#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main()
{
    const char *dev = "/dev/nvme0n1";
    const int block_size = getpagesize();

    int fd = open(dev, O_RDWR);
    if(fd < 0) {
        fprintf(stderr, "error opening file; errno %d", errno);
        goto clean1;
    }

    int *ptr = mmap(
        NULL, block_size, 
        PROT_WRITE|PROT_READ,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0
    );
    if(ptr == MAP_FAILED) {
        fprintf(stderr, "error creating anon map; errno %d", errno);
        goto clean2;
    }

    for(int i = 0; i < 1024; ++i)
    {
        unsigned char *ptr2 = mmap(
            ptr, block_size, 
            PROT_WRITE|PROT_READ, 
            MAP_SHARED|MAP_FIXED, 
            fd, i*block_size
        );
        if(ptr2 == MAP_FAILED) {
            fprintf(stderr, "error mapping block %d; errno %d", i, errno);
            goto clean2;
        }

        memset(ptr2, i, block_size);
        printf("%u ", (unsigned int)ptr2[0]);
        fflush(stdout);
    }

    clean2:
    munmap(ptr, block_size);

    clean1:
    close(fd);
}