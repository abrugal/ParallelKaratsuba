#include <iostream>
#include "Karatsuba_ThreadPool.hpp"
#include "Karatsuba_Semaphore.hpp"
#include "Karatsuba_Uncapped.hpp"
#include "Karatsuba.hpp"
#include <cstdlib>
using namespace std;

// Test Correctness
// No need to test big numbers
// Multiple all integers between -1000 and 1000 with each other.
// Maybe include a couple of large integers

int main(void)
{
  // const auto processor_count = std::thread::hardware_concurrency();

  // cout << processor_count;

  for (int i = -100; i <= 100; i++)
  {
    for (int j = -100; j <= 100; j++)
    {
      BigInt a(i), b(j);

      BigInt normal(i * j);
      BigInt seq = Karatsuba::multiply(a, b);
      BigInt sem = Karatsuba_Semaphore::multiply(a, b);
      BigInt thr = Karatsuba_ThreadPool::multiply(a, b);

      if (!(normal == seq && normal == sem && normal == thr))
      {
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

  for (int i = 0; i < 2; i++)
  {
    string first = "", second = "";
    for (int i = 0; i < 1000; i++)
    {
      first += (rand() % 9 + 1) + '0';
      second += (rand() % 9 + 1) + '0';
    }

    BigInt a(first);
    BigInt b(second);

    cout << a << "\n";
    cout << b << "\n";

    BigInt normal = gradeschool(a, b);
    BigInt seq = Karatsuba::multiply(a, b);
    BigInt sem = Karatsuba_Semaphore::multiply(a, b);
    BigInt thr = Karatsuba_ThreadPool::multiply(a, b);

    if (!(normal == seq && normal == sem && normal == thr))
    {
      cout << "FAIL: " << a << " " << b << "\n";
      return 0;
    }
  }

  cout << "SUCCESS\n";
  return 0;
}
