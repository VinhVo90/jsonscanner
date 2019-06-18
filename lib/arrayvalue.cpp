#ifndef _ARRAY_VALUE_CPP
#define _ARRAY_VALUE_CPP

#include "arrayvalue.h"

ArrayValue::ArrayValue(const string &name, Value *container) : Value(container, OBJECT_TYPE::ARRAY, name, 2)
{
}

ArrayValue::~ArrayValue()
{
  list<Value*>::iterator it;
  for (it = m_lstData.begin(); it != m_lstData.end(); ++it) {
    delete (*it);
  }
}

void ArrayValue::add(Value *value)
{
  m_lstData.push_back(value);
}

string ArrayValue::toBizFileString(int level) const
{  
  if (0 == m_lstData.size()) return "";

  string tabLevel;
  for (int i = 0; i < level; i += 1) {
    tabLevel.append("\t");
  }

  list<Value*>::const_iterator it;
  string s;
  if (OBJECT_TYPE::SINGLE == (*(m_lstData.begin()))->getType()) {
    for(it = m_lstData.begin(); it != m_lstData.end(); ++it) {
      s.append(tabLevel).append(m_sName).append("\n");
      s.append((*it)->toBizFileString(level + 1));
    }
  } else {
    s.append(tabLevel).append(m_sName).append("\n");
    for( it = m_lstData.begin(); it != m_lstData.end(); ++it) {
      s.append((*it)->toBizFileString(level + 1));
    }
  }

  return s;
}

int ArrayValue::getItemCount() const
{
  return m_lstData.size();
}

void ArrayValue::print(int level) const
{
  for (int i = 0 ; i < level; i += 1) {
    cout << "\t";
  }
  cout << m_sName << endl;

  list<Value*>::const_iterator it;
  for (it = m_lstData.begin() ; it != m_lstData.end(); ++it) {
    (*it)->print(level + 1);
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