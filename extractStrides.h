#ifndef extractStrides_h
#define extractStrides_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int extractStrides(
                     float *data,
                     double *time,
                     int n_samples,
                     float E,
                     int *n_S,
                     float *S_i
                     );

#endif
