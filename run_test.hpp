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