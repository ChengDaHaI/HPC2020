#include <stdlib.h>
#include <stdio.h>

#define FILENAME "./dblp-co-authors.txt"
#define NumAuthor 317080
#define DataLen   1049866
#define BlockSize 1024 
#define GridSize  int(DataLen/BlockSize) + 1
#define MAX 343
#define newGridSize int(NumAuthor/BlockSize) + 1
int dataset[DataLen * 2];// array to store the raw dataset

void dataset_read(int * dataset);
__global__ void dataset_parse(int * dataset, int * output,  int * full_output);
//int dataset_maxCoAuthor(int * output, int lenght);
//void dataset_plot(int * output, int lenght, int max);
__global__ void output_parse(int * full_output, int * output, int * num_author_array);
__device__ void check_pair(int * full_output, int start, int len, int possible_pair, int * pair_array, int * pair_len);
__device__ void count_diff_auth(int * pair_array, int * new_array,int pair_len, int * pure_len, int indx);

int main(int argc, char * argv[])
{
    int output[NumAuthor] = {0};
    int full_output[MAX * NumAuthor] = { 0 };
    int * cu_output;//array to store the co-authors number of each author
    int * cu_full_output;

    dataset_read(dataset);

    // Set device that we will use for our cuda code
    cudaSetDevice(0);
    
    // Time Variables
    cudaEvent_t start, stop;
    cudaEventCreate (&start);
    cudaEventCreate (&stop);
    float time;
    
    int * cu_dataset;
    cudaEventRecord(start,0);
    cudaMalloc((void**)&cu_output, NumAuthor * sizeof(int));
    cudaMalloc((void**)&cu_full_output, MAX * NumAuthor * sizeof(int));
    cudaMalloc((void**)&cu_dataset, DataLen * 2 * sizeof(int));
    cudaMemcpy(cu_dataset, dataset, DataLen * 2 * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(cu_full_output, full_output, MAX * NumAuthor  * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(cu_output, output, NumAuthor  * sizeof(int), cudaMemcpyHostToDevice);

    dataset_parse<<<GridSize, BlockSize>>>(cu_dataset, cu_output, cu_full_output);
    cudaDeviceSynchronize();
    cudaMemcpy(output, cu_output, NumAuthor  * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(full_output, cu_full_output, MAX * NumAuthor  * sizeof(int), cudaMemcpyDeviceToHost);
    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);  
    //int max = dataset_maxCoAuthor(output, NumAuthor);
    //printf("Time elapsed: %f\n", time);
    
    int * cu_num_author_array;
    int * num_author_array = (int*)malloc(NumAuthor * sizeof(int));
    cudaMalloc((void**)&cu_num_author_array, NumAuthor * sizeof(int));
    cudaMemset(cu_num_author_array, 0, NumAuthor * sizeof(int));
    
    cudaEventRecord(start,0);
    output_parse<<<newGridSize, BlockSize>>>(cu_full_output, cu_output, cu_num_author_array); 
    cudaDeviceSynchronize();
    printf("Error in Kernel output_parse:%s\n",cudaGetErrorString(cudaGetLastError()));    
    cudaDeviceSynchronize();
    cudaMemcpy(num_author_array, cu_num_author_array, NumAuthor  * sizeof(int), cudaMemcpyDeviceToHost);
    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);
    printf("Elapsed time in Kernel output_parse: %f\n", time);
    int total_number = 0;
    for (int i = 0; i < NumAuthor; i++)
         total_number += num_author_array[i];
    printf("Total number of authors is %d\n", total_number/2);
    return 0;
}

void dataset_read( int * dataset)
{
    FILE * datafile;
    datafile = fopen( FILENAME, "r");
    char line[255];
    while (true)
    {
        fscanf(datafile, "%s", line);
        if (atoi(line) == 1)
        {
            dataset[0] = 1;
            break;
        }
    }
    for(int i = 1; i < 2 * DataLen; i++){
        fscanf(datafile, "%d", &dataset[i]);
    }
    fclose(datafile);
}

__global__ void dataset_parse(int * dataset, int * output, int * full_output)
{
    int indx = threadIdx.x + blockIdx.x * blockDim.x;
    int i, j;
     if(indx < DataLen){   
        i = atomicAdd(&(output[dataset[2*indx]-1]), 1);
        full_output[(dataset[2*indx]-1) * MAX + i] = dataset[2*indx+1];         
        j = atomicAdd(&(output[dataset[2*indx+1]-1]), 1);
        full_output[(dataset[2*indx + 1]-1) * MAX + j] = dataset[2*indx];
}
}

/*
int dataset_maxCoAuthor(int * output, int lenght)
{
    int max =0;
    int max_num = 0;
    int max_ind[1000] = { 0 };
    //memset(max_ind, 0, 1000);
    for(int i = 0; i < lenght; i++)
    {
        //printf("output:%d, %d", i, output[i]);
        if(max < output[i])
        {
           // printf("Max right now:%d, %d\n", i, output[i]);
            max = output[i];
            max_num = 0;
            memset(max_ind, 0, 1000);
            max_ind[max_num] = i;
        }
        else if(max == output[i])
        {
            max_num++;
            max_ind[max_num] = i;
        }
        //else{
        //printf("max is:%d, %d\n", max, max_ind[0]);
         //}
    }
    printf("The  list of authors with most co-authors:\n");
    for(int i = 0; i <= max_num; i++)
    {
    printf("Author: %6d has %6d co-authors.\n", max_ind[i] + 1, output[max_ind[i]]);
    }
    return output[max_ind[0]];
}

void dataset_plot(int * output, int lenght, int max)
{
    //int* numCoAuthorList;
    int* numCoAuthorList = (int*)malloc(max * sizeof(int));
    memset(numCoAuthorList, 0, max);
    for(int i = 0; i < lenght; i++)
    {
        if(output[i] <= max)
        {
            numCoAuthorList[output[i] - 1]++;
        }
        else{
            printf("\nError in Finding MAX!!!\n");
        }
    }
    
    FILE *fp;

    fp = fopen("./output.txt", "wb");

    fwrite(numCoAuthorList, sizeof(int), max, fp);
    fclose(fp);

}
*/

__global__ void output_parse(int * full_output, int * output, int * num_author_array)
{
    int indx = threadIdx.x + blockIdx.x * blockDim.x;
    if(indx < NumAuthor){
        int pair_array[10000] = { 0 };
        int pair_len = 0;
        int coauthor, coauthor_co_len, possible_pair;
        for(int i = 0; i < output[indx]; i++){
            coauthor = full_output[indx * MAX + i];    
            coauthor_co_len = output[coauthor-1];
            for(int j = 0; j < coauthor_co_len; j++){
                possible_pair = full_output[(coauthor - 1) * MAX + j];
                check_pair(full_output, indx * MAX, output[indx], possible_pair, pair_array, &pair_len);
                }
        }

        //int * new_array = (int*)malloc(pair_len * sizeof(int));
        //memset(new_array, 0, pair_len );
        int new_array[10000] = {0};
        int num_authors = 0;
        count_diff_auth(pair_array, new_array, pair_len, &num_authors, indx);
        num_author_array[indx] = num_authors; 
    }
}

__device__ void check_pair(int * full_output, int start, int len, int possible_pair, int * pair_array, int * pair_len){
    for(int i = 0; i < len; i++){
        if(full_output[start + i] == possible_pair){
            pair_array[*pair_len] = possible_pair;
            (*pair_len)++;
            break;
        }        
    }         
}

__device__ void count_diff_auth(int * pair_array,int * new_array, int pair_len, int * pure_len, int indx){
    //int * new_array;
    //cudaMalloc((void**)&new_array, pair_len * sizeof(int));
    //cudaMemset(new_array, 0, pair_len * sizeof(int));
    //printf("pair_array[0]:%d, pair_len:%d, %d\n",pair_array[0], pair_len, indx);
    new_array[0] = pair_array[0];
    *pure_len = 1;
    for(int i = 1; i < pair_len; i++){
        int j;
        for(j = 0; j< *pure_len; j++){
            if(pair_array[i] == new_array[j]){
                break;
            }
        }
        
        if (j == *pure_len){
            new_array[*pure_len] = pair_array[i];
            (*pure_len)++;
        }  
    }
}
