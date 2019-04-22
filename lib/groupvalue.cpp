#ifndef _GROUP_VALUE_CPP
#define _GROUP_VALUE_CPP

#include "groupvalue.h"

GroupValue::GroupValue(const string &sName, Value *pContainer) : Value(pContainer, OBJECT_TYPE::GROUP, sName, 2)
{
}

GroupValue::~GroupValue()
{
  int nCount = m_arrData.getCount();
  for (int i = 0; i < nCount; i += 1) {
    delete m_arrData[i];
  }
}

void GroupValue::add(Value *pValue)
{
  // Priority: SINGLE > GROUP > ARRAY
  int nCount = m_arrData.getCount();
  if ( 0 == nCount) {
    m_arrData.insert(pValue);
    return;
  }

  int i = 0;
  for (; i < nCount; i += 1) {
    if (pValue->getPriority() > m_arrData[i]->getPriority()) break;
  }
  
  if (i < nCount) {
    m_arrData.insert(pValue, i);
    return;
  }

  m_arrData.insertLast(pValue);
}

string GroupValue::toBizFileString() const
{
  string s;
  int nCount = m_arrData.getCount();
  s.append(m_sName).append("\n");

  for( int i = 0; i < nCount; i += 1) {
    s.append(m_arrData[i]->toBizFileString());
  }

  return s;
}

Value* GroupValue::getItemByName(string sItemName) const
{
  int nCount = m_arrData.getCount();

  for( int i = 0; i < nCount; i += 1) {
    if (m_arrData[i]->getName() == sItemName) return m_arrData[i];
  }

  return NULL;
}

int GroupValue::getItemCount() const
{
  return m_arrData.getCount();
}

Value* GroupValue::getItemByIndex(int nIndex) const
{
  if (0 == m_arrData.getCount()) return NULL;
  if (0 > nIndex) return NULL;
  if (nIndex > m_arrData.getCount() - 1) return NULL;

  return m_arrData[nIndex];
}

void GroupValue::print(int nLevel) const
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