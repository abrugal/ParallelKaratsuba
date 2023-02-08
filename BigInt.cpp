#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include <mutex>
// #include <semaphore>
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

  // cout << xh << " " << xl << endl;
  // cout << yh << " " << yl << endl;

  // thread t1([](){

  // });

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

  // BigInt a = karatsuba(xh, yh);
  // BigInt d = karatsuba(xl, yl);
  // BigInt e = karatsuba(xh + xl, yh + yl) - a - d;

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

  for (int i = 0; i < 100; i++)
  {
    s += (i % 9 + 1) + '0';
  }

  BigInt big(s);

  // BigInt res1 = karatsuba(big, big);

  // cout << res1 << ln;

  std::promise<BigInt> p;
  auto f = p.get_future();
  std::thread t([](std::promise<BigInt> &&p, BigInt big) {
    BigInt a = big;
    BigInt b = big;

    p.set_value(ParallelKaratsuba(a, b));
  }, std::move(p), big);

  t.join();

  BigInt res = f.get();

  cout << res << ln;

  return 0;
}