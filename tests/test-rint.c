#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#define BPP_DATA_DRIVE "/home/koala/Documents/brandons-pi-program/data-drives"
// #define BPP_PRIMES_FILE "/home/koala/Documents/brandons-pi-program/primes.32b"

#include "moduli-set.h"
#include "residue-int.cuh"
#include "debug.h"


#define N_PRIMES 100
#define N_DIGITS 100

void print_u32_arr(uint32_t *arr, size_t s)
{
    if(s == 0) printf("[]\n");
    else
    {
        printf("[%u", arr[0]);
        for(int i = 0; i < s; ++i)
            printf(", %u", arr[i]);
        printf("]\n");
    }
}

int main()
{
    printf("%s, %s", BPP_DATA_DRIVE, getenv("BPP_PRIMES_FILE"));
    printf("%s testing residue integer library\n", INF);
    struct ms m;
    struct ri r;
    uint32_t *buf;

    // Init Moduli Set
    if(ms_init(N_PRIMES, &m) == -1)
    {
        printf("%s could not initialize moduli set; errno %d\n", ERR, errno);
        goto clean1;
    }
    
    printf("%s initialized moduli set\n", SUC);

    // Init Residue Integer
    if(ri_init(BPP_DATA_DRIVE, &m, N_DIGITS, &r) == -1)
    {
        printf("%s could not initialize residue integer; errno %d\n", ERR, errno);
        goto clean1;
    }
    
    printf("%s initialized residue integer\n", SUC);

    // Write and read first digit
    buf = (uint32_t *)malloc(r.ds);
    if(ri_set_digit(0, (uint32_t)-1, &r) == -1)
    {
        printf("%s could not write first digit; errno %d\n", ERR, errno);
        goto clean2;
    }

    if(ri_read_digit(0, buf, &r) == -1)
    {
        printf("%s could not read first digit; errno %d\n", ERR, errno);
        goto clean2;
    }

    // Make sure last few moduli are correct
    if(
        buf[r.nd-1] == 4 &&
        buf[r.nd-2] == 16 &&
        buf[r.nd-3] == 64 &&
        buf[r.nd-4] == 98
    ) printf("%s wrote and read first digit correctly", SUC);
    else 
    {
        printf("%s did not write and read first digit", ERR);
        goto clean2;
    }

    // Cleanup
    clean2:
    free(buf);

    clean1:
    ms_quit(&m);
}