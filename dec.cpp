#include <assert.h>
#include <iostream>
#include "dec.h"

bool myEOF;
set<int> br_target;

// type 1, 1 reg def + 2 use
#define DEF_REG1_SIZE 8
string def_reg1[] = {"add", "sub", "mul", "div", "mod", 
                     "cmpeq", "cmple", "cmplt"};
// type 2, 1 reg def + 1 use
#define DEF_REG2_SIZE 2
string def_reg2[] = {"neg", "load"};
// type 3, 0 reg def + 2 use
#define DEF_REG3_SIZE 1
string def_reg3[] = {"store"};
// type 4, 0 def + 1 use
#define DEF_REG4_SIZE 4
string def_reg4[] = {"blbc", "blbs", "write", "param"};
// type 5, 1 def + 1 use, define in 2nd operand position
#define DEF_REG5_SIZE 1
string def_reg5[] = {"move"};
// type 6, branches whose destination might be changed
// br, blbs, blbc
// type 7, ret, call, eneter, entrypc, read, wrl, nop won't be deleted and depend on nothing
#define BB_END_SIZE 6
string bb_end[] = {"br", "blbc", "blbs", "ret", "call", "nop"};

inline int instr_num(string instr) {
  int pos1 = instr.find("instr ")+6;
  int len = instr.find(":")-pos1;
  
  return atoi(instr.substr(pos1, len).c_str());
}

pair<OpType, int> get_1op(string instr) {
  int pos1, pos2, pos3;
  string op1str;
  pos2 = instr.find_last_of(" ");
  pos1 = instr.find_last_of(" ", pos2-1);
  op1str = instr.substr(pos1+1, pos2-pos1-1);
  // check if it is a reg
  pos1 = op1str.find('(');
  if (pos1 != string::npos) {
    int reg_num = atoi(op1str.substr(pos1+1, op1str.length()-1).c_str());
    //cout<<"op1: "<<reg_num<<endl;
    return make_pair(REG, reg_num);
  }

  // check if it is a branch target
  pos1 = op1str.find('[');
  if (pos1 != string::npos) {
    int reg_num = atoi(op1str.substr(pos1+1, op1str.length()-1).c_str());
    //cout<<"op2: "<<reg_num<<endl;
    return make_pair(NONE, reg_num);
  }

  // check if it is a var
  pos1 = op1str.find("#");
  pos2 = op1str.find("_base");
  pos3 = op1str.find("_offset");
  if (pos1 != string::npos && pos2 == string::npos && pos3 == string::npos) {
    int reg_num = atoi(op1str.substr(pos1+1, op1str.length()-1).c_str());
    //cout<<"op1: "<<reg_num<<endl;
    return make_pair(VAR, reg_num);
  }

  return make_pair(NONE, -1);
}

pair<OpType, int> get_2op(string instr) {
  int pos1, pos2, pos3;
  string op1str;
  pos2 = instr.length();
  pos1 = instr.find_last_of(" ");
  op1str = instr.substr(pos1+1, pos2-pos1-1);
  // check if it is a reg
  pos1 = op1str.find('(');
  if (pos1 != string::npos) {
    int reg_num = atoi(op1str.substr(pos1+1, op1str.length()-1).c_str());
    //cout<<"op2: "<<reg_num<<endl;
    return make_pair(REG, reg_num);
  }

  // check if it is a branch target
  pos1 = op1str.find('[');
  if (pos1 != string::npos) {
    int reg_num = atoi(op1str.substr(pos1+1, op1str.length()-1).c_str());
    //cout<<"op2: "<<reg_num<<endl;
    return make_pair(NONE, reg_num);
  }

  // check if it is a var
  pos1 = op1str.find("#");
  pos2 = op1str.find("_base");
  pos3 = op1str.find("_offset");
  if (pos1 != string::npos && pos2 == string::npos && pos3 == string::npos) {
    int reg_num = atoi(op1str.substr(pos1+1, op1str.length()-1).c_str());
    //cout<<"op2: "<<reg_num<<endl;
    return make_pair(VAR, reg_num);
  }

  return make_pair(NONE, -1);
}

inline bool newfunc_reached() {
  string s;
  int found1, found2, found3;

  //peek line
  streampos sp = cin.tellg();
  getline( cin, s );
  cin.seekg( sp );

  found1 = s.find("enter");
  found2 = s.find("entrypc");
  found3 = s.find("nop");

  if (found1 == string::npos && found2 == string::npos && found3 == string::npos)
    return 0;
  else
    return 1;
}

// return 0 if reach the end of basic block
// return 1 if there are instructions following
bool Instr::populate(string temp) {
  int found;

  // get instruction
  instr = temp;

  // get intruction number
  //cout<<instr<<endl;
  num = instr_num(instr);
  pair<OpType, int> t;

  // populate def and use
  // type 1, 1 def + 2 use
  for(int i=0; i<DEF_REG1_SIZE; i++) {
    found = instr.find(def_reg1[i]);
    if (found != std::string::npos) {
      def.insert(make_pair(REG, num));
      use.insert(get_1op(instr));
      use.insert(get_2op(instr));
      if (br_target.find(num-1) != br_target.end())
        return 0;
      return 1;
    }
  }
  // type 2, 1 def + 1 use
  for(int i=0; i<DEF_REG2_SIZE; i++) {
    found = instr.find(def_reg2[i]);
    if (found != std::string::npos) {
      def.insert(make_pair(REG, num));
      use.insert(get_1op(instr));
      if (br_target.find(num-1) != br_target.end())
        return 0;
      return 1;
    }
  }
  // type 3, 0 def + 2 use
  for(int i=0; i<DEF_REG3_SIZE; i++) {
    found = instr.find(def_reg3[i]);
    if (found != std::string::npos) {
      use.insert(get_1op(instr));
      use.insert(get_2op(instr));
      if (br_target.find(num-1) != br_target.end())
        return 0;
      return 1;
    }
  }
  // type 5, 0 def + 1 use
  for(int i=0; i<DEF_REG5_SIZE; i++) {
    found = instr.find(def_reg5[i]);
    if (found != std::string::npos) {
      use.insert(get_1op(instr));
      def.insert(get_2op(instr));
      if (br_target.find(num-1) != br_target.end())
        return 0;
      return 1;
    }
  }
  // type 4, 0 def + 1 use
  for(int i=0; i<DEF_REG4_SIZE; i++) {
    found = instr.find(def_reg4[i]);
    if (found != std::string::npos) {
      use.insert(get_1op(instr));
    }
  }
  // check end of bb
  for(int i=0; i<BB_END_SIZE; i++) {
    found = instr.find(bb_end[i]);
    if (found != std::string::npos)
      return 0;
  }
  return 1;
}

// return 0 if reach the end of the function
// return 1 if there are other bb following
bool BasicBlock::populate() {
  string temp;
  bool ret=1;

  // get a basic block
  while(getline(cin, temp)) {
    instr.push_back(new Instr);
    ret = instr.back()->populate(temp);
    if (!ret)
      break;
  }

  // update basic block number
  num = instr.front()->num;

  // check branch target and populate children
  string last_instr = instr.back()->instr;
  int last_instr_num = instr.back()->num;
  int found, found1;
  // call
  found = last_instr.find("call");
  if (found != string::npos) {
    branch_target = -1;
    children.insert(last_instr_num+1);
  }
  // br
  found = last_instr.find("br");
  if (found != string::npos) {
    branch_target = get_1op(last_instr).second;
    children.insert(branch_target);
  }
  // blbc and blbs
  found = last_instr.find("blbc");
  found1 = last_instr.find("blbs");
  if (found != string::npos || found1 != string::npos) {
    branch_target = get_2op(last_instr).second;
    children.insert(branch_target);
    children.insert(last_instr_num+1);
  }

  // check if reach the end of a function
  if (cin.eof()) {
    cout<<"end of file@@@"<<endl;
    return 0;
  }
  else if (newfunc_reached()) {
    cout<<"end of func@@@"<<endl;
    return 0;
  }
  else {
    cout<<"continue @@@@"<<endl;
    return 1;
  }

}

BasicBlock* Function::get_bb(int num) {
  for(int i=0; i<bb.size(); i++) {
    if (bb[i]->num == num)
      return bb[i];
  }
  assert(0 && "BB not found");
  return NULL;
}

void Function::populate() {
  bool ret;

  // store cin pos
  streampos sp = cin.tellg();

  // get all branch targets
  string temp;
  int pos1, pos2;

  while(getline(cin, temp)) {
    cout<<temp<<endl;

    pos1 = temp.find("br");
    if (pos1 != string::npos) {
      br_target.insert(get_1op(temp).second);
      continue;
    }

    pos1 = temp.find("blbc");
    pos2 = temp.find("blbs");
    if (pos1 != string::npos || pos2 != string::npos) {
      br_target.insert(get_2op(temp).second);
      continue;
    }
  }

  // rewind cin
  cin.seekg( sp );


  do {
    bb.push_back(new BasicBlock);
    ret = bb.back()->populate();
  } while(ret);

  return;
}

void Function::print_CFG() {
  cout<<"Function: "<<bb[0]->num<<endl;

  cout<<"Basic blocks: ";
  for(int i=0; i<bb.size(); i++)
    cout<<bb[i]->num<<" ";

  cout<<endl<<"CFG: "<<endl;
  for(int i=0; i<bb.size(); i++) {
    cout<<bb[i]->num<<" -> ";
    for(set<int>::iterator j=bb[i]->children.begin();
        j != bb[i]->children.end(); j++)
      cout<<*j<<" ";
    cout<<endl;
  }
  return;
}

void Function::compute_live() {
  return;
}

void Function::dec() {
  return;
}

void Function::print_instr() {
  return;
}
