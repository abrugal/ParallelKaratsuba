import java.util.*;
import java.math.BigInteger;

public class TestBigInteger
{
  public static void main(String[] args)
  {
    String s = "";
    StringBuilder str = new StringBuilder();

    for (int i = 0; i < 10000; i++)
      str.append((char)((i % 9 + 1) + '0'));

    BigInteger a = new BigInteger(str.toString());

    BigInteger res = a.multiply(a);

    System.out.println(res.toString());
  }
}