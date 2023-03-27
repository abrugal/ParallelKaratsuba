#include <iostream>
#include "Karatsuba_ThreadPool.hpp"
#include "Karatsuba_Semaphore.hpp"
#include "Karatsuba_Uncapped.hpp"
#include "Karatsuba.hpp"
#include <cmath>
#include <fstream>
#include <chrono>

using namespace std;

// Test Runtime
// Numbers of digits that are powers of 2, 2^0, 2^1, 2^2, ... 2 ^ 15?
// 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768

int ThreadPoolTime(BigInt a, BigInt b);
int SemaphoreTime(BigInt a, BigInt b);
int sequentialTime(BigInt a, BigInt b);
int gradeTime(BigInt a, BigInt b);

int main(void)
{
  // file output
  ofstream csv;
  ofstream textfile;
  csv.open("output.csv");
  textfile.open("output.txt");
  csv << "n,ThreadPool,Semaphore,Sequential,Grade" << endl;

  for (int i = 0; i < 16; i++)
  {
    string s = "";
    int n = pow(2, i);

    // makes n number of digits
    for (int x = 0; x < n; x++)
    {
      s += (x % 9 + 1) + '0';
    }

    BigInt big(s);
    BigInt a = big;
    BigInt b = big;

    int tp = ThreadPoolTime(a, b);
    int sem = SemaphoreTime(a, b);
    int seq = sequentialTime(a, b);
    int grade = gradeTime(a, b);
    csv << n << "," << tp << "," << sem << "," << seq << "," << grade << endl;

    textfile << "n = " << n << endl;
    textfile << "ThreadPool: " << tp << endl;
    textfile << "Semaphore: " << sem << endl;
    textfile << "Sequential: " << seq << endl;
    textfile << "Grade: " << grade << endl;
    textfile << endl;
  }
}

int ThreadPoolTime(BigInt a, BigInt b)
{
  auto start = std::chrono::high_resolution_clock::now();
  BigInt resTP = Karatsuba_ThreadPool::multiply(a, b);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count();
}

int SemaphoreTime(BigInt a, BigInt b)
{
  auto start = std::chrono::high_resolution_clock::now();
  BigInt resSem = Karatsuba_Semaphore::multiply(a, b);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count();
}

int sequentialTime(BigInt a, BigInt b)
{
  auto start = std::chrono::high_resolution_clock::now();
  BigInt resSeq = Karatsuba::multiply(a, b);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count();
}

int gradeTime(BigInt a, BigInt b)
{
  auto start = std::chrono::high_resolution_clock::now();
  BigInt resGrade = gradeschool(a, b);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count();
}