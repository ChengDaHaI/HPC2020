#include<iostream>
#include<vector>
#include<pthread.h>
#include<semaphore.h>

sem_t lock;
int count (0);
std::vector<int> nums;

void* countFunc(void* args)
{
    int * data;
    data = (int*) args;
    int start =  data[0];
    int end = data[1];

    int n (0);
    std::vector<int> v;
    for(int i = start; i <= end; i++)
    {
        if (i%3 == 0 or i%5 == 0)
        {
            n++;
            v.push_back(i);
        }
    }
    sem_wait(&lock);
    count += n;
    nums.insert(nums.end(), v.begin(), v.end());
    sem_post(&lock);
}

void printNUM(std::vector <int> vec)
{
    for (std::vector<int>::const_iterator i = vec.begin(); i != vec.end(); ++i)
        std::cout << *i << '\t';
    std::cout<<std::endl;
}

int main()
{
    int num_th = 8;
    int data[num_th][2];
    int start(1), end(5000);
    int interval = (end - start + 1)/num_th;
    
    for(int i = 0; i < num_th; i++)
    {
        data[i][0] = i * interval + 1;
        data[i][1] = (i+1)*interval;
    }

    pthread_t pthd[num_th];
    sem_init(&lock, 0, 1);
    for (int j = 0; j < num_th; j++)
    {
        pthread_create(&pthd[j], NULL, countFunc, &(data[j]));
    }

    for (int j = 0; j < num_th; j++)
    {
        pthread_join(pthd[j], NULL);
    }

    sem_destroy(&lock);

    std::cout<<"Total number is:" << count << std::endl;
    std::cout<<"Found numbers value are:"<< std::endl;
    printNUM(nums);

}