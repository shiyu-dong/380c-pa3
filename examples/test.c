#include <stdio.h>
#define WriteLine() printf("\n");
#define WriteLong(x) printf(" %lld", x);
#define ReadLong(a) if (fscanf(stdin, "%lld", &a) != 1) a = 0;
#define long long long

long n;

void main() {

    long a;
    long b;
    long i;
    long j;

    i = 0;//live
    j = 5;//live
    a = 0;//dead
    b = 0;//dead
    n = 10;
    if (i < 0) {
      if (j < 0) {
        n = 5;//live
      }
      else {
        while (j > 0) {
          j = j - 1;//live
          a = 3;//dead
        }
      }
    }
    else {
      if (j < 0) {
        b = 10;//dead
      }
      else {
          a = 8;
          j = j - 1;//live
          b = 8;//dead
          n = 6;//live
      }
    }
    i = 5;
    j = 10;
}
