#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

#define minInt  1
#define maxInt  1000

void printData(int N, int binNUM, int taskNUM, int * binCount, int *binedData);
void generateData(int * dataset, int N);
void histogram(int * dataset, int N, int binIND, int & binSize, int * binedData, int binNUM);


int main(int argc, char * argv[])
{
    int binNUM = atoi(argv[1]);
    int N      = atoi(argv[2]);
    int print_flag = atoi(argv[3]);

    int *bined_dataset;

    // int * binSize;
    int * binCount = new int[binNUM]();
    // int * local_data;
    int * binedData = new int [N * binNUM];
    // for(int i =0; i < binNUM; i++)
    //     binedData[i] = new int[N]();
    // int * binTotal;

    double time_elapsed,time_max;
    int taskNUM, taskID;


    
    // double time_elapsed, time_max;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&taskNUM);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskID);

    if (binNUM % taskNUM)
    {
        printf("Non equal bin number and node number\n");
        MPI_Finalize();
        return 0;
    }
    int *dataset = new int[N];
    int binSize = 0;
    bined_dataset = new int[N] ();
    // local_data = new int[N/taskNUM]();
    // binedData = new int *[N]();
    // binTotal = new int [binNUM*taskNUM]();

    printf ("MPI task %d has started...\n", taskID);

    if (taskID == 0)
    {
        srand(0);
        generateData(dataset, N);
    }


    double start = MPI_Wtime();


    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(dataset, N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    int binIND = taskID;
    histogram(dataset,  N,  binIND, binSize, bined_dataset,  binNUM);
    // histogram(local_data, N, binNUM, binSize, binedData, taskID);
    MPI_Barrier(MPI_COMM_WORLD);
    // MPI_Reduce(binSize , binCount, binNUM, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Gather(&binSize, 1, MPI_INT, binCount, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Gather( bined_dataset , N, MPI_INT, binedData, N, MPI_INT, 0, MPI_COMM_WORLD);

    double stop = MPI_Wtime();
    time_elapsed = stop - start;

    MPI_Reduce(&time_elapsed, &time_max, 1, MPI_DOUBLE, MPI_MAX, 0,MPI_COMM_WORLD);

    if (taskID == 0 and print_flag == 1)
    {

        printData(N, binNUM,  taskNUM,  binCount, binedData);
        
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

void printData(int N, int binNUM, int taskNUM, int * binCount, int *binedData)
{
    // for(int i =0; i < binNUM * N; i++)
    //     printf("%4d\t", binedData[0][i]);
    for(int i =0; i < binNUM; i++)
        {
            printf("\n%d Integers in Bin # %d :\n",binCount[i], i+1);
            for(int j = 0; j < binCount[i]; j++)
            {
                    printf("%4d\t", binedData[i * N + j]);
            }
        }
    printf("\n");
}

void histogram(int * dataset, int N, int binIND, int & binSize, int * binedData, int binNUM)
{
    int interval = maxInt / binNUM;

    for(int i = 0; i < N; i++)
    {
        if (binIND == (dataset[i] - 1)/interval)
        {
            binedData[binSize++] = dataset[i];
            // if (binIND == 3)
                // printf("dataset[i]:%d, binedData: %d, %d\n", dataset[i], binedData[binSize-1], binSize-1);
            // binSize = binSize + 1;

        }
        
        // binSize[binInd]++;
    }

    // printf("bin #%d Size: %d\n", binIND, binSize);
    // for(int i =0; i < binSize; i++)
    //     printf("%4d\t", binedData[i]);
    // printf("\n");

}
