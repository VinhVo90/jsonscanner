#ifndef _GROUP_VALUE_CPP
#define _GROUP_VALUE_CPP

#include "groupvalue.h"

GroupValue::GroupValue(const string &name, Value *container) : Value(container, OBJECT_TYPE::GROUP, name, 1)
{
}

GroupValue::~GroupValue()
{
  list<Value*>::iterator it;
  for (it = m_lstData.begin(); it != m_lstData.end(); ++it) {
    delete (*it);
  }
}

void GroupValue::add(Value *value)
{
  // Priority: SINGLE > GROUP > ARRAY
  if ( 0 == m_lstData.size()) {
    m_lstData.push_back(value);
    return;
  }

  list<Value*>::iterator it = m_lstData.end();
  it--;
  for (; it != m_lstData.begin(); --it) {
    if (value->getPriority() <= (*it)->getPriority()) {
      break;
    }
  }

  it++;
  m_lstData.insert(it, value);
}

string GroupValue::toBizFileString(int level) const
{
  if (0 == m_lstData.size()) return "";

  string tabLevel;
  for (int i = 0; i < level; i += 1) {
    tabLevel.append("\t");
  }

  string s;
  s.append(tabLevel).append(m_sName).append("\n");
  list<Value*>::const_iterator it = m_lstData.begin();
  for (; it != m_lstData.end(); ++it) {
    s.append((*it)->toBizFileString(level + 1));
  }
  
  return s;
}

Value* GroupValue::getItemByName(string itemName) const
{
  list<Value*>::const_iterator it = m_lstData.begin();
  for (; it != m_lstData.end(); ++it) {
    if (itemName == (*it)->getName()) return *it;
  }

  return NULL;
}

int GroupValue::getItemCount() const
{
  return m_lstData.size();
}

void GroupValue::print(int level) const
{
  for (int i = 0 ; i < level; i += 1) {
    cout << "\t";
  }
  cout << m_sName << endl;

  list<Value*>::const_iterator it = m_lstData.begin();
  for (; it != m_lstData.end(); ++it) {
    (*it)->print(level + 1);
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