#include <fstream>

#include "residue-int.h"

using namespace std;

inline string ResidueInt::residue_path(uint32_t i)
{ return dir + "/" + to_string(moduli[i]) + ".32b"; }

ResidueInt::ResidueInt(string dir_, const ModuliSet &moduli_set_, uint32_t nd_)
: moduli_set(moduli_set_), moduli(moduli_set_.get_moduli())
{
    dir = dir_;
    nd = nd_;
    nm = moduli.size();

    // Create file and allocate enough space
    for(uint32_t i = 0; i < nm; ++i)
    {
        ofstream rfile(residue_path(i), ios::binary | ios::out);
        rfile.seekp(4*nd-1);
        rfile.write("", 1);
    }
}

void ResidueInt::naive_read_digit(uint32_t i, uint32_t *buf)
{
    for(uint32_t j = 0; j < nm; ++j)
    {
        int fd = open(residue_path(j).c_str(), O_RDONLY);
        if(fd == -1)
            throw runtime_error("Could not open file for reading");

        if(pread(fd, buf+j, 4, 4*i) == -1)
            throw runtime_error("Could not read from file to buffer");

        close(fd);
    }
}

vector<uint32_t> ResidueInt::naive_read_digit(uint32_t i)
{
    vector<uint32_t> res;
    res.resize(nm);
    naive_read_digit(i, res.data());
    return res;
}

void ResidueInt::naive_set_digit(uint32_t k, uint32_t i)
{
    uint32_t n;
    for(uint32_t j = 0; j < nm; ++j)
    {
        int fd = open(residue_path(j).c_str(), O_WRONLY);
        if(fd < 0)
            throw runtime_error("Could not open file for writing");

        n = k%moduli[j];
        if(pwrite(fd, &n, 4, 4*i) == -1)
            throw runtime_error("Could not write residue");

        close(fd);
    }
}

// TODO: Support different number of digits
int ResidueInt::compare(
    ResidueInt &A, ResidueInt &B,
    uint32_t *scratch, uint32_t scratch_size
)
{
    if(&A.moduli_set != &B.moduli_set || A.nd != B.nd)
        throw runtime_error(
            "Cannot compare two ResidueInt objects with \
            different moduli sets or number of digits."
        );

    

    return 1;
}