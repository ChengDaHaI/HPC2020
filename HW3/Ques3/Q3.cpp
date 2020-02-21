#include <vector>
// #include <mutex>
#include <omp.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
// #include <string>
#include <random>
// #include <iomanip>


#define fork omp_lock_t

omp_lock_t print_lock;

struct table
{
   std::atomic<bool>                    ready{ false };
//    std::array<fork, no_of_philosophers> forks;
};

struct fork_id
{
    int             phi_ind;
   table&            dinnertable;
   fork&             left_fork;
   fork&             right_fork;
   int                 left_id;
   int               right_id;
};

std::mt19937      rng{ std::random_device{}() };



void think()
{
    static thread_local std::uniform_int_distribution<> wait(1, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait(rng) * 150));
}

void printStatus(int phi_id, int l_id, int r_id, bool on_off)
{
    omp_set_lock(&print_lock);
    if (on_off)
    {
        std::cout<< "Phi " << phi_id << " starts eating, pick up fork " << l_id << " and fork " << r_id <<std::endl ;
    }
    else
    {
        std::cout<< "Phi " << phi_id << " finishes eating, put down fork " << l_id << " and fork " << r_id <<std::endl ;
    }
    omp_unset_lock(&print_lock);
}

void eat(fork& left_fork, fork& right_fork, int name, int l_id, int r_id)
{
    omp_set_lock(&left_fork);
    omp_set_lock(&right_fork);

    // printStatus(name, l_id, r_id, true);
    static thread_local std::uniform_int_distribution<> dist(1, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng) * 50));
    // printStatus(name, l_id, r_id, false);

    omp_unset_lock(&left_fork);
    omp_unset_lock(&right_fork);

}


void dine(std::vector<fork_id> arg)
{
    std::chrono::time_point<std::chrono::system_clock> start, end; 
    start = std::chrono::system_clock::now(); 
    #pragma omp barrier
    int id = omp_get_thread_num();
    omp_set_lock(&print_lock);
    std::cout<< "Philosopher " <<id << " initialize!"<<std::endl;
    omp_unset_lock(&print_lock);
    struct fork_id * data;
    data = &arg[id];
    table& dinnertable = data->dinnertable;
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 2));
    // std::cout<<"WATCH table for" <<id<< "\n";
    int numEating = 0;
    while (!dinnertable.ready);
    // std::cout<<"table READY for" <<id<< "\n";

    do
    {
        think();
        eat(data->left_fork, data->right_fork, data->phi_ind, data->left_id, data->right_id);
        numEating++;
    } while (numEating < 100);
    end = std::chrono::system_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end - start; 
    omp_set_lock(&print_lock);
    std::cout << "elapsed time for Phi " << id << " is:"  << elapsed_seconds.count() << "s\n";
    omp_unset_lock(&print_lock);
}


void wholeDine(int N_phi)
{

   fork forks[N_phi];

   table table;
   std::vector<fork_id> data_vec;
   for(int i = 0; i< N_phi-1; i++){
     fork_id data = {.phi_ind = i, .dinnertable = table, .left_fork = forks[i], .right_fork = forks[i+1], .left_id = i, .right_id = i+1};
     data_vec.push_back(data);
   }
    fork_id data = {.phi_ind = N_phi-1, .dinnertable = table, .left_fork = forks[N_phi-1], .right_fork = forks[0], .left_id = N_phi-1, .right_id = 0};
    data_vec.push_back(data);

    // for(int i = 0; i<= N_phi-1; i++){
    //     pthread_create(&phiThread[i], NULL, dine,  &data_vec[i]);
    // }

   std::cout << "Dinner started!" << std::endl;
    table.ready = true;
    omp_set_num_threads(N_phi);

   for (int i = 0; i < N_phi; i++)
    {
        omp_init_lock(&forks[i]);
    }
    #pragma omp parallel num_threads(N_phi) shared(data_vec, table)
    {
        dine(data_vec);    
    }
    


    std::cout << "Dinner done!" << std::endl;

    for(int j = 0; j< N_phi; j++)
    {
        omp_destroy_lock(&forks[j]);
    }

}

int main(int argc, char* argv[])
{   

   int N = std::atoi(argv[1]);
   std::cout<<"Number of Philosopher is: "<<N<<std::endl;

   wholeDine(N);

   return 0;
}