#include<iostream>
#include<math.h>


float fact_f(int n)
{
    return (n==1 || n==0) ? 1: n * fact_f(n - 1);
} 

double fact_d(int n)
{
    return (n==1 || n==0) ? 1: n * fact_d(n - 1);
} 
// compute f(x) = x + exp(x) using Taylor series
// exp(x) = 1 + x/1! + x^2/2! + x^3/3! + ...
float TaylorSeries_float(int numSeries, float x)
{
    float fx (x + 1);
    for (int i = 1; i< numSeries; i++)
    {
        fx += powf32(x, i)/fact_f(i);
    }

    return fx;

}

double TaylorSeries_double(int numSeries, double x)
{
    double fx (x + 1);
    for (int i = 1; i< numSeries; i++)
    {
        fx += powf64(x, i)/fact_d(i);
    }

    return fx;

}


int main(int argc, char* argv[])
{

    int n = atoi(argv[1]);


    float x1(2.0), x2(10.0), x3(50.0);
    std::cout<<"For float x = "<< x1 <<", f(x) = " << TaylorSeries_float(n, x1) <<std::endl;
    std::cout<<"For float x = "<< x2 <<", f(x) = " << TaylorSeries_float(n, x2) <<std::endl;
    std::cout<<"For float x = "<< x3 <<", f(x) = " << TaylorSeries_float(n, x3) <<std::endl;
    double y1(2.0), y2(10.0), y3(50.0);
    std::cout<<"For double x = "<< y1 <<", f(x) = " << TaylorSeries_double(n, y1) <<std::endl;
    std::cout<<"For double x = "<< y2 <<", f(x) = " << TaylorSeries_double(n, y2) <<std::endl;
    std::cout<<"For double x = "<< y3 <<", f(x) = " << TaylorSeries_double(n, y3) <<std::endl;
   
   return 0;

}