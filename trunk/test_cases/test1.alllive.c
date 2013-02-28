#include <stdio.h>
#define WriteLine() printf("\n");
#define WriteLong(x) printf(" %lld", x);
#define ReadLong(a) if (fscanf(stdin, "%lld", &a) != 1) a = 0;
#define long long long

long n;
long dead1;
long dead2;
long live1;
long live2;
long deadlive;

void main() {
  dead1 = 0;
  dead2 = 0;
  live1 = 0;
  live2 = 0;
  deadlive = 0;

  while (live1 < 10) {
    dead1 = live1 * 2;
    dead2 = live1 - 1;
    live1 = live1 +1;
    n = live1;
  }

  //test for empty block
  if (live2 == 0){
    dead1 = 5;
  }
  else {
    dead2 = 10;
  } 

  //test for deadlive
  deadlive = 1;
  n = deadlive;
  deadlive = deadlive * 2;
  deadlive = 3;

  
}
