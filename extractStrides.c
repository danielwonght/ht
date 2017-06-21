#include "extractStrides.h"

#define BUFF_SIZE 1024

/*
 * sets first <n> values in <*arr> to <val>
 */
void clear_buffer(float *arr, float val, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        arr[i] = val;
    }
}

float calcMean(float *data, float *idx, int n)
{
    float total = 0.0;
    int i;
    for (i = 0; i < n; i++) {
        int idx_temp = (int) idx[i];
        total += data[idx_temp];
    }
    return (total/(float) n);
}

int
find_peaks_and_troughs(
                       float *arr, 	// signal
                       int n_samples, 	// number of samples present in the signal
                       float E, 	// threshold for peak detection
                       // arrays that will store the indicies of the located
                       // peaks and troughs
                       float *P, float *T,
                       // number of peaks (n_P) and number of troughs (n_T)
                       // found in the data set *arr
                       int *n_P, int *n_T
                       )
{
    int a, b, i, d, _n_P, _n_T;
    
    i = -1; d = 0; a = 0; b = 0;
    _n_P = 0; _n_T = 0;
    
    clear_buffer(P, 0.0f, n_samples);
    clear_buffer(T, 0.0f, n_samples);
    
    while (i != n_samples) {
        i++;
        if (d == 0) {
            if (arr[a] >= (arr[i] + E)) {
                d = 2;
            } else if (arr[i] >= (arr[b] + E)) {
                d = 1;
            }
            if (arr[a] <= arr[i]) {
                a = i;
            } else if (arr[i] <= arr[b]) {
                b = i;
            }
        } else if (d == 1) {
            if (arr[a] <= arr[i]) {
                a = i;
            } else if (arr[a] >= (arr[i] + E)) {
                /*
                 * Peak has been detected.
                 * Add index at detected peak
                 * to array of peak indicies
                 * increment count of peak indicies
                 */
                P[_n_P] = a;
                _n_P++;
                b = i;
                d = 2;
            }
        } else if (d == 2) {
            if (arr[i] <= arr[b]) {
                b = i;
            } else if (arr[i] >= (arr[b] + E)) {
                /*
                 * Trough has been detected.
                 * Add index at detected trough
                 * to array of trough indicies
                 * increment count of trough indicies
                 */
                T[_n_T] = b;
                _n_T++;
                a = i;
                d = 1;
            }
        }
    }
    
    (*n_P) = _n_P;
    (*n_T) = _n_T;
    return 0;
}

int extractStrides(
                    float *data, 	// signal
                    double *time,    // time of signal
                    int n_samples, 	// number of samples present in the signal
                    float E, 	// threshold for peak detection
                    int *n_S,
                    float *S_i
                    )
{
    
    int rv, n_P, n_T;
    int j; // for filtering data
    int k; // for filtering data
    float *P_i;
    float *T_i;
    float *temp; // for filtering data
    
    P_i = (float *) malloc(sizeof(float) * n_samples);
    T_i = (float *) malloc(sizeof(float) * n_samples);
    temp = (float *) malloc(sizeof(float) * n_samples);
    
    rv = find_peaks_and_troughs(data, n_samples, E, P_i, T_i, &n_P, &n_T);
    
    
    if (rv < 0) {
        fprintf(stderr, "find_peaks_and_troughs failed\n");
        exit(EXIT_FAILURE);
    }
    
    // start filtering peaks or troughs that are too close
    if (n_T > 1)
    {
        float minimum = data[(int)T_i[0]];
        int num = 0;
        
        for (j = 1; j < n_T; j++)
        {
            if (data[(int)T_i[j]] < minimum)
            {
                minimum = data[(int)T_i[j]];
            }
        }
        
        for (j = 0; j < n_T; j++)
        {
            if (data[(int)T_i[j]] < 0.5*minimum)
            {
                temp[num] = T_i[j];
                num++;
            }
        }
        
        
        S_i[0] = temp[0];
        k = 0;
        
        for (j = 1; j < num; j++)
        {
            int idx1 = (int) temp[j];
            int idx2 = (int) S_i[k];
            
            if (time[idx1] - time[idx2] < 0.5)
            {
                if(data[idx1] < data[idx2])
                {
                    S_i[k] = temp[j];
                }
            }
            else
            {
                k++;
                S_i[k] = temp[j];
            }
        }
        
        *n_S = k+1;

        
        return 1;
    }
    else
    {
        return 0;
    }
}












