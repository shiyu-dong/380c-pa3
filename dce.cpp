#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "dce.h"

using namespace std;

inline void BasicBlock::add_instr_def(list<Instr*>::iterator it) {
  set<int>::iterator j;

  for(set<pair<OpType, int> >::iterator i=(*it)->def.begin(); i != (*it)->def.end(); i++) {
    if (i->first == VAR) {
      def.insert(i->second);
      j = use.find(i->second);
      if (j != use.end()) use.erase(j);
    }
  }

  return;
}

inline void BasicBlock::add_instr_use(list<Instr*>::iterator it) {

  for(set<pair<OpType, int> >::iterator i=(*it)->use.begin(); i != (*it)->use.end(); i++) {
    if (i->first == VAR)
      use.insert(i->second);
  }

  return;
}

void BasicBlock::compute_defuse() {
  use.clear();
  def.clear();

  if (instr.size() == 0)
    return;

  list<Instr*>::iterator i=instr.end();
  do {
    i--;
    add_instr_def(i);
    add_instr_use(i);
  } while(i != instr.begin());

}

// return true if there is change in liveness
// return false when there is no change
bool Function::compute_bb_live(int bb_num) {
  // parent->live_list = child->live_list - parent->define + parent->use
  BasicBlock* parent = get_bb(bb_num);

  // construct a temp set;
  // compare it with live_list to detect changes
  set<int> live_t;

  parent->children_live.clear();
  
  // union of all children's live_list
  for(set<BasicBlock*>::iterator i=parent->children_p.begin();
      i!=parent->children_p.end(); i++) {
    BasicBlock* child = *i;
    for(set<int>::iterator j=child->live_list.begin();
        j!=child->live_list.end(); j++) {
      live_t.insert(*j);
      parent->children_live.insert(*j);
    }
  }

  // minus parent->define
  for(set<int>::iterator i=parent->def.begin();
      i!=parent->def.end(); i++) {
    set<int>::iterator j = live_t.find(*i);
    if (j != live_t.end()) {
      live_t.erase(j);
    }
  }

  // add parent->use
  for(set<int>::iterator i=parent->use.begin();
      i!=parent->use.end(); i++) {
    live_t.insert(*i);
  }

  // check if there is any change
  set<int>::iterator i = live_t.begin();
  set<int>::iterator j = parent->live_list.begin();
  while(i!=live_t.end() && j!=parent->live_list.end()) {
    if (*i != *j) {
      parent->live_list.clear();
      parent->live_list = live_t;
      return true;
    }
    i++;
    j++;
  }
  
  if (i==live_t.end() && j==parent->live_list.end()) {
#ifdef DEBUG_1
    cout<<"\\bb num: "<<bb_num<<" live size: "<<parent->live_list.size()<<endl;
#endif
    return false;
  }
  
  parent->live_list.clear();
  parent->live_list = live_t;
  return true;
}

void Function::compute_live() {
  bool has_change = 1;

  // iterate through all bbs until liveness for each bb stablizes
  while(has_change) {
    has_change = 0;
    for(int i=0; i<bb.size(); i++) {
      has_change |= compute_bb_live(bb[i]->num);
    }
  }

  return;
}

// retrun true if any dce happens
bool BasicBlock::dce(Function* f, map<int, int>& ref_map, set<int>& dead_var_offset) {
  // live hold live C variables 
  set<int> live;
  // dead hold dead virtual registers
  set<int> dead;
  bool eliminated = false;

  // working bottom up, init live
  live.clear();
  live = children_live;
  dead.clear();

  // an instr should be eliminated if
  // 1) it defines a C variable that is not live
  // 2) it defines virtual registers that are dead
  // update dead virtual registers in both cases

  list<Instr*>::iterator it = instr.end();
  while(it != instr.begin()) {
    it --;

    // check if it uses any C variables not in live_list
    if ((*it)->use.size() != 0) {
      set<pair<OpType, int> >::iterator jt;
      for(jt = (*it)->use.begin(); jt != (*it)->use.end(); jt++) {
        if (jt->first == VAR) {
          live.insert(jt->second);
        }
      }
    }

    if ((*it)->def.size() != 0) {
      pair<OpType, int> this_def= *(*it)->def.begin();
    // check if it defines dead C variable
    // check if it defines dead virtual reg
      if ( (this_def.first == VAR && live.find(this_def.second) == live.end()) ||
           (this_def.first == REG && dead.find(this_def.second) != dead.end()) ){

        // copy all it's virtual register into dead
        for(set<pair<OpType, int> >::iterator jt = (*it)->use.begin();
            jt != (*it)->use.end(); jt++) {
          if (jt->first == REG) {
            dead.insert(jt->second);
          }
        }

        // decrement reference count in ref_map;
        if (this_def.first == VAR && live.find(this_def.second) == live.end()
            && this_def.second < 0) {
#ifdef DEBUG_0
          assert(ref_map.find(this_def.second) != ref_map.end());
          assert(ref_map[this_def.second] > 0);
#endif
          ref_map[this_def.second] = ref_map[this_def.second] - 1;
          if (ref_map[this_def.second] == 0) {
            dead_var_offset.insert(this_def.second);
#ifdef DEBUG_1
            cout<<"//erasing variable: "<<this_def.second<<endl;
#endif
            ref_map.erase(this_def.second);
          }
        }

        // delete this instruction
#ifdef DEBUG_1
        cout<<"//erasing instruction: "<<(*it)->instr<<endl;
#endif
        it = instr.erase(it);
        eliminated |= 1;
      }
    }

  }

  return eliminated;
}

int Function::next_instr_num(int start) {
  if (start == -1)
    assert(0 && "illegal instruction number");
  for(int i=0; i<bb.size(); i++) {
    if (bb[i]->num >= start) {
      return bb[i]->num;
    }
  }
  assert(0 && "illegal instruction number");
  return -1;
}

void Function::reconnect() {

  // for each bb check make sure bb num is correct
  for(int i=0; i<bb.size(); i++) {
    // there must be instr in this bb
    assert(bb[i]->instr.size() > 0);

    bb[i]->num = (*bb[i]->instr.begin())->num;
  }

  // for each bb check check branch destination exists
  for(int i=0; i<bb.size(); i++) {
    // branch target is correct, last branch instruction is correct
    if (bb[i]->branch_target != -1) {
      bool need_to_reconnect = 1;

      for(set<BasicBlock*>::iterator it=bb[i]->children_p.begin();
          it != bb[i]->children_p.end(); it++) {
        if (bb[i]->branch_target == (*it)->num) {
          need_to_reconnect = 0;
        }
      }

      if (need_to_reconnect) {
        bb[i]->branch_target = next_instr_num(bb[i]->branch_target);

        // change branch instruction string
        string old_instr = bb[i]->instr.back()->instr;
        string new_instr;
        stringstream t;

        t<<bb[i]->branch_target;

        size_t pos = old_instr.find('[');
        assert(pos != string::npos);

        new_instr += old_instr.substr(0, pos+1);
        new_instr += t.str();
        new_instr += "]";
        bb[i]->instr.back()->instr = new_instr;
      }
    }
  }

  return;
}

void Function::rename_operand(string & instr) {
  int last_pos, pos1, pos = -1;
  int count;

  while(1) {
    last_pos = pos+1;
    pos = instr.find('#', last_pos);
    if (pos == string::npos)
      break;
    else if (instr.substr(last_pos, pos).find("_base") != string::npos)
      continue;
      pos1 = instr.find(' ', pos);
    int offset = atoi(instr.substr(pos+1, pos1).c_str());

    // rename the offset
    if (offset < 0) {
      count = 0;
      for(set<int>::iterator it = dead_var_offset.begin();
          it != dead_var_offset.end(); it++) {
        if (offset < *it)
          count++;
        else
          break;
      }
      offset = offset + count*8;
    }

      /*
      it = var_map.find(offset);
      if (it == var_map.end()) {
        var_map[offset] = count*(-8);
        count = count+1;
      }
      offset = var_map[offset];*/

    // put back
    string new_str;
    stringstream t;
    int pos2 = instr.find(' ', pos);

    t<<offset;

    new_str = instr.substr(0, pos+1);
    new_str += t.str();
    if (pos2 != string::npos)
      new_str += instr.substr(pos2);

    instr = new_str;
  }
}

void Function::rename() {
  // rename C registers of each instruction
  for(int i=0; i<bb.size(); i++) {
    for(list<Instr*>::iterator it = bb[i]->instr.begin();
        it != bb[i]->instr.end(); it++) {
        rename_operand((*it)->instr);
    }
  }
  local_size = local_size - dead_var_offset.size();
}

void Function::dce() {
  bool has_change = 1;

  // keep doing dce until no change is made
  while(has_change) {
    has_change = 0;
    // compute use and def for each bb
    for(int i=0; i<bb.size(); i++) {
      bb[i]->compute_defuse();
    }

    // compute live var for each bb
    compute_live();

#ifdef DEBUG_2
    for(int i=0; i<bb.size(); i++) {
      cout<<"BB num: "<<bb[i]->num<<"\n\t";
      for(set<int>::iterator j=bb[i]->live_list.begin();
          j != bb[i]->live_list.end(); j++) {
          cout<<*j<<" ";
      }
      cout<<endl;
    }

    cout<<endl;
    print_instr();
    cout<<endl;
#endif


    // dce on each bb
    for(vector<BasicBlock*>::iterator i=bb.begin();
        i != bb.end(); i++) {
      has_change |= (*i)->dce(this, ref_map, dead_var_offset);

      // delete a bb if it is empty
      if ((*i)->instr.size() == 0) {
        // reconnect its parents to its children
        for(set<BasicBlock*>::iterator it = (*i)->parent_p.begin();
            it != (*i)->parent_p.end(); it++) {
          // remove myself from parent's children list
          (*it)->children_p.erase(*i);
          // add my children to parent's children list
          for(set<BasicBlock*>::iterator jt = (*i)->children_p.begin();
              jt != (*i)->children_p.end(); jt++) {
            (*it)->children_p.insert((*jt));
          }
        }
#ifdef DEBUG_1
        cout<<"//erasing bb: "<<(*i)->num<<endl;
#endif
        i = bb.erase(i);
      }
    }
  }
  // reconnect the CFG
  reconnect();

  // change C variable indexing
  rename();

  return;
}

