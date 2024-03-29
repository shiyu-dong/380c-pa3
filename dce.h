#ifndef __DEC_H__
#define __DEC_H__

#include <cstdlib>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <utility>
using namespace std;

//#define DEBUG_1
#define DEBUG_0

enum OpType {REG, VAR, NONE};
extern set<int> br_target;
struct Function;

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

  bool populate(string, bool&, int&, map<int, int>&);
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
  bool populate(int&, map<int, int>&);

  // DCE
  void compute_defuse();
  bool dce(Function*, map<int, int>&, set<int>&);
  inline void add_instr_def(list<Instr*>::iterator);
  inline void add_instr_use(list<Instr*>::iterator);
};

struct Function {
  vector<BasicBlock*> bb;
  int local_size;
  map<int, int> ref_map;
  set<int> dead_var_offset;

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
  void rename_operand(string&);
  void rename();
  int next_instr_num(int);
};


pair<OpType, int> get_1op(string instr, map<int, int>&);
pair<OpType, int> get_2op(string instr, map<int, int>&, bool);
#endif
