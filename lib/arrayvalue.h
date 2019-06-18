#pragma once

#include <iostream>
#include "value.h"
#include "const.h"
#include <list>
using namespace std;

class ArrayValue : public Value {
  list<Value*> m_lstData;
public:
  ArrayValue(const string &name = "", Value *container = NULL);
  virtual ~ArrayValue();
  void add(Value* value);
  int getItemCount() const;
  Value* getItemByIndex(int index) const;

  template <typename cbFunc>  
  void forEach(cbFunc func);

  virtual string toBizFileString(int level = 0) const;
  virtual void print(int level = 0) const;
};

#include "arrayvalue.cpp"