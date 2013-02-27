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
struct Function;

// type 1
// 1 reg def + 2 use
#define DEF_REG1_SIZE 8
string def_reg1[] = {"add", "sub", "mul", "div", "mod", 
                     "cmpeq", "cmple", "cmplt"};
// type 2
// 1 reg def + 1 use of the only op
#define DEF_REG2_SIZE 2
string def_reg2[] = {"neg", "load"};
// type 3
// 0 reg def + 2 use
#define DEF_REG3_SIZE 1
string def_reg3[] = {"store"};
// type 4
// 0 def + 1st use of two ops
// need to check BB boundary
#define DEF_REG4_SIZE 2
string def_reg4[] = {"blbc", "blbs"};
// type 5
// 1 def + 1 use, define in 2nd operand position
#define DEF_REG5_SIZE 1
string def_reg5[] = {"move"};
// type 6
// 0 def + 1 use of the only op
#define DEF_REG6_SIZE 2
string def_reg6[] = {"write", "param"};
// type 7, branches whose destination might be changed
// br, blbs, blbc
// type 8, ret, call, enter, entrypc, read, wrl, nop won't be deleted and depend on nothing
#define BB_END_SIZE 5
string bb_end[] = {"br", "blbc", "blbs", "ret", "call"};


inline int instr_num(string instr) {
  int pos1 = instr.find("instr ")+6;
  int len = instr.find(":")-pos1;
  
  return atoi(instr.substr(pos1, len).c_str());
}


struct Instr {
  int num;
  set<pair<OpType, int> > use;
  set<pair<OpType, int> > def;
  string instr;

  bool populate(string, bool&, int&);
};

struct BasicBlock {
  int num;
  
  set<int> use;  // always refers to C variables
  set<int> def;  // always refers to C variables
  set<int> live_list;  // always refers to C variables
  list<Instr*> instr;
  set<int> children; // indexed by bb number, used in population only
  set<BasicBlock*> children_p; // children pointers
  set<BasicBlock*> parent_p; // children pointers
  set<int> children_live; // live C variables of children

  int branch_target;
  bool main;

  // CFG
  bool populate(int&);

  // DCE
  void compute_defuse();
  bool dce(Function*, set<int>&);
  inline void add_instr_def(list<Instr*>::iterator);
  inline void add_instr_use(list<Instr*>::iterator);
};

struct Function {
  vector<BasicBlock*> bb;
  set<int> dead_var_offset;
  int local_size; // negative number..

  BasicBlock* get_bb(int);

  // CFG
  void populate();
  void print_CFG();
  void print_instr();

  // DCE
  void compute_live();
  bool compute_bb_live(int);
  void dce();
  void reconnect();
  void rename();
  int next_instr_num(int);
};


pair<OpType, int> get_1op(string instr);
pair<OpType, int> get_2op(string instr);
#endif
