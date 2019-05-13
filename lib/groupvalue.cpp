#ifndef _GROUP_VALUE_CPP
#define _GROUP_VALUE_CPP

#include "groupvalue.h"

GroupValue::GroupValue(const string &sName, Value *pContainer) : Value(pContainer, OBJECT_TYPE::GROUP, sName, 1)
{
}

GroupValue::~GroupValue()
{
  list<Value*>::iterator it;
  for (it = m_lstData.begin(); it != m_lstData.end(); ++it) {
    delete (*it);
  }
}

void GroupValue::add(Value *pValue)
{
  // Priority: SINGLE > GROUP > ARRAY
  if ( 0 == m_lstData.size()) {
    m_lstData.push_back(pValue);
    return;
  }

  list<Value*>::iterator it = m_lstData.end();
  it--;
  for (; it != m_lstData.begin(); --it) {
    if (pValue->getPriority() <= (*it)->getPriority()) {
      break;
    }
  }

  it++;
  m_lstData.insert(it, pValue);
}

string GroupValue::toBizFileString() const
{
  string s;
  s.append(m_sName).append("\n");

  list<Value*>::const_iterator it = m_lstData.begin();
  for (; it != m_lstData.end(); ++it) {
    s.append((*it)->toBizFileString());
  }
  
  return s;
}

Value* GroupValue::getItemByName(string sItemName) const
{
  list<Value*>::const_iterator it = m_lstData.begin();
  for (; it != m_lstData.end(); ++it) {
    if (sItemName == (*it)->getName()) return *it;
  }

  return NULL;
}

int GroupValue::getItemCount() const
{
  return m_lstData.size();
}

void GroupValue::print(int nLevel) const
{
  for (int i = 0 ; i < nLevel; i += 1) {
    cout << "\t";
  }
  cout << m_sName << endl;

  list<Value*>::const_iterator it = m_lstData.begin();
  for (; it != m_lstData.end(); ++it) {
    (*it)->print(nLevel + 1);
  }
}

template <typename cbFunc>
void GroupValue::forEach(cbFunc func)
{
  list<Value*>::iterator it;
  int nIndex = 0;
  for (it = m_lstData.begin(); it != m_lstData.end(); ++it) {
    func(*it, nIndex);
  }
}

#endif