#include <bits/stdc++.h>
#include "BigInt.hpp"

using namespace std;

// ThreadPool
// int main(void)
// {
//   string s = "";

//   for (int i = 0; i < 20000; i++)
//   {
//     s += (i % 9 + 1) + '0';
//   }

//   BigInt big(s);
//   int trials = 1;
//   int64_t total_time = 0;

//   auto go = [&]()
//   {
//     auto start = chrono::high_resolution_clock::now();

//     auto f = pool.submit([](BigInt a, BigInt b)
//     {
//       return ParallelKaratsuba(a, b);
//     }, big, big);

//     BigInt res = f.get();

//     auto stop = chrono::high_resolution_clock::now();
//     auto ms = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
//     double seconds = ms / 1000.0;

//     total_time += ms;
//   };

//   for (int i = 0; i < trials; i++)
//     go();

//   cout << "avg: " << ((double)total_time / trials) / 1000 << endl;

//   return 0;
// }

// Semaphore
// int main(void)
// {
//   string s = "";

//   for (int i = 0; i < 20000; i++)
//   {
//     s += (i % 9 + 1) + '0';
//   }

//   BigInt big(s);
//   int trials = 1;
//   int64_t total_time = 0;
  
//   auto go = [&]()
//   {
//     auto start = chrono::high_resolution_clock::now();

//     sem.acquire();

//     promise<BigInt> p;
//     auto f = p.get_future();
//     thread t([](std::promise<BigInt> &&p, BigInt big) {
//       BigInt a = big;
//       BigInt b = big;

//       p.set_value(ParallelKaratsuba(a, b));
//     }, move(p), big);

//     t.join();
//     BigInt res = f.get();

//     auto stop = chrono::high_resolution_clock::now();
//     auto ms = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
//     double seconds = ms / 1000.0;

//     // cout << fixed << setprecision(5) << seconds << ln;

//     total_time += ms;
//   };

//   for (int i = 0; i < trials; i++)
//     go();

//   cout << "avg: " << ((double)total_time / trials) / 1000 << endl;

//   return 0;
// }