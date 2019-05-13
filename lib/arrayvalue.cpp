#ifndef _ARRAY_VALUE_CPP
#define _ARRAY_VALUE_CPP

#include "arrayvalue.h"

ArrayValue::ArrayValue(const string &sName, Value *pContainer) : Value(pContainer, OBJECT_TYPE::ARRAY, sName, 2)
{
}

ArrayValue::~ArrayValue()
{
  list<Value*>::iterator it;
  for (it = m_lstData.begin(); it != m_lstData.end(); ++it) {
    delete (*it);
  }
}

void ArrayValue::add(Value *pValue)
{
  m_lstData.push_back(pValue);
}

string ArrayValue::toBizFileString() const
{
  if (0 == m_lstData.size()) return "";

  list<Value*>::const_iterator it;
  string s;
  if (OBJECT_TYPE::SINGLE == (*(m_lstData.begin()))->getType()) {
    for(it = m_lstData.begin(); it != m_lstData.end(); ++it) {
      s.append(m_sName).append("\n");
      s.append((*it)->toBizFileString());
    }
  } else {
    s.append(m_sName).append("\n");
    for( it = m_lstData.begin(); it != m_lstData.end(); ++it) {
      s.append((*it)->toBizFileString());
    }
  }

  return s;
}

int ArrayValue::getItemCount() const
{
  return m_lstData.size();
}

void ArrayValue::print(int nLevel) const
{
  for (int i = 0 ; i < nLevel; i += 1) {
    cout << "\t";
  }
  cout << m_sName << endl;

  list<Value*>::const_iterator it;
  for (it = m_lstData.begin() ; it != m_lstData.end(); ++it) {
    (*it)->print(nLevel + 1);
  }
}

template <typename cbFunc>
void ArrayValue::forEach(cbFunc func)
{
  list<Value*>::iterator it;
  int nIndex = 0;
  for (it = m_lstData.begin(); it != m_lstData.end(); ++it) {
    func(*it, nIndex);
  }
}
#endif