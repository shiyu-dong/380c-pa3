#include <assert.h>
#include <iostream>
#include <fstream>
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
  
  // union of all children's live_list
  for(set<int>::iterator i=parent->children.begin();
      i!=parent->children.end(); i++) {
    BasicBlock* child = get_bb(*i);
    for(set<int>::iterator j=child->live_list.begin();
        j!=child->live_list.end(); j++) {
      live_t.insert(*j);
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
    return false;
  }
  
  parent->live_list.clear();
  parent->live_list = live_t;
  return true;
}

// retrun true if any dce happens
bool BasicBlock::dce() {
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

void Function::dce() {
  bool has_change = 1;

  // keep doing dce until no change is made
  while(has_change) {
    has_change = 0;
    // compute use and def for each bb
    for(int i=0; i<bb.size(); i++) {
      bb[i]->compute_defuse();

      // print use, def
      //cout<<"bb: "<<bb[i]->num<<"\n";
      //cout<<"use size: "<<bb[i]->use.size()<<"\n\t";
      //for(set<int>::iterator j=bb[i]->use.begin(); j!=bb[i]->use.end(); j++)
      //  cout<<*j<<" ";
      //cout<<endl;
      //cout<<"def size: "<<bb[i]->def.size()<<"\n\t";
      //for(set<int>::iterator j=bb[i]->def.begin(); j!=bb[i]->def.end(); j++)
      //  cout<<*j<<" ";
      //cout<<endl<<endl;;
    }

    // compute live var for each bb
    compute_live();

    for(int i=0; i<bb.size(); i++) {
      cout<<"BB num: "<<bb[i]->num<<"\n\t";
      for(set<int>::iterator j=bb[i]->live_list.begin();
          j != bb[i]->live_list.end(); j++) {
          cout<<*j<<" ";
      }
      cout<<endl;
    }


    // dce on each bb
    //for(int i=0; i<bb.size(); i++) {
    //  has_change |= bb[i]->dce();
    //}
  }

  return;
}

