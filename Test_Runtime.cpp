#include <iostream>
#include "Karatsuba_ThreadPool.hpp"
#include "Karatsuba_Semaphore.hpp"
#include "Karatsuba_Uncapped.hpp"
#include "Karatsuba.hpp"
#include <cmath>
using namespace std;

// Test Runtime
// Numbers of digits that are powers of 2, 2^0, 2^1, 2^2, ... 2 ^ 15?
// 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
int ThreadPoolTime(int n);
int SemaphoreTime(int n);
int sequentialTime(int n);
int gradeTime(int n);

int main(void)
{
  for(int i = 0; i < 16; i++){
    int n = pow(2, i);
    cout << "n = " << n << endl;
    cout << "ThreadPool: " << ThreadPoolTime(n) << endl;
    cout << "Semaphore: " << SemaphoreTime(n) << endl;
    cout << "Sequential: " << sequentialTime(n) << endl;
    cout << "Grade: " << gradeTime(n) << endl;
    cout << endl;
  }
}

int ThreadPoolTime(int n)
{
  BigInt a(-n), b(n);
  auto start = std::chrono::high_resolution_clock::now();
  BigInt resTP = Karatsuba_ThreadPool::multiply(a, b);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count();
}

int SemaphoreTime(int n)
{
  BigInt a(-n), b(n);
  auto start = std::chrono::high_resolution_clock::now();
  BigInt resSem = Karatsuba_Semaphore::multiply(a, b);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count();
}

int sequentialTime(int n)
{
  BigInt a(-n), b(n);
  auto start = std::chrono::high_resolution_clock::now();
  BigInt resSeq = Karatsuba::multiply(a, b);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count();
}

int gradeTime(int n)
{
  BigInt a(-n), b(n);
  auto start = std::chrono::high_resolution_clock::now();
  BigInt resGrade = gradeschool(a, b);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count();
}