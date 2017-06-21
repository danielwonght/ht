#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define BUFF_SIZE 1024

//float compute_correlation(float *arr1, float *arr2, int n_sample)
//{
//    // You may need to fill this part
//}

//float compute_merit(float cor, float intercor, int k)
//{
//    // You may need to fill this part
//}

int main(int argc, char **argv){
    /* Generic variables */
    int i, idx;
    int rv;
    /* Variables for reading file line by line */
    char *ifile_name, *ofile_pt_name;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    float *op;
    
    int N_SAMPLES;
    int N_FEATURE;
    int N_OUTPUT = 1;
    
    
    
    
    /*
     * Check if the user entered the correct command line arguments
     * Usage:
     * ./feature_filter <ifile_name> <ofile_name>
     * Or
     * ./feature_filter
     */
    if (argc != 3) {
        ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
        memset(ifile_name, 0, BUFF_SIZE);
        snprintf(ifile_name,
                 BUFF_SIZE,
                 "test11_feature.csv"
                 );
        ofile_pt_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
        memset(ofile_pt_name, 0, BUFF_SIZE);
        snprintf(ofile_pt_name, BUFF_SIZE, "filtered_feature.csv");
    } else {
        ifile_name = argv[1];
        ofile_pt_name = argv[2];
    }
    
    printf("Arguments used:\n\t%s=%s\n\t%s=%s\n",
           "ifile_name", ifile_name,
           "ofile_name", ofile_pt_name
           );
    
    
    /* open the input file */
    printf("Attempting to read from file \'%s\'.\n", ifile_name);
    fp = fopen(ifile_name, "r");
    if (fp == NULL) {
        fprintf(stderr,
                "Failed to read from file \'%s\'.\n",
                ifile_name
                );
        exit(EXIT_FAILURE);
    }
    
    /* count the number of lines in the file */
    read = getline(&line, &len, fp); //discard header of file
    int *temp;
    N_FEATURE = 0;
    temp = (int *) malloc(sizeof(int) * 1);
    rv = sscanf(line, "%d\n", &temp[0]);
    N_FEATURE = temp[0];
    N_SAMPLES = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        N_SAMPLES++;
    }
    
    float *feature_all[N_FEATURE];
    float *feature_correlation[N_FEATURE];
    for(i = 0; i < N_FEATURE; i++)
    {
        feature_all[i] = (float *)malloc(sizeof(float) * N_SAMPLES);
        feature_correlation[i] = (float *)malloc(sizeof(float) * N_FEATURE);
    }
    
    printf("number of sample: %d\n", N_SAMPLES);
    /* go back to the start of the file so that the data can be read */
    rewind(fp);
    read = getline(&line, &len, fp); //discard header of file
    
    /* start reading the data from the file into the data structures */
    i = 0;
    op = (float *) malloc(sizeof(float) * N_SAMPLES);
    const char s[2] = ",";
    char *token;
    int z = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        /* parse the data */
        token = strtok(line, s);
        i = 0;
        while(token != NULL)
        {
            if (i == N_FEATURE)
            {
                rv = sscanf(token, "%f", &op[z]);
            } else {
                rv = sscanf(token, "%f", &feature_all[i][z]);
            }
            if (rv != 1){
                fprintf(stderr,
                        "%s %d \'%s\'. %s.\n",
                        "Failed to read token",
                        i,
                        token,
                        "Exiting"
                        );
                exit(EXIT_FAILURE);
            }
            token = strtok(NULL, s);
            i++;
        }
        z++;
    }
    fclose(fp);
    
    printf("finish reading!\n");
    
    /* create correlation matrix */
    // Write code for creating correlation matrix
    // You can choose to use function compute_correlation
    // You need to modify this part
    
    
    
    printf("finish building correlation matrix!\n");
    
    /* filter features by CFS */
    
//    float *cor_avg, *intercor_avg, *max_layer;
//    int *idx_layer;
//    cor_avg = (float *) malloc(sizeof(float) * N_FEATURE);
//    intercor_avg = (float *) malloc(sizeof(float) * N_FEATURE);
//    idx_layer = (int *) malloc(sizeof(int) * N_FEATURE);
//    max_layer = (float *) malloc(sizeof(float) * N_FEATURE);
//    for (i = 0; i < N_FEATURE; i++) // preset default value in each layer
//    {
//        idx_layer[i] = -1;
//        max_layer[i] = 0.0f;
//        cor_avg[i] = feature_correlation[i][i];
//        intercor_avg[i] = 0.0f;
//    }
//    printf("finish setting default values!\n");
    
    /* You can choose to use the part above to set default values for each variable
     * Or you can have your own way to write code
     */
    
    /* find the highest correlation(merit value) for feature subset of size 1 */
    // You need to modify this part
    
    
    
    printf("finish finding the feature subset of size 1!\n");
    
    /* find the highest merit value for feature subset of size 2 or more */
    // You need to modify this part
    
    
    printf("finish finding the feature subset of size 2 or more!\n");
    
    /* write into a output file */
    printf("Attempting to write to file \'%s\'.\n", ofile_pt_name);
    fp = fopen(ofile_pt_name, "w");
    if (fp == NULL) {
        fprintf(stderr,
                "Failed to write to file \'%s\'.\n",
                ofile_pt_name
                );
        exit(EXIT_FAILURE);
    }
    
    /* start writing */
    fprintf(fp, "%d %d %d\n",
            N_SAMPLES,
            actual_NF,
            N_OUTPUT
            );
    // Write code for writing into output file
    // You can refer to the sample code
    // You need to modify this part
    
    
    
    // Sample code:
//    /* count actual number of features in the selected feature subset */
//    int actual_NF = 0;
//    for(i = 0; i < N_FEATURE; i++)
//    {
//        if(idx_layer[i] != -1)
//        {
//            actual_NF++;
//        }
//    }
//    
//    for (i = 0; i < N_SAMPLES; i++) {
//        for(j = 0; j < actual_NF-1; j++)
//        {
//            fprintf(fp, "%lf ", feature_all[idx_layer[j]][i]);
//        }
//        fprintf(fp, "%lf\n", feature_all[idx_layer[actual_NF-1]][i]);
//        fprintf(fp, "%d\n", (int) op[i]);
//    }
    fclose(fp);
    return 0;
}


