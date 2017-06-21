#ifndef decision_maker_h
#define decision_maker_h


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "floatfann.h"

#define BUFF_SIZE 1024
void location_predictor(char *network_name, int N_SAMPLES, int N_feature, int N_output, float **feature, int *location);

void location_predictor2(char *network_name, int N_SAMPLES, int N_feature, int N_output, float **feature, int *location);

void combination_layer_1(int *run, int *walk, int *turn, int *stair, int N_SAMPLES, int *location_L1);

#endif
