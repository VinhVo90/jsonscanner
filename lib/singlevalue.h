#pragma once

#include <iostream>
#include "value.h"
#include "const.h"
using namespace std;

class SingleValue : public Value {
  string m_sData;
public:
  SingleValue(const string &sName = "", const string &sValue = "", Value *pContainer = NULL);
  virtual ~SingleValue();
  void setValue(const string &sValue);
  const string& getValue() const;
  virtual string toBizFileString() const;
  virtual void print(int nLevel = 0) const;
};

#include "singlevalue.cpp"