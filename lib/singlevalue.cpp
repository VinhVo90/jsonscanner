#ifndef _SINGLE_VALUE_CPP
#define _SINGLE_VALUE_CPP

#include "singlevalue.h"

SingleValue::SingleValue(const string &sName, const string &sValue, Value *pContainer) : Value(pContainer, OBJECT_TYPE::SINGLE, sName, 3) {
  m_sData = sValue;
}

SingleValue::~SingleValue() {

}

void SingleValue::setValue(const string &sValue) {
  m_sData = sValue;
}

const string& SingleValue::getValue() const {
  return m_sData;
}

string SingleValue::toBizFileString() const {
  string s;
  if ("" != m_sName) {
    s.append(m_sName).append(":");
  }

  s.append(m_sData).append("\n");

  return s;
}

void SingleValue::print(int nLevel) const {
  for (int i = 0; i < nLevel; i += 1) {
    cout << "\t";
  }

  cout << m_sName << ":" << m_sData << endl;
}
#endif