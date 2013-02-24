#include "dec.h"
#include <iostream>
#include <vector>
#include <assert.h>
using namespace std;

int main(int argc, char* argv[]) {
  // argv[2] optimize
  // argv[3] print format

  int j=0;
  myEOF = 0;
  vector<Function*> functions;

  assert(argc == 3);

  // build CFG
  while(!myEOF) {
    functions.push_back(new Function);
    functions[j]->populate();
    j++;
  }

  if(!strcmp(argv[1], "-opt=dce")) {
    for(int i=0; i<functions.size(); i++) {
      functions[i]->compute_live();
      functions[i]->dec();
    }
  }

  if(!strcmp(argv[2], "-backend=3addr")) {
    for(int i=0; i<functions.size(); i++)
      functions[i]->print_instr();
    return 0;
  }
  else {
    for(int i=1; i<functions.size()-1; i++)
      functions[i]->print_CFG();
    return 0;
  }

  return 0;
}
