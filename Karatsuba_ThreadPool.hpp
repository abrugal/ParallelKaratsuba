#pragma once

#include <thread>
#include <future>
#include <vector>
#include <iomanip>
#include "BS_thread_pool.hpp"
#include "BigInt.hpp"
#include "Karatsuba.hpp"
using namespace std;

namespace Karatsuba_ThreadPool
{
  BS::thread_pool pool;

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

    if (pool.get_tasks_total() < pool.get_thread_count())
    {
      spawned[0] = true;

      futures[0] = pool.submit([](BigInt a, BigInt b) {
        return ParallelKaratsuba(a, b);
      }, xh, yh);
    }
    else
    {
      a = Karatsuba::karatsuba(xh, yh);
    }

    if (pool.get_tasks_total() < pool.get_thread_count())
    {
      spawned[1] = true;

      futures[1] = pool.submit([](BigInt a, BigInt b) {
        return ParallelKaratsuba(a, b);
      }, xl, yl);
    }
    else
    {
      d = Karatsuba::karatsuba(xl, yl);
    }

    if (pool.get_tasks_total() < pool.get_thread_count())
    {
      spawned[2] = true;

      futures[2] = pool.submit([](BigInt a, BigInt b) {
        return ParallelKaratsuba(a, b);
      }, xh + xl, yh + yl);
    }
    else
    {
      e = Karatsuba::karatsuba(xh + xl, yh + yl);
    }

    if (spawned[0])
    {
      a = futures[0].get();
    }

    if (spawned[1])
    {
      d = futures[1].get();
    }

    if (spawned[2])
    {
      e = futures[2].get();
    }

    e = e - a - d;

    BigInt res = a.left_shift(len) + e.left_shift(lower) + d;

    while (res.s.back() == '0') res.s.pop_back();

    return res;
  }

  BigInt multiply(BigInt a, BigInt b)
  {
    auto f = pool.submit([](BigInt a, BigInt b)
    {
      return ParallelKaratsuba(a, b);
    }, a, b);

    BigInt res = f.get();
    res.pos = a.pos == b.pos;

    return res;
  }
}
