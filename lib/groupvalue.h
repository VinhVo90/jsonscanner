#pragma once

#include "Array.h"
#include "value.h"
#include "const.h"

class GroupValue : public Value {
  Array<Value*> m_arrData;
public:
  GroupValue(const string &sName = "", Value *pContainer = NULL);
  virtual ~GroupValue();
  void add(Value *pValue);
  int getItemCount() const;
  Value* getItemByName(string sItemName) const;
  Value* getItemByIndex(int nIndex) const;
  virtual string toBizFileString() const;
  virtual void print(int nLevel = 0) const;
};

#include "groupvalue.cpp"