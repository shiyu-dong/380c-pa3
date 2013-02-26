#include <stdio.h>
#define WriteLine() printf("\n");
#define WriteLong(x) printf(" %lld", x);
#define ReadLong(a) if (fscanf(stdin, "%lld", &a) != 1) a = 0;
#define long long long

const long f = 9;
long g;

void test(long z) {
  g = z + f;
}


void main() {
  long a;
  long b;
  long c;
  long d;
  long e;
  
  a = 11;
  b = 7;
  c = a + b;//c = 18
  d = b - c;//d = -11
  c = +a + d + 1;//c = 1;
  d = -b - c;//d = -8
  c = a * d;//c = -88
  c = -c;//c = 88;
  d = c/b;//d=12 ;
  c = d/b;//c = 1;
  d = -b / c;//d = -7;
  test(d);

  WriteLong(c);
  WriteLine();
  WriteLong(d);
  WriteLine();
  WriteLong(f);
  WriteLine();
  WriteLong(g);
  WriteLine();


//out: 1 -7 9 2

}
