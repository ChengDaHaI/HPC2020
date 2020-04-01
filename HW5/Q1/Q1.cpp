#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

#define FILENAME "./dblp-co-authors.txt"
#define NumAuthor 317080
#define DataLen   1049866

int dataset[DataLen * 2];// array to store the raw dataset

void dataset_read(int * dataset);

void dataset_parse(int * dataset, int * output, int lenght);

int dataset_maxCoAuthor(int * output, int lenght);

void dataset_plot(int * output, int lenght, int max);

int main(int argc, char * argv[])
{
    int output[NumAuthor];//array to store the co-authors number of each author

    dataset_read(dataset);

    int taskNUM, taskID;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&taskNUM);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskID);

    printf ("MPI task %d has started...\n", taskID);
    int * local_dataset;
    double start = MPI_Wtime();
    int * local_outpt = new int[NumAuthor]();
    // MPI_Scatter(dataset, 2 * DataLen/taskNUM, MPI_INT, local_dataset, 2 * DataLen/taskNUM, MPI_INT, 0, MPI_COMM_WORLD);//TODO: change data format from int to array
    if (taskID == 0){
        local_dataset = new int[2 * DataLen/taskNUM]();
        local_dataset = dataset;
        for(int i = 1; i < taskNUM -1; i++){
            // printf("Start Send for ID:%d\n", i);
            MPI_Send(dataset + i * 2 * DataLen/taskNUM , 2 * DataLen/taskNUM, MPI_INT, i, 0, MPI_COMM_WORLD);
            // printf("FInish Send for ID:%d\n", i);
        }
        // printf("Start Send for ID:%d\n", taskNUM - 1);
        MPI_Send(dataset + (taskNUM -1) * 2 * DataLen/taskNUM , 2 * DataLen - (taskNUM -1) * 2 * DataLen/taskNUM, MPI_INT, taskNUM -1, 0, MPI_COMM_WORLD);
        // printf("FInish Send for ID:%d\n", taskNUM - 1);
    }
    else{
        if (taskID < taskNUM -1)
        {
            local_dataset = new int[2 * DataLen/taskNUM]();
            MPI_Recv(local_dataset, 2 * DataLen/taskNUM, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("FInish Recv for ID:%d\n", taskID);
        }
        else{
            local_dataset = new int[2 * DataLen - (taskNUM -1) * 2 * DataLen/taskNUM]();
            MPI_Recv(local_dataset, 2 * DataLen - (taskNUM -1) * 2 * DataLen/taskNUM, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("FInish Recv for ID:%d\n", taskID);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (taskID < taskNUM -1){
        dataset_parse(local_dataset, local_outpt, 2 * DataLen/taskNUM);
    }
    else{
    dataset_parse(local_dataset, local_outpt, 2 * DataLen - (taskNUM -1) * 2 * DataLen/taskNUM);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(local_outpt , output, NumAuthor, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double stop = MPI_Wtime();
    double time_elapsed = stop - start;
    double time_max;
    MPI_Reduce(&time_elapsed, &time_max, 1, MPI_DOUBLE, MPI_MAX, 0,MPI_COMM_WORLD);

    if (taskID == 0)
    {
        int max = dataset_maxCoAuthor(output, NumAuthor);
        dataset_plot(output, NumAuthor, max);
        printf("Time elapsed: %f\n", time_max);
    }

    MPI_Finalize();
    return 0;
}


void dataset_read( int * dataset)
{
    FILE * datafile;
    datafile = fopen( FILENAME, "r");
    char line[255];
    // int char_num = 0;
    while (true)
    {
        fscanf(datafile, "%s", line);
        // printf("line: %s\n", line);
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

void dataset_parse(int * dataset, int * output, int lenght)
{
    for(int i = 0; i < lenght; i+=2)
    {
        output[dataset[i] - 1]++;
        output[dataset[i+1]-1]++;
    }
}

int dataset_maxCoAuthor(int * output, int lenght)
{
    int max, max_num = 0;
    int max_ind[1000];
    for(int i = 0; i < lenght; i++)
    {
        if(max < output[i])
        {
            max = output[i];
            max_num = 0;
            max_ind[max_num] = i;
        }
        else if(max == output[i])
        {
            max_num++;
            max_ind[max_num] = i;
        }
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
    int numCoAuthorList[max] = {0};
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
    int total = 0;
    for(int i =0; i< max; i++)
        total +=  numCoAuthorList[i];
    printf("Total author:%d", total);
    FILE *fp;

    fp = fopen("./output.txt", "wb");

    fwrite(numCoAuthorList, sizeof(int), sizeof(numCoAuthorList), fp);
    fclose(fp);

}
