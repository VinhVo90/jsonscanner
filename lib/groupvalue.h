#pragma once

#include "value.h"
#include "const.h"
#include <list>

class GroupValue : public Value {
  list<Value*> m_lstData;
public:
  GroupValue(const string &sName = "", Value *pContainer = NULL);
  virtual ~GroupValue();
  void add(Value *pValue);
  int getItemCount() const;
  Value* getItemByName(string sItemName) const;
  Value* getItemByIndex(int nIndex) const;
  virtual string toBizFileString() const;
  virtual void print(int nLevel = 0) const;

  template <typename cbFunc>  
  void forEach(cbFunc func);
};

#include "groupvalue.cpp"