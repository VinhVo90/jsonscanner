#ifndef _VALUE_CPP
#define _VALUE_CPP

#include "value.h"

Value::Value(Value *pContainer, int nType, const string &sName, int nPriority)
{
  m_nType = nType;
  m_sName = sName;
  m_pContainer = pContainer;
  m_nPriority = nPriority;
}

Value::~Value()
{
}

int Value::getType() const
{
  return m_nType;
}

const string& Value::getName() const
{
  return m_sName;
}

void Value::setName(const string &sName)
{
  m_sName = sName;
}

Value* Value::getContainer() const
{
  return m_pContainer;
}

void Value::setContainer(Value *pContainer)
{
  m_pContainer = pContainer;
}

int Value::getPriority() const
{
  return m_nPriority;
}

#endif