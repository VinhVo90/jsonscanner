#pragma once

#include "value.h"
#include "const.h"
#include <list>

class GroupValue : public Value {
  list<Value*> m_lstData;
public:
  GroupValue(const string &name = "", Value *container = NULL);
  virtual ~GroupValue();
  void add(Value *value);
  int getItemCount() const;
  Value* getItemByName(string itemName) const;
  Value* getItemByIndex(int index) const;
  virtual string toBizFileString(int level = 0) const;
  virtual void print(int level = 0) const;

  template <typename cbFunc>  
  void forEach(cbFunc func);
};

#include "groupvalue.cpp"