#ifndef __MODULI_SET__
#define __MODULI_SET__

#include <math.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <math.h>
#include <fstream>
#include <exception>

using namespace std;

#define N_32B_PRIMES 203280220 // Not including 2

class ModuliSet
{
    private:
        vector<uint32_t> moduli;
        uint32_t n;
        double erange;

    public:
        ModuliSet(uint32_t n_);
        const vector<uint32_t> &get_moduli() const;
        double get_erange();
};

#endif