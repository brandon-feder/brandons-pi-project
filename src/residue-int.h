#ifndef __RESIDUE_INT__
#define __RESIDUE_INT__

#include <vector>
#include <assert.h>
#include <string>

#include "moduli-set.h"

using namespace std;

class ResidueInt
{
    private:
        string dir;
        const ModuliSet &moduli_set;
        const vector<uint32_t> &moduli;
        uint32_t nd;
        uint32_t nm;

        inline string residue_path(uint32_t i);

    public:
        ResidueInt(string dir_, const ModuliSet &moduli_set_, uint32_t nd_);

        static int compare(
            ResidueInt &A, ResidueInt &B, 
            uint32_t *scratch, uint32_t scratch_size
        );

        // These should only be used for debugging
        void naive_read_digit(uint32_t i, uint32_t *buf);
        vector<uint32_t> naive_read_digit(uint32_t i);
        void naive_set_digit(uint32_t k, uint32_t i);
};

#endif