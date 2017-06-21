#include "decision_maker.h"

void location_predictor(char *network_name, int N_SAMPLES, int N_feature, int N_output, float **feature, int *location)
{
    struct fann *ann;
    int i, j;
    fann_type *calc_out;
    fann_type input[N_feature];
    ann = fann_create_from_file(network_name);
    for(i = 0; i< N_SAMPLES; i++)
    {
        location[i] = -1;
    }
    for (i = 0; i < N_SAMPLES; i++)
    {
        for(j = 0; j < N_feature; j++)
        {
            input[j] = feature[j][i];
        }
        calc_out = fann_run(ann, input);
        for (j = 0; j < N_output; j++){
            if (calc_out[j] > 0)
            {
                location[i] = j+1;
            }
        }
    }
}

void location_predictor2(char *network_name, int N_SAMPLES, int N_feature, int N_output, float **feature, int *location)
{
    struct fann *ann;
    int i, j;
    fann_type *calc_out;
    fann_type input[N_feature];
    ann = fann_create_from_file(network_name);
    float max;
    for(i = 0; i< N_SAMPLES; i++)
    {
        location[i] = -1;
    }
    for (i = 0; i < N_SAMPLES; i++)
    {
        for(j = 0; j < N_feature; j++)
        {
            input[j] = feature[j][i];
        }
        calc_out = fann_run(ann, input);
        max = -1.1f;
        for (j = 0; j < N_output; j++){
            if (calc_out[j] > max)
            {
                location[i] = j+1;
                max = calc_out[j];
            }
        }
    }
}

void combination_layer_1(int *run, int *walk, int *turn, int *stair, int N_SAMPLES, int *location_L1)
{
    int i;
    for(i = 0; i < N_SAMPLES; i++)
    {
        location_L1[i] = -1;
    }
    for(i = 0; i < N_SAMPLES; i++)
    {
        if (run[i] == 1){
            location_L1[i] = 1;
        } else if (walk[i] == 1){
            location_L1[i] = 2;
        } else if (turn[i] == 1){
            location_L1[i] = 3;
        } else if (stair[i] == 1){
            location_L1[i] = 4;
        }
    }
}
