#include <iostream>
#include "Karatsuba_ThreadPool.hpp"
#include "Karatsuba_Semaphore.hpp"
#include "Karatsuba_Uncapped.hpp"
#include "Karatsuba.hpp"
using namespace std;

// Test Correctness
// No need to test big numbers
// Multiple all integers between -1000 and 1000 with each other.
// Maybe include a couple of large integers

int main(void)
{

  // const auto processor_count = std::thread::hardware_concurrency();

  //cout << processor_count;

  for (int i = -1000; i <= 1000; i++)
  {
    for (int j = -1000; j <= 1000; j++)
    {
      BigInt a(i), b(j);

      BigInt normal = gradeschool(a, b);
      BigInt seq = Karatsuba::karatsuba(a, b);
      BigInt sem = Karatsuba_Semaphore::ParallelKaratsuba(a, b);
      BigInt thr = Karatsuba_ThreadPool::ParallelKaratsuba(a, b);

      if (!(normal == seq && normal == sem && normal == thr)){
        cout << "FAIL: " << i << " " << j << "\n";
        return 0;
      }
      /*
      else{
        cout << "PASS: " << i << " " << j << "\n";
      }
      */
    }
  }
  cout << "SUCCESS\n";
  return 0;
}
