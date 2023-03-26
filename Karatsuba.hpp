#pragma once

#include "BigInt.hpp"
using namespace std;

namespace Karatsuba
{
  // Sequential
  BigInt karatsuba(BigInt x, BigInt y)
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

    BigInt a = karatsuba(xh, yh);
    BigInt d = karatsuba(xl, yl);
    BigInt e = karatsuba(xh + xl, yh + yl) - a - d;

    BigInt res = a.left_shift(len) + e.left_shift(lower) + d;

    while (res.s.back() == '0') res.s.pop_back();

    return res;
  }

  BigInt multiply(BigInt a, BigInt b)
  {
    return karatsuba(a, b);
  }
}