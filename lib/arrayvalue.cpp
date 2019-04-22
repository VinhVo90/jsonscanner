#ifndef _ARRAY_VALUE_CPP
#define _ARRAY_VALUE_CPP

#include "arrayvalue.h"

ArrayValue::ArrayValue(const string &sName, Value *pContainer) : Value(pContainer, OBJECT_TYPE::ARRAY, sName, 1)
{
}

ArrayValue::~ArrayValue()
{
  int nCount = m_arrData.getCount();
  for (int i = 0; i < nCount; i += 1) {
    delete m_arrData[i];
  }
}

void ArrayValue::add(Value *pValue)
{
  m_arrData.insertLast(pValue);
}

string ArrayValue::toBizFileString() const
{
  int nCount = m_arrData.getCount();
  if (0 == nCount) return "";

  string s;
  if (OBJECT_TYPE::SINGLE == m_arrData[0]->getType()) {
    for( int i = 0; i < nCount; i += 1) {
      s.append(m_sName).append("\n");
      s.append(m_arrData[i]->toBizFileString());
    }
  } else {
    s.append(m_sName).append("\n");
    for( int i = 0; i < nCount; i += 1) {
      s.append(m_arrData[i]->toBizFileString());
    }
  }

  return s;
}

int ArrayValue::getItemCount() const
{
  return m_arrData.getCount();
}

Value* ArrayValue::getItemByIndex(int nIndex) const
{
  if (0 == m_arrData.getCount()) return NULL;
  if (0 > nIndex) return NULL;
  if (nIndex > m_arrData.getCount() - 1) return NULL;

  return m_arrData[nIndex];
}

void ArrayValue::print(int nLevel) const
{
  for (int i = 0 ; i < nLevel; i += 1) {
    cout << "\t";
  }
  cout << m_sName << endl;

  int nCount = m_arrData.getCount();
  for (int i = 0 ; i < nCount; i += 1) {
    m_arrData[i]->print(nLevel + 1);
  }
}
#endif