#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <fstream>  
#include <unistd.h>
#include <sys/time.h>

#include "debug.h"
#include "moduli-set.h"
#include "residue-int.h"

using namespace std;

int main()
{
    uint32_t n_moduli, n_digits;
    n_moduli = 100;
    n_digits = 100;

    printf("%s program start\n", SUC);
    ModuliSet M(n_moduli);
    printf("%s created moduli set\n", SUC);

    printf("%s moduli set effective range: %fb\n", INF, M.get_erange());
    printf("%s residue integer min size: %ld bytes\n", INF, 
        M.get_moduli().size()*4 * n_digits
    );
}
