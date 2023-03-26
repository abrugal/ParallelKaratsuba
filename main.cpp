#include <iostream>
#include "Karatsuba_ThreadPool.hpp"
#include "Karatsuba_Semaphore.hpp"
#include "Karatsuba_Uncapped.hpp"
#include "Karatsuba.hpp"

using namespace std;

int main(void)
{
  BigInt a("5"), b("12");

  BigInt resTP = Karatsuba_ThreadPool::multiply(a, b);
  cout << resTP << endl;

  BigInt resSem = Karatsuba_Semaphore::multiply(a, b);
  cout << resSem << endl;

  BigInt resSeq = Karatsuba::multiply(a, b);
  cout << resSeq << endl;

  BigInt resBad = Karatsuba_Uncapped::multiply(a, b);
  cout << resBad << endl;

  BigInt resGrade = gradeschool(a, b);
  cout << resGrade << endl;

  return 0;
}