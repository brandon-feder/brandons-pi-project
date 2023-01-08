#include "moduli-set.h"

using namespace std;

ModuliSet::ModuliSet(uint32_t n_)
{
    char *pfile = getenv("BPP_PRIMES_FILE");
    if(pfile == NULL)
        throw runtime_error("Environment variable BPP_PRIMES_FILE not set");

    n = n_;
    moduli.resize(n);
    ifstream fin(pfile, ios::binary | ios::ate);
    fin.seekg(4*(N_32B_PRIMES-n), ios_base::beg);
    fin.read((char *)moduli.data(), 4*n);

    for(uint64_t m: moduli)
        erange += log2(m);
}

const vector<uint32_t> &ModuliSet::get_moduli() const
{ return moduli; }

double ModuliSet::get_erange()
{ return erange; } 