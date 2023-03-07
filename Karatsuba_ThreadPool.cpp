#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <iomanip>
#include "BS_thread_pool.hpp"
#include "BigInt.hpp"
#define sz(x) (int)x.size()
#define deb(x) cout << #x << ": " << x << endl;
#define ln '\n'
using namespace std;

BS::thread_pool pool;

string to_string(const BigInt &v)
{
  string s = v.s;
  if (!v.pos && v.s != "0") s.push_back('-');
  reverse(s.begin(), s.end());
  return s;
}

ostream& operator<<(ostream &os, const BigInt &v)
{
  return (os << to_string(v));
}

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

  // cout << xh << " " << xl << endl;
  // cout << yh << " " << yl << endl;

  BigInt a = karatsuba(xh, yh);
  BigInt d = karatsuba(xl, yl);
  BigInt e = karatsuba(xh + xl, yh + yl) - a - d;

  BigInt res = a.left_shift(len) + e.left_shift(lower) + d;

  while (res.s.back() == '0') res.s.pop_back();

  return res;
}

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
    a = karatsuba(xh, yh);
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
    d = karatsuba(xl, yl);
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
    e = karatsuba(xh + xl, yh + yl);
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

BigInt multByOneDigit(BigInt x, int v)
{
  BigInt res;

  int carry = 0;

  for (int i = 0; i < x.len(); i++)
  {
    int prod = x[i] * v + carry;
    res.push_back(prod % 10);
    carry = prod / 10;
  }

  if (carry)
    res.push_back(carry);
    
  return res;
}

BigInt gradeschool(BigInt x, BigInt y)
{
  if (x.s == "0") return x;
  if (y.s == "0") return y;

  BigInt res(0);

  for (int i = 0; i < x.len(); i++)
  {
    BigInt m = multByOneDigit(y, x[i]).left_shift(i);
    res = res + m;
  }

  return res;
}

int main(void)
{
  string astr, bstr;

  cout << "Enter first integer: ";
  cin >> astr;

  cout << "Enter second integer: ";
  cin >> bstr;

  BigInt a(astr), b(bstr);

  auto f = pool.submit([](BigInt a, BigInt b)
  {
    return ParallelKaratsuba(a, b);
  }, a, b);

  BigInt res = f.get();

  cout << res << ln;

  return 0;
}