#ifndef _SINGLE_VALUE_CPP
#define _SINGLE_VALUE_CPP

#include "singlevalue.h"

SingleValue::SingleValue(const string &name, const string &value, Value *container) : Value(container, OBJECT_TYPE::SINGLE, name, 3)
{
  m_sData = value;
}

SingleValue::~SingleValue()
{
}

void SingleValue::setValue(const string &value)
{
  m_sData = value;
}

const string& SingleValue::getValue() const
{
  return m_sData;
}

string SingleValue::toBizFileString(int level) const
{
  string tabLevel;
  for (int i = 0; i < level; i += 1) {
    tabLevel.append("\t");
  }

  string s;
  s.append(tabLevel);
  
  if ("" != m_sName) {
    s.append(m_sName).append(":");
  }

  s.append(m_sData).append("\n");

  return s;
}

void SingleValue::print(int level) const
{
  for (int i = 0; i < level; i += 1) {
    cout << "\t";
  }

  cout << m_sName << ":" << m_sData << endl;
}

#endif