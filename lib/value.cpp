#ifndef _VALUE_CPP
#define _VALUE_CPP

#include "value.h"

Value::Value(Value *container, int type, const string &name, int priority)
{
  m_nType = type;
  m_sName = name;
  m_pContainer = container;
  m_nPriority = priority;
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

Value* Value::getContainer() const
{
  return m_pContainer;
}

void Value::setContainer(Value *container)
{
  m_pContainer = container;
}

int Value::getPriority() const
{
  return m_nPriority;
}

#endif