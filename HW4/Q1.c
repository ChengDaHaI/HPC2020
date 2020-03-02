#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

double dboardPI(int darts);

#define MASTER 0
// #define dartsNUM 10*1000*1000

int main(int argc, char * argv[ ])
{
    int taskNUM, taskID;

    int dartsNUM = atoi(argv[1]);

    double pi_task,
            pi_sum,
            pi_avg;

    double time_elapsed,
            time_max;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&taskNUM);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskID);
    printf ("MPI task %d has started...\n", taskID);
    
    srand(taskID);

    pi_avg = 0; 
    double start = MPI_Wtime();
    /* All tasks calculate pi using dartboard algorithm */
    pi_task = dboardPI(dartsNUM);
    double stop = MPI_Wtime();
    time_elapsed = stop - start;
    MPI_Reduce(&time_elapsed, &time_max, 1, MPI_DOUBLE, MPI_MAX,
                    MASTER, MPI_COMM_WORLD);
    MPI_Reduce(&pi_task, &pi_sum, 1, MPI_DOUBLE, MPI_SUM,
                    MASTER, MPI_COMM_WORLD);
    
    /* Master computes average for this iteration and all iterations */
    if (taskID == MASTER) {
        printf ("\nReal value of PI: 3.1415926535897 \n");
    printf("Simulated PI Value is :%f\n", pi_sum/taskNUM);
    printf("Time elapsed:%f seconds\n", time_max);
    }
    MPI_Finalize();
    return 0;

}

double dboardPI(int dartNUM)
{
    int score = 0;
    double coor_x, coor_y;
    for(int i = 0; i < dartNUM; i++)
    {
        coor_x = (double) rand()/(double)RAND_MAX;
        coor_y = (double) rand()/(double)RAND_MAX;
        if ((coor_x * coor_x + coor_y * coor_y) <= 1)
        {
            score ++;
        }
    }
    return 4.0 * ((double) score/(double) dartNUM);
}