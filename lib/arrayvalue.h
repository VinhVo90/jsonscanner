#pragma once

#include <iostream>
#include "Array.h"
#include "value.h"
#include "const.h"

using namespace std;

class ArrayValue : public Value {
  Array<Value*> m_arrData;
public:
  ArrayValue(const string &sName = "", Value *pContainer = NULL);
  virtual ~ArrayValue();
  void add(Value* pValue);
  int getItemCount() const;
  Value* getItemByIndex(int nIndex) const;
  virtual string toBizFileString() const;
  virtual void print(int nLevel = 0) const;
};

#include "arrayvalue.cpp"