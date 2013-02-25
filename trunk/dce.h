#ifndef __DEC_H__
#define __DEC_H__

#include <cstdlib>
#include <list>
#include <set>
#include <vector>
#include <string>
#include <utility>
using namespace std;

enum OpType {REG, VAR, NONE};
extern set<int> br_target;

struct Instr {
  int num;
  set<pair<OpType, int> > use;
  set<pair<OpType, int> > def;
  string instr;

  bool populate(string, bool&);
};

struct BasicBlock {
  int num;
  
  set<int> use;  // always refers to C variables
  set<int> def;  // always refers to C variables
  set<int> live_list;  // always refers to C variables
  list<Instr*> instr;
  set<int> children; // indexed by bb number

  int branch_target;
  bool main;

  bool populate();
};

class Function {
private:
  vector<BasicBlock*> bb;

public:
  BasicBlock* get_bb(int num);

  void populate();
  void compute_live();
  void dec();
  void print_CFG();
  void print_instr();
};


pair<OpType, int> get_1op(string instr);
pair<OpType, int> get_2op(string instr);
#endif
