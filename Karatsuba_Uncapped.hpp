// First Parallel Karatsuba
// Spawns threads always
// Slower than sequential Karatsuba

#pragma once

#include <thread>
#include <future>
#include <semaphore>
#include "BigInt.hpp"
using namespace std;

namespace Karatsuba_Uncapped
{
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

    promise<BigInt> pa;
    auto fa = pa.get_future();
    thread ta([](promise<BigInt> &&p, BigInt a, BigInt b) {
      p.set_value(ParallelKaratsuba(a, b));
    }, move(pa), xh, yh);

    promise<BigInt> pd;
    auto fd = pd.get_future();
    thread td([](promise<BigInt> &&p, BigInt a, BigInt b) {
      p.set_value(ParallelKaratsuba(a, b));
    }, move(pd), xl, yl);
    
    promise<BigInt> pe;
    auto fe = pe.get_future();
    thread te([](promise<BigInt> &&p, BigInt a, BigInt b) {
      p.set_value(ParallelKaratsuba(a, b));
    }, move(pe), xh + xl, yh + yl);

    ta.join();
    td.join();
    te.join();

    BigInt a = fa.get();
    BigInt d = fd.get();
    BigInt e = fe.get() - a - d;

    BigInt res = a.left_shift(len) + e.left_shift(lower) + d;

    while (res.s.back() == '0') res.s.pop_back();

    return res;
  }

  BigInt multiply(BigInt a, BigInt b)
  {
    promise<BigInt> p;
    auto f = p.get_future();
    thread t([](std::promise<BigInt> &&p, BigInt a, BigInt b) {
      p.set_value(ParallelKaratsuba(a, b));
    }, move(p), a, b);

    t.join();

    BigInt res = f.get();
    res.pos = a.pos == b.pos;

    return res;
  }
}