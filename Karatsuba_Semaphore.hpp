#pragma once

#include <thread>
#include <future>
#include <semaphore>
#include <vector>
#include <iomanip>
#include "BigInt.hpp"
#include "Karatsuba.hpp"
using namespace std;

namespace Karatsuba_Semaphore
{
  counting_semaphore sem(12);

  BigInt ParallelKaratsuba(BigInt x, BigInt y)
  {
    if (x.s == "0") return x;
    if (y.s == "0") return y;

    int len = max(x.len(), y.len());
    if (len > 1 && (len & 1)) len++;

    while (x.len() < len) x.push_back(0);
    while (y.len() < len) y.push_back(0);

    if (len == 1)
    {
      return x[0] * y[0];
    }

    int upper = len / 2;
    int lower = len - upper;

    BigInt xh, xl, yh, yl;
    xh.s = x.s.substr(lower, upper);
    xl.s = x.s.substr(0, lower);

    yh.s = y.s.substr(lower, upper);
    yl.s = y.s.substr(0, lower);

    BigInt a, d, e;
    vector<bool> spawned(3);
    vector<thread> t(3);
    vector<future<BigInt>> futures(3);
    vector<promise<BigInt>> promises(3);
    
    for (int i = 0; i < 3; i++)
      futures[i] = promises[i].get_future();

    if (sem.try_acquire())
    {
      spawned[0] = true;

      t[0] = thread([](promise<BigInt> &&p, BigInt a, BigInt b) {
        p.set_value(ParallelKaratsuba(a, b));
      }, move(promises[0]), xh, yh);
    }
    else
    {
      a = Karatsuba::karatsuba(xh, yh);
    }

    if (sem.try_acquire())
    {
      spawned[1] = true;

      t[1] = thread([](promise<BigInt> &&p, BigInt a, BigInt b) {
        p.set_value(ParallelKaratsuba(a, b));
      }, move(promises[1]), xl, yl);
    }
    else
    {
      d = Karatsuba::karatsuba(xl, yl);
    }

    if (sem.try_acquire())
    {
      spawned[2] = true;

      t[2] = thread([](promise<BigInt> &&p, BigInt a, BigInt b) {
        p.set_value(ParallelKaratsuba(a, b));
      }, move(promises[2]), xh + xl, yh + yl);
    }
    else
    {
      e = Karatsuba::karatsuba(xh + xl, yh + yl);
    }

    if (spawned[0])
    {
      t[0].join();
      a = futures[0].get();
    }

    if (spawned[1])
    {
      t[1].join();
      d = futures[1].get();
    }

    if (spawned[2])
    {
      t[2].join();
      e = futures[2].get();
    }

    e = e - a - d;

    BigInt res = a.left_shift(len) + e.left_shift(lower) + d;

    while (res.s.back() == '0') res.s.pop_back();

    sem.release();

    return res;
  }

  BigInt multiply(BigInt a, BigInt b)
  {
    sem.acquire();
    promise<BigInt> p;
    auto f = p.get_future();
    thread t([](std::promise<BigInt> &&p, BigInt a, BigInt b) {
      p.set_value(ParallelKaratsuba(a, b));
    }, move(p), a, b);

    t.join();

    BigInt res = f.get();
    res.pos = a.pos == b.pos;
    
    sem.release();
    return res;
  }
}
