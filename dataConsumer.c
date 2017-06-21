#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <mraa/i2c.h>
#include <sys/time.h>
#include "extractStrides.h"
#include "extractFeatures.h"
#include "decision_maker.h"
#define BUFF_SIZE 1024
#define ARRAY_CONCAT(TYPE, A, An, B, Bn) \
(TYPE *)array_concat((const void *)(A), (An), (const void *)(B), (Bn), sizeof(TYPE));

void *array_concat(const void *a, size_t an,
                   const void *b, size_t bn, size_t s)
{
    char *p = malloc(s * (an + bn));
    memcpy(p, a, an*s);
    memcpy(p + an*s, b, bn*s);
    return p;
}



sig_atomic_t volatile run_flag = 1;

void do_when_interrupted(int sig)
{
    if (sig == SIGINT)
        run_flag = 0;
}

int main() {
    int i, j, k, fd;
    int rv;
    /* Variables for reading file line by line */
    char *ifile_name;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int default_SAMPLE_Size = 100000;
    int N_SAMPLES = 5000;
    int N_Data;
    int FeatureArrSize = 50;
    ifile_name = "data.csv";
    
    float *x, *y, *z, *gx, *gy, *gz;
    double *t1, *t2;
    // variables for data collected from input file
    float pk_threshold = 10;	// pk-threshold value
    /* Variables for peak-trough detection */
    
    float *S_i; 	// indicies of the start of each stride
    int n_S; 	// number of strides

    float *x_accel_max_seg, *x_accel_min_seg, *x_gyro_max_seg, *x_gyro_min_seg;
    float *y_accel_max_seg, *y_accel_min_seg, *y_gyro_max_seg, *y_gyro_min_seg;
    float *z_accel_max_seg, *z_accel_min_seg, *z_gyro_max_seg, *z_gyro_min_seg;
    float *x_accel_mean_seg, *x_gyro_mean_seg;
    float *y_accel_mean_seg, *y_gyro_mean_seg;
    float *z_accel_mean_seg, *z_gyro_mean_seg;
    float *x_accel_std_seg, *x_gyro_std_seg;
    float *y_accel_std_seg, *y_gyro_std_seg;
    float *z_accel_std_seg, *z_gyro_std_seg;
    float *x_accel_mean, *x_gyro_mean;
    float *y_accel_mean, *y_gyro_mean;
    float *z_accel_mean, *z_gyro_mean;
    float *x_accel_std, *x_gyro_std;
    float *y_accel_std, *y_gyro_std;
    float *z_accel_std, *z_gyro_std;
    float *x_accel_skew_seg, *y_accel_skew_seg, *z_accel_skew_seg;
    float *x_gyro_skew_seg, *y_gyro_skew_seg, *z_gyro_skew_seg;
    
    double *time_interval;
    
    int *result_running;
    int *result_runSpeed;
    
    t1 = (double *) malloc(sizeof(double) * default_SAMPLE_Size);
    t2 = (double *) malloc(sizeof(double) * default_SAMPLE_Size);
    x = (float *) malloc(sizeof(float) * default_SAMPLE_Size);
    y = (float *) malloc(sizeof(float) * default_SAMPLE_Size);
    z = (float *) malloc(sizeof(float) * default_SAMPLE_Size);
    gx = (float *) malloc(sizeof(float) * default_SAMPLE_Size);
    gy = (float *) malloc(sizeof(float) * default_SAMPLE_Size);
    gz = (float *) malloc(sizeof(float) * default_SAMPLE_Size);
    S_i = (float *) malloc(sizeof(float) * default_SAMPLE_Size);
    
    x_accel_max_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    x_accel_min_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_accel_max_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_accel_min_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_accel_max_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_accel_min_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    x_gyro_max_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    x_gyro_min_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_gyro_max_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_gyro_min_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_gyro_max_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_gyro_min_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    
    x_accel_mean_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_accel_mean_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_accel_mean_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    x_gyro_mean_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_gyro_mean_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_gyro_mean_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    
    x_accel_std_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_accel_std_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_accel_std_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    x_gyro_std_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_gyro_std_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_gyro_std_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    
    x_accel_skew_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_accel_skew_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_accel_skew_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    x_gyro_skew_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    y_gyro_skew_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    z_gyro_skew_seg = (float *) malloc(sizeof(float) * (FeatureArrSize*4));
    
    x_accel_mean = (float *) malloc(sizeof(float) * FeatureArrSize);
    x_gyro_mean = (float *) malloc(sizeof(float) * FeatureArrSize);
    y_accel_mean = (float *) malloc(sizeof(float) * FeatureArrSize);
    y_gyro_mean = (float *) malloc(sizeof(float) * FeatureArrSize);
    z_accel_mean = (float *) malloc(sizeof(float) * FeatureArrSize);
    z_gyro_mean = (float *) malloc(sizeof(float) * FeatureArrSize);
    
    x_accel_std = (float *) malloc(sizeof(float) * FeatureArrSize);
    x_gyro_std = (float *) malloc(sizeof(float) * FeatureArrSize);
    y_accel_std = (float *) malloc(sizeof(float) * FeatureArrSize);
    y_gyro_std = (float *) malloc(sizeof(float) * FeatureArrSize);
    z_accel_std = (float *) malloc(sizeof(float) * FeatureArrSize);
    z_gyro_std = (float *) malloc(sizeof(float) * FeatureArrSize);
    
    time_interval = (double *) malloc(sizeof(double) * N_SAMPLES);
    
    result_running = (int *) malloc(sizeof(int) * FeatureArrSize);
    result_runSpeed = (int *) malloc(sizeof(int) * FeatureArrSize);
    
    
    
    /* run recognition */
    int N_FEATURE_RUN = 2;
    float *feature_run[N_FEATURE_RUN];
    for(i = 0; i < N_FEATURE_RUN; i++)
    {
        feature_run[i] = (float *) malloc(sizeof(float) * N_SAMPLES);
    }
    int *location_run;
    location_run = (int *) malloc(sizeof(int) * N_SAMPLES);
    /* walk recognition */
    int N_FEATURE_WALK = 3;
    float *feature_walk[N_FEATURE_WALK];
    for(i = 0; i < N_FEATURE_WALK; i++)
    {
        feature_walk[i] = (float *) malloc(sizeof(float) * N_SAMPLES);
    }
    int *location_walk;
    location_walk = (int *) malloc(sizeof(int) * N_SAMPLES);
    /* turn recognition */
    int N_FEATURE_TURN = 2;
    float *feature_turn[N_FEATURE_TURN];
    for(i = 0; i < N_FEATURE_TURN; i++)
    {
        feature_turn[i] = (float *) malloc(sizeof(float) * N_SAMPLES);
    }
    int *location_turn;
    location_turn = (int *) malloc(sizeof(int) * N_SAMPLES);
    /* stair recogniation */
    int N_FEATURE_STAIR = 4;
    float *feature_stair[N_FEATURE_STAIR];
    for(i = 0; i < N_FEATURE_STAIR; i++)
    {
        feature_stair[i] = (float *) malloc(sizeof(float) * N_SAMPLES);
    }
    int *location_stair;
    location_stair = (int *) malloc(sizeof(int) * N_SAMPLES);
    /* backup */
    int N_FEATURE_BACKUP = 3;
    float *feature_backup[N_FEATURE_BACKUP];
    for(i = 0; i < N_FEATURE_BACKUP; i++)
    {
        feature_backup[i] = (float *) malloc(sizeof(float) * N_SAMPLES);
    }
    int *location_backup;
    location_backup = (int *) malloc(sizeof(int) * N_SAMPLES);
    int N_BU;
    /* layer 1 */
    int *location_layer_1;
    location_layer_1 = (int *) malloc(sizeof(int) * N_SAMPLES);
    int N_R; // run
    int N_W; // walk
    int N_T; // turn
    int N_S; // stair
    /* layer 2 */
    /* run speed */
    int N_FEATURE_RUNSPEED = 3;
    float *feature_runspeed[N_FEATURE_RUNSPEED];
    int *location_runspeed;
    /* Walking speed */
    int N_FEATURE_WALKSPEED = 2;
    float *feature_walkspeed[N_FEATURE_WALKSPEED];
    int *location_walkspeed;
    /* Turning direction */
    int N_FEATURE_TURNDIR = 1;
    float *feature_turndir[N_FEATURE_TURNDIR];
    int *location_turndir;
    /* Stair ascent or descent */
    int N_UP;
    int N_DOWN;
    int N_FEATURE_AD = 3;
    int N_FEATURE_UPSPEED = 4;
    int N_FEATURE_DOWNSPEED = 2;
    float *feature_ad[N_FEATURE_AD], *feature_upspeed[N_FEATURE_UPSPEED],
    *feature_downspeed[N_FEATURE_DOWNSPEED];
    int *location_ad;
    /* layer 3 */
    /* Stair ascent speed */
    int N_FEATURE_US = 4;
    float *feature_us[N_FEATURE_US];
    int *location_us;
    /* Stair descent speed */
    int N_FEATURE_DS = 2;
    float *feature_ds[N_FEATURE_DS];
    int *location_ds;
    
    char *display[BUFF_SIZE];
    for(i = 0; i < BUFF_SIZE; i++)
    {
        display[i] = (char *)malloc(sizeof(char) * BUFF_SIZE);
    }
    
    
    
    
    while(run_flag)
    {
        fp = fopen(ifile_name, "r");
        
        usleep(1000);
        
        if (fp != NULL) {
            
            fd = fileno(fp);
            flock(fd, LOCK_EX);
    
            /* count the number of lines in the file */
            read = getline(&line, &len, fp); //discard header of file
            N_Data = 0;
            while ((read = getline(&line, &len, fp)) != -1) {
                N_Data++;
            }
    
            /* go back to the start of the file so that the data can be read */
            rewind(fp);
            read = getline(&line, &len, fp); //discard header of file
    
            /* start reading the data from the file into the data structures */
            i = 0;
    
            while ((read = getline(&line, &len, fp)) != -1) {
                /* parse the data */
                rv = sscanf(line, "%lf,%lf,%f,%f,%f,%f,%f,%f\n", &t1[i], &t2[i], &x[i], &y[i], &z[i], &gx[i], &gy[i], &gz[i]);
                if (rv != 8) {
                    fprintf(stderr,
                            "%s %d \'%s\'. %s.\n",
                            "Failed to read line",
                            i,
                            line,
                            "Exiting"
                            );
                    exit(EXIT_FAILURE);
                }
        
                i++;
            }
            
            fclose(fp);
            
            remove(ifile_name);
            
            printf("removed one file!\n");
            j = extractStrides(gz, t2, N_Data, pk_threshold, &n_S, S_i);
            
            if (j == 0 || n_S == 1)
            {
                printf("No Strides detected\n");
                printf("---------------------------------\n");
                continue;
            }
            extractFeatures(x, y, z, gx, gy, gz, N_Data, S_i, &n_S,
                            x_accel_max_seg, x_accel_min_seg, x_gyro_max_seg, x_gyro_min_seg,
                            y_accel_max_seg, y_accel_min_seg, y_gyro_max_seg, y_gyro_min_seg,
                            z_accel_max_seg, z_accel_min_seg, z_gyro_max_seg, z_gyro_min_seg,
                            x_accel_mean_seg, x_gyro_mean_seg, y_accel_mean_seg, y_gyro_mean_seg,
                            z_accel_mean_seg, z_gyro_mean_seg, x_accel_std_seg, x_gyro_std_seg,
                            y_accel_std_seg, y_gyro_std_seg, z_accel_std_seg, z_gyro_std_seg,
                            time_interval, t2,
                            x_accel_mean, x_gyro_mean, y_accel_mean, y_gyro_mean,
                            z_accel_mean, z_gyro_mean, x_accel_std, x_gyro_std,
                            y_accel_std, y_gyro_std, z_accel_std, z_gyro_std,
                            x_accel_skew_seg, y_accel_skew_seg, z_accel_skew_seg,
                            x_gyro_skew_seg, y_gyro_skew_seg, z_gyro_skew_seg
                            );
            
            /************************/
            /* run recognition */
            N_SAMPLES = n_S - 1;
            for(i = 0; i < N_SAMPLES; i++)
            {
                feature_run[0][i] = x_gyro_std_seg[i*4+1];
                feature_run[1][i] = x_gyro_mean_seg[i*4+2];
            }
            location_predictor("Run.net", N_SAMPLES, N_FEATURE_RUN, 1, feature_run, location_run);

            /* walk recognition */
            for(i = 0; i < N_SAMPLES; i++)
            {
                feature_walk[0][i] = x_accel_mean_seg[i*4+1];
                feature_walk[1][i] = x_gyro_mean_seg[i*4+1];
                feature_walk[2][i] = x_gyro_mean_seg[i*4+3];
            }
            location_predictor("Walk.net", N_SAMPLES, N_FEATURE_WALK, 1, feature_walk, location_walk);
            
            /* turn recognition */
            for(i = 0; i < N_SAMPLES; i++)
            {
                feature_turn[0][i] = z_accel_mean_seg[i*4+3];
                feature_turn[1][i] = y_gyro_mean[i];
            }
            location_predictor("Turn.net", N_SAMPLES, N_FEATURE_TURN, 1, feature_turn, location_turn);
            
            /* stair recogniation */
            for(i = 0; i < N_SAMPLES; i++)
            {
                feature_stair[0][i] = z_accel_mean[i];
                feature_stair[1][i] = y_accel_mean_seg[i*4+3];
                feature_stair[2][i] = x_gyro_mean_seg[i*4+2];
                feature_stair[3][i] = x_accel_mean_seg[i*4+1];
            }
            location_predictor("UpDown.net", N_SAMPLES, N_FEATURE_STAIR, 1, feature_stair, location_stair);
            
            /* backup */
            combination_layer_1(location_run,
                                location_walk,
                                location_turn,
                                location_stair,
                                N_SAMPLES,
                                location_layer_1);
            N_BU = 0;
            j = 0;
            for(i = 0; i < N_SAMPLES; i++)
            {
                if(location_layer_1[i] == -1)
                {
                    feature_backup[0][j] = x_gyro_mean_seg[i*4+2];
                    feature_backup[1][j] = x_gyro_mean_seg[i*4+3];
                    feature_backup[2][j] = y_gyro_mean[i];
                    N_BU++;
                    j++;
                }
            }
            location_predictor2("1_Backup.net", N_BU, N_FEATURE_BACKUP, 4, feature_backup, location_backup);
            
            j = 0;
            for(i = 0; i < N_SAMPLES; i++)
            {
                if(location_layer_1[i] == -1)
                {
                    location_layer_1[i] = location_backup[j];
                    j++;
                }
            }
            
            /***************************************/
            /** determine the location in layer 1 **/
            /***************************************/
            N_R = 0; // run
            N_W = 0; // walk
            N_T = 0; // turn
            N_S = 0; // stair
            for(i = 0; i < N_SAMPLES; i++)
            {
                switch(location_layer_1[i]){
                    case 1:
                        N_R++;
                        display[i] = "---Running---";
                        break;
                    case 2:
                        N_W++;
                        display[i] = "---Walking---";
                        break;
                    case 3:
                        N_T++;
                        display[i] = "---Making a turn---";
                        break;
                    case 4:
                        N_S++;
                        display[i] = "---Stair---";
                        break;
                    default:
                        display[i] = "---Error exists in Layer [1]---";
                }
            }
            
            /***************************************/
            /** determine the location in layer 2 **/
            /***************************************/
            
            /* Running speed */
            if(N_R != 0)
            {
                for(i = 0; i < N_FEATURE_RUNSPEED; i++)
                {
                    feature_runspeed[i] = (float *) malloc(sizeof(float) * N_R);
                }
                j = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 1)
                    {
                        feature_runspeed[0][j] = z_gyro_std_seg[i*4];
                        feature_runspeed[1][j] = y_accel_mean_seg[i*4+1];
                        feature_runspeed[2][j] = z_gyro_min_seg[i*4];
                        j++;
                    }
                }
                location_runspeed = (int *) malloc(sizeof(int) * N_R);
                location_predictor2("2_RunSpeed.net", N_R, N_FEATURE_RUNSPEED, 4, feature_runspeed, location_runspeed);
                j = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 1)
                    {
                        switch(location_runspeed[j]){
                            case 1:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                      "---At Speed of lvl [1]---", strlen("---At Speed of lvl [1]---")+1, sizeof(char));
                                break;
                            case 2:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                      "---At Speed of lvl [2]---", strlen("---At Speed of lvl [2]---")+1, sizeof(char));
                                break;
                            case 3:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                      "---At Speed of lvl [3]---", strlen("---At Speed of lvl [3]---")+1, sizeof(char));
                                break;
                            case 4:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                      "---At Speed of lvl [4]---", strlen("---At Speed of lvl [4]---")+1, sizeof(char));
                            break;
                            default:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                      "---Error exists in Layer [2]---", strlen("---Error exists in Layer [2]---")+1, sizeof(char));
                        }
                        j++;
                    }
                }
                
                for(i = 0; i < N_FEATURE_RUNSPEED; i++)
                {
                    free(feature_runspeed[i]);
                }
                free(location_runspeed);
                
            }
            
            /* Walking speed */
            if(N_W != 0)
            {
                for(i = 0; i < N_FEATURE_WALKSPEED; i++)
                {
                    feature_walkspeed[i] = (float *) malloc(sizeof(float) * N_W);
                }
                j = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 2)
                    {
                        feature_walkspeed[0][j] = y_accel_std[i];
                        feature_walkspeed[1][j] = x_accel_mean_seg[i*4];
                        j++;
                    }
                }
                location_walkspeed = (int *) malloc(sizeof(int) * N_W);
                location_predictor2("2_WalkSpeed.net", N_W, N_FEATURE_WALKSPEED, 4, feature_walkspeed, location_walkspeed);
                
                j = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 2)
                    {
                        switch(location_walkspeed[j]){
                            case 1:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---At Speed of lvl [1]---", strlen("---At Speed of lvl [1]---")+1, sizeof(char));
                                break;
                            case 2:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---At Speed of lvl [2]---", strlen("---At Speed of lvl [2]---")+1, sizeof(char));
                                break;
                            case 3:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---At Speed of lvl [3]---", strlen("---At Speed of lvl [3]---")+1, sizeof(char));
                                break;
                            case 4:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---At Speed of lvl [4]---", strlen("---At Speed of lvl [4]---")+1, sizeof(char));
                                break;
                            default:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---Error exists in Layer [2]---", strlen("---Error exists in Layer [2]---")+1, sizeof(char));
                        }
                        j++;
                    }
                }
                
                for(i = 0; i < N_FEATURE_WALKSPEED; i++)
                {
                    free(feature_walkspeed[i]);
                }
                free(location_walkspeed);
                
            }
            
            /* Turning direction */
            if(N_T != 0)
            {
                for(i = 0; i < N_FEATURE_TURNDIR; i++)
                {
                    feature_turndir[i] = (float *) malloc(sizeof(float) * N_T);
                }
                j = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 3)
                    {
                        feature_turndir[0][j] = x_gyro_mean_seg[i*4+3];
                        j++;
                    }
                }
                location_turndir = (int *) malloc(sizeof(int) * N_T);
                location_predictor2("2_LR.net", N_T, N_FEATURE_TURNDIR, 2, feature_turndir, location_turndir);
                
                j = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 3)
                    {
                        switch(location_turndir[j]){
                            case 1:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                      "---To the right---", strlen("---To the right---")+1, sizeof(char));
                                break;
                            case 2:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                      "---To the left---", strlen("---To the left---")+1, sizeof(char));
                                break;
                            default:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---Error exists in Layer [2]---", strlen("---Error exists in Layer [2]---")+1, sizeof(char));
                        }
                        j++;
                    }
                }
                
                for(i = 0; i < N_FEATURE_TURNDIR; i++)
                {
                    free(feature_turndir[i]);
                }
                free(location_turndir);
                
            }
            
            /* Stair ascent or descent */
            N_UP = 0;
            N_DOWN = 0;
            if(N_S != 0)
            {
                for(i = 0; i < N_FEATURE_AD; i++)
                {
                    feature_ad[i] = (float *) malloc(sizeof(float) * N_S);
                }
                for(i = 0; i < N_FEATURE_UPSPEED; i++)
                {
                    feature_upspeed[i] = (float *) malloc(sizeof(float) * N_S);
                }
                for(i = 0; i < N_FEATURE_DOWNSPEED; i++)
                {
                    feature_downspeed[i] = (float *) malloc(sizeof(float) * N_S);
                }
                j = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 4)
                    {
                        feature_ad[0][j] = y_gyro_mean_seg[i*4+1];
                        feature_ad[1][j] = y_accel_mean_seg[i*4];
                        feature_ad[2][j] = x_accel_mean_seg[i*4+2];
                        feature_upspeed[0][j] = y_accel_max_seg[i*4+2];
                        feature_upspeed[1][j] = z_gyro_mean_seg[i*4+2];
                        feature_upspeed[2][j] = x_accel_mean_seg[i*4+3];
                        feature_upspeed[3][j] = x_accel_mean_seg[i*4];
                        feature_downspeed[0][j] = z_gyro_std[i];
                        feature_downspeed[1][j] = x_accel_mean_seg[i*4];
                        j++;
                    }
                }
                location_ad = (int *) malloc(sizeof(int) * N_S);
                location_predictor("2_UpOrDown.net", N_S, N_FEATURE_AD, 1, feature_ad, location_ad);
                
                j = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 4)
                    {
                        switch(location_ad[j]){
                            case 1:
                                N_UP++;
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---Ascent---", strlen("---Ascent---")+1, sizeof(char));
                                break;
                            case -1:
                                N_DOWN++;
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---Descent---", strlen("---Descent---")+1, sizeof(char));
                                break;
                            default:
                                display[i] = array_concat(display[i], strlen(display[i]),
                                                          "---Error exists in Layer [2]---", strlen("---Error exists in Layer [2]---")+1, sizeof(char));
                        }
                        j++;
                    }
                }
                
                for(i = 0; i < N_FEATURE_AD; i++)
                {
                    free(feature_ad[i]);
                }
                
            }
            
            /***************************************/
            /** determine the location in layer 3 **/
            /***************************************/
            
            /* Stair ascent speed */
            if(N_UP != 0)
            {
                for(i = 0; i < N_FEATURE_US; i++)
                {
                    feature_us[i] = (float *) malloc(sizeof(float) * N_UP);
                }
                j = 0;
                for(i = 0; i < N_S; i++)
                {
                    if(location_ad[i] == 1)
                    {
                        feature_us[0][j] = feature_upspeed[0][i];
                        feature_us[1][j] = feature_upspeed[1][i];
                        feature_us[2][j] = feature_upspeed[2][i];
                        feature_us[3][j] = feature_upspeed[3][i];
                        j++;
                    }
                }
                /*int *location_us;*/
                location_us = (int *) malloc(sizeof(int) * N_UP);
                location_predictor2("3_UpSpeed.net", N_UP, N_FEATURE_US, 3, feature_us, location_us);
                
                j = 0;
                k = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 4)
                    {
                        if(location_ad[j] == 1)
                        {
                            switch(location_us[k]){
                                case 1:
                                    display[i] = array_concat(display[i], strlen(display[i]),
                                                              "---At Speed of lvl [1]---", strlen("---At Speed of lvl [1]---")+1, sizeof(char));
                                    break;
                                case 2:
                                    display[i] = array_concat(display[i], strlen(display[i]),
                                                              "---At Speed of lvl [2]---", strlen("---At Speed of lvl [2]---")+1, sizeof(char));
                                    break;
                                case 3:
                                    display[i] = array_concat(display[i], strlen(display[i]),
                                                              "---At Speed of lvl [3]---", strlen("---At Speed of lvl [3]---")+1, sizeof(char));
                                    break;
                                default:
                                    display[i] = array_concat(display[i], strlen(display[i]),
                                                              "---Error exists in Layer [3]---", strlen("---Error exists in Layer [3]---")+1, sizeof(char));
                            }
                            k++;
                        }
                        j++;
                    }
                }
                
                for(i = 0; i < N_FEATURE_US; i++)
                {
                    free(feature_us[i]);
                }
                free(location_us);
                
            }
            
            /* Stair descent speed */
            if(N_DOWN != 0)
            {
                for(i = 0; i < N_FEATURE_DS; i++)
                {
                    feature_ds[i] = (float *) malloc(sizeof(float) * N_DOWN);
                }
                j = 0;
                for(i = 0; i < N_S; i++)
                {
                    if(location_ad[i] == -1)
                    {
                        feature_ds[0][j] = feature_downspeed[0][i];
                        feature_ds[1][j] = feature_downspeed[1][i];
                        j++;
                    }
                }
                location_ds = (int *) malloc(sizeof(int) * N_DOWN);
                location_predictor2("3_DownSpeed.net", N_DOWN, N_FEATURE_DS, 3, feature_ds, location_ds);
                
                j = 0;
                k = 0;
                for(i = 0; i < N_SAMPLES; i++)
                {
                    if(location_layer_1[i] == 4)
                    {
                        if(location_ad[j] == -1)
                        {
                            switch(location_ds[k]){
                                case 1:
                                    display[i] = array_concat(display[i], strlen(display[i]),
                                                              "---At Speed of lvl [1]---", strlen("---At Speed of lvl [1]---")+1, sizeof(char));
                                    break;
                                case 2:
                                    display[i] = array_concat(display[i], strlen(display[i]),
                                                              "---At Speed of lvl [2]---", strlen("---At Speed of lvl [2]---")+1, sizeof(char));
                                    break;
                                case 3:
                                    display[i] = array_concat(display[i], strlen(display[i]),
                                                              "---At Speed of lvl [3]---", strlen("---At Speed of lvl [3]---")+1, sizeof(char));
                                    break;
                                default:
                                    display[i] = array_concat(display[i], strlen(display[i]),
                                                              "---Error exists in Layer [3]---", strlen("---Error exists in Layer [3]---")+1, sizeof(char));
                            }
                            k++;
                        }
                        j++;
                    }
                }
                
                for(i = 0; i < N_FEATURE_DS; i++)
                {
                    free(feature_ds[i]);
                }
                free(location_ds);
    
            }
            
            /* FREE MEMORY*/
            if(N_S != 0)
            {
                free(location_ad);
                {
                    for(i = 0; i < N_FEATURE_DOWNSPEED; i++)
                    {
                        free(feature_downspeed[i]);
                    }
                    for(i = 0; i < N_FEATURE_UPSPEED; i++)
                    {
                        free(feature_upspeed[i]);
                    }
                }
            }
            
            for(i = 0; i < N_SAMPLES; i++)
            {
                printf("%s\n", display[i]);
            }
            printf("---------------------------------\n");
        }
    }
    return 0;
}
