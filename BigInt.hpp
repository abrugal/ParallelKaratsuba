#pragma once

#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include <mutex>
#include <semaphore>
#include <vector>
#include <chrono>
#include <iomanip>
#define sz(x) (int)x.size()
#define deb(x) cout << #x << ": " << x << endl;
#define ln '\n'
using namespace std;

struct BigInt
{
  string s;
  bool pos = true;

  BigInt() {}

  BigInt(string s)
  {
    this->s = string(s.rbegin(), s.rend());
    if (!isdigit(this->s.back()))
    {
      if (this->s.back() == '-')
        pos = false;
      this->s.pop_back();
    }
  }

  BigInt(int v)
  {
    if (!v) s = "0";
    if (v < 0)
    {
      pos = false;
      v = -v;
    }
    while (v)
    {
      this->push_back(v % 10);
      v /= 10;
    }
  }

  BigInt operator+(BigInt o)
  {
    if (this->pos != o.pos)
    {
      // 5 -3
      // -5 3
      bool sign;
      BigInt a = *this;
      BigInt b = o;

      bool signa = a.pos;
      bool signb = b.pos;

      a.pos = b.pos = true;

      BigInt res = a - b;

      if (res.s != "0")
        if (a > b)
          res.pos = signa;
        else
          res.pos = signb;
      
      return res;
    }
    else
    {
      BigInt a, b;

      if (this->len() >= o.len())
        a.s = this->s, b.s = o.s;
      else
        b.s = this->s, a.s = o.s;

      int carry = 0, i;

      for (i = 0; i < b.len(); i++)
      {
        int sum = carry + a[i] + b[i];
        a.set(i, sum % 10);
        carry = sum / 10;
      }

      for (; carry; i++)
      {
        if (i < a.len()) 
        {
          int sum = carry + a[i];
          a.set(i, sum % 10);
          carry = sum / 10;
        }
        else
        {
          a.push_back(carry);
          carry = 0;
        }
      }

      a.pos = this->pos;

      return a;
      
    }
  }

  BigInt operator-(BigInt o)
  {
    if (pos != o.pos)
    {
      BigInt res;
      this->pos = !pos;
      res = *this + o;
      this->pos = !pos;
      res.pos = this->pos;
      return res;
    }
    else
    {
      BigInt big = *this;
      BigInt small = o;

      big.pos = small.pos = true;

      if (small > big)
      {
        swap(small, big);
        big.pos = !this->pos;
      }
      else
        big.pos = this->pos;


      int carry = 0, i = 0;

      for (i = 0; i < small.len(); i++)
      {
        int sub = big[i] - small[i] - carry;

        if (sub < 0)
        {
          carry = 1;
          sub += 10;
        }
        else
          carry = 0;

        big.set(i, sub);
      }

      for (; carry; i++)
      {
        int sub = big[i] - carry;

        if (sub < 0)
        {
          carry = 1;
          sub += 10;
        }
        else
          carry = 0;

        big.set(i, sub);
      }

      while (sz(big.s) > 1 && big.s.back() == '0')
        big.s.pop_back();

      if (big.s == "0")
        big.pos = true;

      return big;
    }
  }

  BigInt left_shift(int k)
  {
    BigInt res = *this;
    reverse(res.s.begin(), res.s.end());
    while (k--) res.s.push_back('0');
    reverse(res.s.begin(), res.s.end());
    return res;
  }

  void push_back(int v)
  {
    s.push_back(v + '0');
  }

  void set(int i, int v)
  {
    s[i] = v + '0';
  }

  int len() const
  {
    return sz(s);
  }

  int operator[](int idx) const
  {
    return s[idx] - '0';
  }

  bool operator<(const BigInt &o) const
  {
    if (!pos && o.pos) return true;
    if (pos && !o.pos) return false;
    if (len() < o.len()) return pos;
    if (len() > o.len()) return !pos;

    for (int i = o.len() - 1; i >= 0; i--)
      if ((*this)[i] != o[i])
      {
        bool flag = (*this)[i] < o[i];
        if (!pos) flag = !flag;
        return flag;
      }

    return false;
  }

  bool operator>(const BigInt &o) const
  {
    return o < *this;
  }

  bool operator==(const BigInt &o) const
  {
    if (s == "0" && o.s == "0")
      return true;
    return pos == o.pos && s == o.s;
  }
  
};