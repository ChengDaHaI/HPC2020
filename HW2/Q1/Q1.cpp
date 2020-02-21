#include <vector>
#include <mutex>
#include <pthread.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <random>
#include <iomanip>


#define fork std::mutex
// struct fork
// {
//    std::mutex mutex;
// };

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
};

std::mt19937      rng{ std::random_device{}() };

void eat(fork& left_fork, fork& right_fork, int name)
{
    std::lock(left_fork, right_fork);

    std::lock_guard<std::mutex> left_lock(left_fork,   std::adopt_lock);
    std::lock_guard<std::mutex> right_lock(right_fork, std::adopt_lock);

    // std::cout<< "Phi " << name << " started eating.\n";

    static thread_local std::uniform_int_distribution<> dist(1, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng) * 50));

    // std::cout<< "Phi " << name << " finished eating.\n";
}

void think()
{
    static thread_local std::uniform_int_distribution<> wait(1, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait(rng) * 150));

    // std::cout << "is thinking" << std::endl;
}

void *dine(void *arg)
{
	std::chrono::time_point<std::chrono::system_clock> start, end; 
    start = std::chrono::system_clock::now(); 
    struct fork_id * data;
    data = (struct fork_id *) arg;
    table& dinnertable = data->dinnertable;
    fork& l_fork = data->left_fork;
    fork& r_fork = data->right_fork;
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 2));
	int numEating = 0;
    while (!dinnertable.ready);
    
    do
    {
        think();
        eat(l_fork, r_fork, data->phi_ind);
		numEating++;
    } while (numEating < 100);
	end = std::chrono::system_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end - start; 
	std::cout << "elapsed time for Phi " << data->phi_ind << " is:"  << elapsed_seconds.count() << "s\n";
}


void wholeDine(int N_phi)
{
   std::this_thread::sleep_for(std::chrono::seconds(1));
   std::cout << "Dinner started!" << std::endl;

   // fork forks[N_phi];
   std::mutex forks[N_phi];
   pthread_t phiThread[N_phi];

   table table;
   std::vector<fork_id> data_vec;
   for(int i = 0; i< N_phi-1; i++){
     fork_id data = {.phi_ind = i, .dinnertable = table, .left_fork = forks[i], .right_fork = forks[i+1]};
     data_vec.push_back(data);
   }
    fork_id data = {.phi_ind = N_phi-1, .dinnertable = table, .left_fork = forks[N_phi-1], .right_fork = forks[0]};
    data_vec.push_back(data);

    for(int i = 0; i<= N_phi-1; i++){
        pthread_create(&phiThread[i], NULL, dine,  &data_vec[i]);
    }

    std::cout<<"table READY\n";
    table.ready = true;
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    //table.ready = false;
	for(int i = 0; i<= N_phi-1; i++){
        pthread_join(phiThread[i], NULL);
    }
    std::cout << "Dinner done!" << std::endl;
    
}

int main(int argc, char* argv[])
{   

   int N = std::atoi(argv[1]);
   std::cout<<"N = "<<N<<std::endl;
   wholeDine(N);

   return 0;
}
