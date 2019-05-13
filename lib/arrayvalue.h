#pragma once

#include <iostream>
#include "value.h"
#include "const.h"
#include <list>

using namespace std;


class ArrayValue : public Value {
  list<Value*> m_lstData;
public:
  ArrayValue(const string &sName = "", Value *pContainer = NULL);
  virtual ~ArrayValue();
  void add(Value* pValue);
  int getItemCount() const;
  Value* getItemByIndex(int nIndex) const;

  template <typename cbFunc>  
  void forEach(cbFunc func);

  virtual string toBizFileString() const;
  virtual void print(int nLevel = 0) const;
};

#include "arrayvalue.cpp"