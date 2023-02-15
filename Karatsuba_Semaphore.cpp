#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include <mutex>
#include <semaphore>
#include <vector>
#include <chrono>
#include <iomanip>
#include "BigInt.hpp"
#define sz(x) (int)x.size()
#define deb(x) cout << #x << ": " << x << endl;
#define ln '\n'
using namespace std;

counting_semaphore sem(12);

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
    a = karatsuba(xh, yh);
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
    d = karatsuba(xl, yl);
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
    e = karatsuba(xh + xl, yh + yl);
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

void run_tests()
{
  for (int i = -1000; i <= 1000; i++)
  {
    for (int j = -1000; j <= 1000; j++)
    {
      BigInt a(i), b(j);
      BigInt subres = a - b;
      BigInt addres = a + b;
      
      if (to_string(subres) != to_string(i - j) ||
          to_string(addres) != to_string(i + j))
      {
        cout << "FAIL: " << i << " " << j << endl;
        cout << endl;
        goto DONE;
      }

      if (i >= 0 && j >= 0)
      {
        BigInt prodres = karatsuba(a, b);

        if (to_string(prodres) != to_string(i * j))
        {
          cout << "FAIL: " << i << " " << j << endl;
          cout << endl;
          goto DONE;
        }
      }
    }
  }

  cout << "PASS" << endl;

DONE:;
}

int main(void)
{
  string s = "";

  for (int i = 0; i < 20000; i++)
  {
    s += (i % 9 + 1) + '0';
  }

  BigInt big(s);
  int trials = 1;
  int64_t total_time = 0;
  
  auto go = [&]()
  {
    auto start = chrono::high_resolution_clock::now();

    sem.acquire();

    promise<BigInt> p;
    auto f = p.get_future();
    thread t([](std::promise<BigInt> &&p, BigInt big) {
      BigInt a = big;
      BigInt b = big;

      p.set_value(ParallelKaratsuba(a, b));
    }, move(p), big);

    t.join();
    BigInt res = f.get();

    auto stop = chrono::high_resolution_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
    double seconds = ms / 1000.0;

    // cout << fixed << setprecision(5) << seconds << ln;

    total_time += ms;
  };

  for (int i = 0; i < trials; i++)
    go();

  cout << "avg: " << ((double)total_time / trials) / 1000 << endl;

  return 0;
}