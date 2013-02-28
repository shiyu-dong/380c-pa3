#include <stdio.h>
#define WriteLine() printf("\n");
#define WriteLong(x) printf(" %lld", x);
#define ReadLong(a) if (fscanf(stdin, "%lld", &a) != 1) a = 0;
#define long long long


const long m = 4;
const long n = 3;
long m1[m][n], m2[m][n];//should be always live
long m3[m][n];
long i, j, k;

struct A {
  long x, y;
} globala, locala;

void main()
{

  i = 0;
  //test global and local array;
  while (i < m) {
    j = 0;
    while (j < n) {
      m1[i][j] = i+j*2;
      m2[i][j] = -m1[i][j];
      m3[i][j] = i*2+j;
      j = j + 1;
    }
  }

  //test global struct and local struct

  k = 0;
  while (k < 10) {
    //dead struct
    locala.x = k;
    locala.y = -k;
    //live struct
    globala.x = k;
    globala.y = -k;

    k = k + 1;
  }
  
  //test empty block loop
  while (k > 0) {
    while (k > 1) {
      while (k > 2) {
        if (k < 3) {}
          //do nothing
        else {}
          //do nothing
      }
    }
  }

}
