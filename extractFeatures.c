#include "extractFeatures.h"


#define BUFF_SIZE 1024

void normalize_sigmoid(float *arr, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        float exp_value;
        exp_value = exp((double) -arr[i]);
        arr[i] = (2/(1+exp_value)) - 1;
    }
}

void normalize_accel_arr(float *arr, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        arr[i] = arr[i]/16.0f;
    }
}

void normalize_gyro_arr(float *arr, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        arr[i] = arr[i]/500.0f;
    }
}

float arr_mean(float *arr, int start, int end)
{
    float total = 0.0;
    int i;
    for (i = start; i < end; i++)
    {
        total += arr[i];
    }
    return total/(float)(end-start+1);
}

float ratio (float n1, float n2)
{
    return n1/n2;
}

float std (float *arr, float mean, int start, int end)
{
    int i;
    float total = 0.0;
    int N = end - start;
    for (i = start; i < end; i++)
    {
        total += ((arr[i]-mean)*(arr[i]-mean))/N;
    }
    
    return sqrt(total);
}

float findMax (float *arr, int start, int end)
{
    int i;
    float max = arr[start];
    for (i = start; i < end; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
    }
    
    return max;
}

float findMin (float *arr, int start, int end)
{
    int i;
    float min = arr[start];
    for (i = start; i < end; i++)
    {
        if (arr[i] < min)
        {
            min = arr[i];
        }
    }
    
    return min;
}

float skewness(float *arr, int start, int end, float mean, float std)
{
    if (std == 0)
    {
        return 0.0f;
    }
    
    int i;
    float total = 0.0f;
    for (i = start; i < end; i++)
    {
        total += pow((arr[i]-mean)/std,3);
    }
    
    return total/((float) (end - start));
}


void extractFeatures (float *x_data,
                      float *y_data,
                      float *z_data,
                      float *gx_data,
                      float *gy_data,
                      float *gz_data,
                      int n_samples,
                      float *S_i,
                      int *n_S,
                      float *x_accel_max_seg,
                      float *x_accel_min_seg,
                      float *x_gyro_max_seg,
                      float *x_gyro_min_seg,
                      float *y_accel_max_seg,
                      float *y_accel_min_seg,
                      float *y_gyro_max_seg,
                      float *y_gyro_min_seg,
                      float *z_accel_max_seg,
                      float *z_accel_min_seg,
                      float *z_gyro_max_seg,
                      float *z_gyro_min_seg,
                      float *x_accel_mean_seg,
                      float *x_gyro_mean_seg,
                      float *y_accel_mean_seg,
                      float *y_gyro_mean_seg,
                      float *z_accel_mean_seg,
                      float *z_gyro_mean_seg,
                      float *x_accel_std_seg,
                      float *x_gyro_std_seg,
                      float *y_accel_std_seg,
                      float *y_gyro_std_seg,
                      float *z_accel_std_seg,
                      float *z_gyro_std_seg,
                      double *time_interval,
                      double *time,
                      float *x_accel_mean,
                      float *x_gyro_mean,
                      float *y_accel_mean,
                      float *y_gyro_mean,
                      float *z_accel_mean,
                      float *z_gyro_mean,
                      float *x_accel_std,
                      float *x_gyro_std,
                      float *y_accel_std,
                      float *y_gyro_std,
                      float *z_accel_std,
                      float *z_gyro_std,
                      float *x_accel_skew_seg,
                      float *y_accel_skew_seg,
                      float *z_accel_skew_seg,
                      float *x_gyro_skew_seg,
                      float *y_gyro_skew_seg,
                      float *z_gyro_skew_seg
                      )
{
    int i;
    int segSize; //the number of data in a segment
    int n_interval = *n_S - 1; //number of interval between strides
    
    float *x_norm, *y_norm, *z_norm, *gx_norm, *gy_norm, *gz_norm;
    
    x_norm = (float *) malloc(sizeof(float) * (n_samples));
    y_norm = (float *) malloc(sizeof(float) * (n_samples));
    z_norm = (float *) malloc(sizeof(float) * (n_samples));
    gx_norm = (float *) malloc(sizeof(float) * (n_samples));
    gy_norm = (float *) malloc(sizeof(float) * (n_samples));
    gz_norm = (float *) malloc(sizeof(float) * (n_samples));
    
    memcpy(x_norm, x_data, n_samples*sizeof(float));
    memcpy(y_norm, y_data, n_samples*sizeof(float));
    memcpy(z_norm, z_data, n_samples*sizeof(float));
    memcpy(gx_norm, gx_data, n_samples*sizeof(float));
    memcpy(gy_norm, gy_data, n_samples*sizeof(float));
    memcpy(gz_norm, gz_data, n_samples*sizeof(float));
    
    normalize_accel_arr(x_norm, n_samples);
    normalize_accel_arr(y_norm, n_samples);
    normalize_accel_arr(z_norm, n_samples);
    normalize_gyro_arr(gx_norm, n_samples);
    normalize_gyro_arr(gy_norm, n_samples);
    normalize_gyro_arr(gz_norm, n_samples);
    
    
    for (i = 0; i < n_interval; i++)
    {
        int idx1 = (int) S_i[i];
        int idx2 = (int) S_i[i+1];
        int segNum = 1; //segment counter
        
        segSize = (idx2-idx1+1)/4;
        
        time_interval[i] = time[idx2] - time[idx1];
        
        x_accel_mean[i] = arr_mean(x_norm, idx1, idx2);
        x_gyro_mean[i] = arr_mean(gx_norm, idx1, idx2);
        y_accel_mean[i] = arr_mean(y_norm, idx1, idx2);
        y_gyro_mean[i] = arr_mean(gy_norm, idx1, idx2);
        z_accel_mean[i] = arr_mean(z_norm, idx1, idx2);
        z_gyro_mean[i] = arr_mean(gz_norm, idx1, idx2);
        
        x_accel_std[i] = std(x_norm, x_accel_mean[i], idx1, idx2);
        x_gyro_std[i] = std(gx_norm, x_gyro_mean[i], idx1, idx2);
        y_accel_std[i] = std(y_norm, y_accel_mean[i], idx1, idx2);
        y_gyro_std[i] = std(gy_norm, y_gyro_mean[i], idx1, idx2);
        z_accel_std[i] = std(z_norm, z_accel_mean[i], idx1, idx2);
        z_gyro_std[i] = std(gz_norm, z_gyro_mean[i], idx1, idx2);
        
        while (segNum != 5)
        {
            int start = idx1+(segNum-1)*segSize;
            int end;
            int p = i*4+segNum-1; //position in the array that current data is put to
            
            if (segNum == 4)
            {
                end = idx2;
            }
            else
            {
                end = idx1+segNum*segSize;
            }
            
            x_accel_max_seg[p] = findMax(x_norm, start, end);
            x_accel_min_seg[p] = findMin(x_norm, start, end);
            x_gyro_max_seg[p] = findMax(gx_norm, start, end);
            x_gyro_min_seg[p] = findMin(gx_norm, start, end);
            y_accel_max_seg[p] = findMax(y_norm, start, end);
            y_accel_min_seg[p] = findMin(y_norm, start, end);
            y_gyro_max_seg[p] = findMax(gy_norm, start, end);
            y_gyro_min_seg[p] = findMin(gy_norm, start, end);
            z_accel_max_seg[p] = findMax(z_norm, start, end);
            z_accel_min_seg[p] = findMin(z_norm, start, end);
            z_gyro_max_seg[p] = findMax(gz_norm, start, end);
            z_gyro_min_seg[p] = findMin(gz_norm, start, end);
            
            x_accel_mean_seg[p] = arr_mean(x_norm, start, end);
            x_gyro_mean_seg[p] = arr_mean(gx_norm, start, end);
            y_accel_mean_seg[p] = arr_mean(y_norm, start, end);
            y_gyro_mean_seg[p] = arr_mean(gy_norm, start, end);
            z_accel_mean_seg[p] = arr_mean(z_norm, start, end);
            z_gyro_mean_seg[p] = arr_mean(gz_norm, start, end);
            
            x_accel_std_seg[p] = std(x_norm, x_accel_mean_seg[p], start, end);
            x_gyro_std_seg[p] = std(gx_norm, x_gyro_mean_seg[p], start, end);
            y_accel_std_seg[p] = std(y_norm, y_accel_mean_seg[p], start, end);
            y_gyro_std_seg[p] = std(gy_norm, y_gyro_mean_seg[p], start, end);
            z_accel_std_seg[p] = std(z_norm, z_accel_mean_seg[p], start, end);
            z_gyro_std_seg[p] = std(gz_norm, z_gyro_mean_seg[p], start, end);
            
            x_accel_skew_seg[p] = skewness(x_norm, start, end, x_accel_mean_seg[p], x_accel_std_seg[p]);
            y_accel_skew_seg[p] = skewness(y_norm, start, end, y_accel_mean_seg[p], y_accel_std_seg[p]);
            z_accel_skew_seg[p] = skewness(z_norm, start, end, z_accel_mean_seg[p], z_accel_std_seg[p]);
            x_gyro_skew_seg[p] = skewness(gx_norm, start, end, x_gyro_mean_seg[p], x_gyro_std_seg[p]);
            y_gyro_skew_seg[p] = skewness(gy_norm, start, end, y_gyro_mean_seg[p], y_gyro_std_seg[p]);
            z_gyro_skew_seg[p] = skewness(gz_norm, start, end, z_gyro_mean_seg[p], z_gyro_std_seg[p]);
            
            segNum++;
        }
    
        
    }
    
    normalize_sigmoid(x_accel_skew_seg, n_interval*4);
    normalize_sigmoid(y_accel_skew_seg, n_interval*4);
    normalize_sigmoid(z_accel_skew_seg, n_interval*4);
    normalize_sigmoid(x_gyro_skew_seg, n_interval*4);
    normalize_sigmoid(y_gyro_skew_seg, n_interval*4);
    normalize_sigmoid(z_gyro_skew_seg, n_interval*4);
    
}





