#include <bits/stdc++.h>
#include "BigInt.hpp"

using namespace std;

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

int main(void)
{
  // auto my_future = pool.submit([]()
  // {
  //   return 65;
  // });


  // // my_future.wait();

  // // .get() waits
  // cout << my_future.get() << endl;

  return 0;
}