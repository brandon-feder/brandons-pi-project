#ifndef NUMBER_THEORY_CU
#define NUMBER_THEORY_CU

#include <math.h>
#include <stdint.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <assert.h>

inline uint64_t mul64_mod(uint64_t a, uint64_t b, uint64_t m) 
{
    uint64_t r;
    __asm__( 
        "mulq %2\n\t"
        "divq %3"
        : "=&d" (r), "+%a" (a)
        : "rm" (b), "rm" (m)
        : "cc"
    );
    return r;
}

inline uint32_t mul32_mod(uint32_t a, uint32_t b, uint32_t m) 
{
    uint32_t r;
    __asm__( 
        "mulq %2\n\t"
        "divq %3"
        : "=&d" (r), "+%a" (a)
        : "rm" (b), "rm" (m)
        : "cc"
    );
    return r;
}

// Assumes m != 1 and a is already reduced mod m
inline uint64_t exp64_mod(uint64_t a, uint64_t exp, uint64_t m) 
{
    uint64_t r = 1;
    while(exp > 0)
    {
        if(exp&1)
            r = mul64_mod(r, a, m);

        exp >>= 1;
        a = mul64_mod(a, a, m);
    }

    return r;
}

inline uint32_t exp32_mod(uint32_t a, uint32_t exp, uint32_t m) 
{
    uint32_t r = 1;
    while(exp > 0)
    {
        if(exp&1)
            r = mul32_mod(r, a, m);

        exp >>= 1;
        a = mul32_mod(a, a, m);
    }

    return r;
}

uint64_t wit[12] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
bool miller(uint64_t n)
{
    uint64_t s = __builtin_ctz(n-1);
    uint64_t d = (n-1)>>s;

    uint64_t x, y;
    for(uint64_t a : wit) {
        x = exp64_mod(a, d, n);
        for(int i = 0; i < s; ++i) 
        {
            y = mul64_mod(x, x, n);
            if(y == 1 && x != 1 && x != n-1)
                return false;
        
            x = y;
        }

        if(y != 1)
            return false;
    }

    return true;
}

#endif