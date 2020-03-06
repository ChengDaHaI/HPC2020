#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

#define minInt  1
#define maxInt  1000

void printData(int N, int binNUM, int taskNUM, int * binTotal, int * binCount, int *bined_dataset);
void generateData(int * dataset, int N);
void histogram(int * dataset, int N, int binNUM, int * binSize, int * binedData, int taskID);


int main(int argc, char * argv[])
{
    int binNUM = atoi(argv[1]);
    int N      = atoi(argv[2]);
    int print_flag = atoi(argv[3]);

    int *dataset = new int[N];
    int *bined_dataset = new int[N];

    int * binSize = new int[binNUM];
    int * binCount = new int[binNUM];

    

    double time_elapsed,time_max;
    int taskNUM, taskID;


    srand(0);
    generateData(dataset, N);
    // double time_elapsed, time_max;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&taskNUM);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskID);

    if (N % taskNUM)
    {
        printf("Non equal data size for each node #rank %d\n", taskNUM);
        MPI_Finalize();
        return 0;
    }
    int * local_data = new int[N/taskNUM];
    int * binedData = new int[N/taskNUM];
    int * binTotal = new int [binNUM*taskNUM];




    printf ("MPI task %d has started...\n", taskID);


    double start = MPI_Wtime();

    

    MPI_Scatter(dataset, N/taskNUM, MPI_INT, local_data, N/taskNUM, MPI_INT, 0, MPI_COMM_WORLD);
    // if (taskNUM == 0)
    //     free(dataset);
    // histogram(dataset, N, binNUM, binSize, bined_dataset);
    histogram(local_data, N/taskNUM, binNUM, binSize, binedData, taskID);
    // if (taskID == 3)
    //         printf ("Step 3 with index \n");
    
    MPI_Reduce(binSize , binCount, binNUM, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Gather(binSize, binNUM, MPI_INT, binTotal, binNUM, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Gather(binedData, N/taskNUM, MPI_INT, bined_dataset, N/taskNUM, MPI_INT, 0, MPI_COMM_WORLD);

    double stop = MPI_Wtime();
    time_elapsed = stop - start;

    MPI_Reduce(&time_elapsed, &time_max, 1, MPI_DOUBLE, MPI_MAX, 0,MPI_COMM_WORLD);

    if (taskID == 0 and print_flag == 1)
    {
        // for(int i = 0; i< binNUM; i++)
        // printf("%4d\t", binCount[i]);
        // for(int i = 0; i< N; i++)
        // printf("%4d\t", dataset[i]);
        // for(int i = 0; i< N; i++)
        // printf("%4d\t", bined_dataset[i]);
        // for(int i = 0; i< binNUM*taskNUM; i++)
        // printf("%4d\t", binTotal[i]);
        // printData(N, binNUM,  taskNUM, binTotal,  binCount, bined_dataset);
        
    }

    if (taskID == 0)
        printf("Elapsed time: %4f\n", time_max);

    MPI_Finalize();

    return 0;

}


void generateData(int * dataset, int N)
{
    for(int i =0; i < N; i++)
    {
        dataset[i] = rand() % maxInt + 1;
    }
}

void printData(int N, int binNUM, int taskNUM, int * binTotal, int * binCount, int *bined_dataset)
{

    for(int i =0; i < binNUM; i++)
        {
            printf("\n%d Integers in Bin # %d :\n",binCount[i], i+1);
            for(int j = 0; j < taskNUM; j++)
            {
                int ind = 0;
                for(int kk=0; kk < i; kk++)
                    ind += binTotal[j*binNUM + kk];
                for(int k = 0; k < binTotal[j*binNUM + i]; k++)
                {
                    printf("%4d\t", bined_dataset[j*N/taskNUM + ind + k]);

                }

            }
        }
    printf("\n");
}

void histogram(int * dataset, int N, int binNUM, int * binSize, int * binedData, int taskID)
{
    int interval = maxInt / binNUM;
    int binInd;
    int * tempData [binNUM];
    for(int i =0; i < binNUM; i++)
        tempData[i] = new int[N];

    
    for(int i = 0; i < N; i++)
    {
        binInd = dataset[i]/interval;
        // output->binSize[binInd]++;
        tempData[binInd][binSize[binInd]++] = dataset[i];
        // printf("dataset[i]:%d, %d, %d",dataset[i], binInd, output->binSize[binInd]);
    }

    int index = 0;

    for(int j = 0; j < binNUM; j++)
    {
        for(int k = 0; k < binSize[j]; k++)
        {
        binedData[index++] = tempData[j][k];
        // if (taskID == 3)
        // printf ("Step 3 with index = %d\n",index);
        }
    }

    
    // free(tempData);
}
