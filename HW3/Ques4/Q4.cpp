#include <iostream>
#include <cstdlib>
#include <omp.h>

#define N 512
omp_lock_t print_lock;


int OMP_matMulplyVec(int** matrix, int* vector, int* result)
{
   int i,j;

#pragma omp parallel shared(matrix,result,vector) private(i,j) 
   {
       int id =  omp_get_thread_num();
       omp_set_lock(&print_lock);
       std::cout<<"Thread ID:" << id <<std::endl;
       omp_unset_lock(&print_lock);
#pragma omp for  schedule(static)
   for (i=0; i<N; i=i+1){
      
      int sum = 0 ;
      #pragma omp parallel for reduction(+: sum)
      for (j=0; j<N; j=j+1){
         sum=(sum)+((matrix[i][j])*(vector[j]));
      }
      result[i]=sum;
   }
   }
   return 0;
}

int main()
{
    int **Mat;
    int Vec[N];
    int* Prod;

    srand(0);

    Prod = new int[N];
    Mat = new int*[N];
    for(int i = 0; i <N; i++)
        Mat[i] = new int[N];
    for(int i = 0; i< N; i++)
    {
        Vec[i] = rand() % 10 + 1;
        for(int j = 0; j< N; j++)
        {
            Mat[i][j] = rand() % 10 + 1;
        }
    }
    
    int max_num = omp_get_max_threads();
    std::cout<<" Available number of processor: " << max_num << std::endl;

    int N_thrd = max_num;
    int interval = N / N_thrd;

    OMP_matMulplyVec(Mat, Vec, Prod);

    std::cout << "Product vector is: \n";
    for(int i = 0; i< N; i++)
    {
        std::cout << Prod[i] << "\t";
    }

}
