#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <sys/time.h>


/* Define globally accessible variables and a mutex */

#define MAX_NUMTHRDS 20
#define MAX_NUM 1000*1000*1000
pthread_t callThd[MAX_NUMTHRDS];
pthread_mutex_t mutexsum;
long sum;
int *Comp;

// long N_number, N_threads;

struct data_stru{
  long id;
  long N;
  long N_thrd; 
};


/*
 function to calculate the total number of primes.
 */
void *setTotalPrimes(void *arg) {
    
    int i, start, end, len ;
    
    long mysum = 0;

    struct data_stru * data;
    data = (struct data_stru *) arg;

    long offset = data->id;
    long N_number = data->N;
    long N_threads = data->N_thrd;
    len = N_number/N_threads;

    start = offset*len + 1;
    if (offset < N_number%N_threads)
        end = start + len;
    else
        {
            end = start + len - 1;
        }
    
    
    /**
     Initialize the portion of Comp
     */
    for (i = start; i <= end; i++) {
        if (i < 3){
            if (i < 2) {
                Comp[i] = 1;
            }else{
                Comp[i] = 0;
            }
        }else{
            if (i % 2 == 0){
                Comp[i] = 1;
            }
        }
    }
    
    /**
     Sieve of Eratoshenes. 
     */
    for (i = start; i <= end; i++) {
        if (i > 2) {
            if (Comp[i] == 0) {
                for (int x = 3 ; x < i; x ++){
                    if ( x*x > i)
                        break;
                    if ( x % 2 != 0 && i % x == 0 ){
                        Comp[i] = 1;
                        break;
                    }
                }
            }
        }
    }
    
    for (i = start; i < end; i++) {
        if (i > 1 && Comp[i] == 0) {
            mysum++;
        }
    }

    
    /*
     Lock a mutex prior to updating the value in the shared
     structure, and unlock it upon updating.
     */
    pthread_mutex_lock (&mutexsum);
    sum += mysum;
    pthread_mutex_unlock (&mutexsum);
    
    
    pthread_exit((void*) 0);
}


double seconds()
{
  timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec + now.tv_usec/1000000.0;
}


int main (int argc, char *argv[])
{
    long i;
    void *status;
    pthread_attr_t attr;
    int input = 0;
    int isLoop = 1;
    
    long N_number  = atoi(argv[1]);
    long N_threads = atoi(argv[2]);

    Comp = (int *)malloc(sizeof(int)*(N_number+1));

    sum = 0;
    
    pthread_mutex_init(&mutexsum, NULL);
        pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    double startTime = seconds();
	for(i=0; i<N_threads; i++)
    {
        struct data_stru *data = (struct data_stru*)malloc(sizeof(struct data_stru));
        data->id = i;
        data->N = N_number;
        data->N_thrd = N_threads;
        pthread_create(&callThd[i], &attr, setTotalPrimes, (void *) data);
	}
    
 	pthread_attr_destroy(&attr);
    
    /* Wait on the other threads */
	for(i=0; i<N_threads; i++)
    {
        pthread_join(callThd[i], &status);
	}
    double joinTime = seconds();
    /* After joining, print out the results */
    printf ("Total number of Prime =  %ld \n", sum);
    // std::cout << "Start time:" << startTime << "\n";
    // std::cout << "joinTime:" << joinTime << "\n";
    std::cout << "Duration:" << joinTime - startTime << "\n";
    
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
    
}
