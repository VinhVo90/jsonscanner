#pragma once

#include <iostream>
#include "value.h"
#include "const.h"
using namespace std;

class SingleValue : public Value {
  string m_sData;
public:
  SingleValue(const string &name = "", const string &value = "", Value *container = NULL);
  virtual ~SingleValue();
  void setValue(const string &value);
  const string& getValue() const;
  virtual string toBizFileString(int level = 0) const;
  virtual void print(int level = 0) const;
};

#include "singlevalue.cpp"