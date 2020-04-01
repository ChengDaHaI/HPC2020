#include <stdlib.h>
#include <stdio.h>

#define FILENAME "./dblp-co-authors.txt"
#define NumAuthor 317080
#define DataLen   1049866
#define BlockSize 1024
#define GridSize  int(DataLen/BlockSize) + 1

int dataset[DataLen * 2];// array to store the raw dataset

void dataset_read(int * dataset);

__global__ void dataset_parse(int * dataset, int * output);

int dataset_maxCoAuthor(int * output, int lenght);

void dataset_plot(int * output, int lenght, int max);

int main(int argc, char * argv[])
{
    int output[NumAuthor] = { 0 };
    int * cu_output;//array to store the co-authors number of each author

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
    cudaMalloc((void**)&cu_output, NumAuthor * sizeof(int) );
    cudaMalloc((void**)&cu_dataset, DataLen * 2 * sizeof(int));
    cudaMemcpy(cu_dataset, dataset, DataLen * 2 * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(cu_output, output, NumAuthor  * sizeof(int), cudaMemcpyHostToDevice);
    
    dataset_parse<<<GridSize, BlockSize>>>(cu_dataset, cu_output);
    cudaDeviceSynchronize();
    //cudaEventSynchronize(stop);

    //cudaEventElapsedTime(&time, start, stop);
    cudaMemcpy(output, cu_output, NumAuthor  * sizeof(int), cudaMemcpyDeviceToHost);
   
    cudaEventRecord(stop,0);
    cudaEventElapsedTime(&time, start, stop);  
    int max = dataset_maxCoAuthor(output, NumAuthor);
    printf("Time elapsed: %f\n", time);
    dataset_plot(output, NumAuthor, max);
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
    for(int i = 1; i < NumAuthor; i++){
        fscanf(datafile, "%d", &dataset[i]);
    }
    fclose(datafile);
}

__global__ void dataset_parse(int * dataset, int * output)
{
    int indx = threadIdx.x + blockIdx.x * blockDim.x;
     if(indx < DataLen){   
        atomicAdd(&(output[dataset[2*indx]-1]), 1);
        atomicAdd(&(output[dataset[2*indx+1]-1]), 1);
        //if (dataset[2*indx]-1 >= 315280)
        //    printf("index: %6d author:%6d output:%6d\n", indx,dataset[2*indx]-1, output[dataset[2*indx]-1]);
        //if (dataset[2*indx+1]-1 >= 315280)
        //    printf("index: %6d author:%6d output:%6d\n", indx,dataset[2*indx+ 1]-1, output[dataset[2*indx+1]-1]);
}
}

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
    memset(numCoAuthorList, 0, sizeof(numCoAuthorList));
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

    fwrite(numCoAuthorList, sizeof(int), sizeof(numCoAuthorList), fp);
    fclose(fp);

}
