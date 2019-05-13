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
  Value(Value *pContainer = NULL, int nType = -1, const string &sName = "", int nPriority = 1);
  virtual ~Value();
  int getType() const;
  const string& getName() const;
  void setName(const string &sType);
  Value* getContainer() const;
  void setContainer(Value *pContainer);
  int getPriority() const;
  virtual string toBizFileString() const = 0;
  virtual void print(int nLevel = 0) const = 0;
};

#include "value.cpp"