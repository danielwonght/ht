
#ifndef extractFeatures_h
#define extractFeatures_h

#include <stdio.h>
/* for file and terminal I/O */
#include <stdio.h>
/* for string manip */
#include <string.h>
/* for exit() */
#include <stdlib.h>
/* for fabsf() */
#include <math.h>

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
                      );

#endif /* extractFeatures_h */
