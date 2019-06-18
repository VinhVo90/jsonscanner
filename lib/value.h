#pragma once
#include <iostream>
#include "const.h"
using namespace std;

class Value {
protected:
  int m_nType;
  string m_sName;
  Value *m_pContainer;
  int m_nPriority;
public:
  Value(Value *container = NULL, int type = -1, const string &name = "", int priority = 1);
  virtual ~Value();
  int getType() const;
  const string& getName() const;
  Value* getContainer() const;
  void setContainer(Value *container);
  int getPriority() const;
  virtual string toBizFileString(int level = 0) const = 0;
  virtual void print(int level = 0) const = 0;
};

#include "value.cpp"