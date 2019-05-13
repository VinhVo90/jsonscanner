#ifndef _ARRAY_CPP
#define _ARRAY_CPP

#include "array.h"

template <class T>
const int Array<T>::DEF_SIZE = 1024;

template <class T>
Array<T>::Array(int size)
{
	m_curSize = size > Array::DEF_SIZE ? size : Array::DEF_SIZE;
	m_pData = new T[m_curSize];
	m_num = 0;
}

template <class T>
Array<T>::Array(const Array & a)
{
	m_pData = NULL; m_curSize = m_num = 0;

	copy(a);
}

template <class T>
Array<T>::~Array()
{
	delete[] m_pData;
}

template <class T>
Array<T>& Array<T>::operator = (const Array & a)
{
	if(this != &a)
	{
		copy(a);
	}

	return *this;
}

template <class T>
T& Array<T>::operator [] (int index)
{
	return m_pData[index];
}

template <class T>
const T& Array<T>::operator [] (int index) const
{
	return m_pData[index];
}

template <class T>
bool Array<T>::isFull() const
{
	return m_num == m_curSize;
}

template <class T>
bool Array<T>::isEmpty() const
{
	return 0 == m_num;
}

template <class T>
int Array<T>::getCount() const
{
	return m_num;
}

template <class T>
bool Array<T>::insert(const T & x, int index)
{
	bool succ = false;

	if (!isFull() || growBy()) {
		if (0 > index || index > m_num) index = m_num;

		for(int i = m_num; i > index; i -= 1) {
			m_pData[i] = m_pData[i - 1];
		}

		m_pData[index] = x;
		m_num += 1;
		
		succ = true;
	}

	return succ;
}

template<class T>
bool Array<T>::insertLast(const T& x)
{
	bool succ = false;

	if (!isFull() || growBy()) {
		m_pData[m_num] = x;
    m_num += 1;
		succ = true;
	}

	return succ;
}

template<class T>
T Array<T>::removeLast()
{
	T x;
	if(!isEmpty()) {
    m_num -= 1;
		x = m_pData[m_num];
	}

	return x;
}

template <class T>
bool Array<T>::remove(const T & x)
{	
	bool succ = false;
	int index = indexOf(x);

	if (index >= 0) {
		for(int i = index; i < m_num - 1; i += 1) {
			m_pData[i] = m_pData[i + 1];
		}

		m_num -= 1;
		succ = true;
	}
	
	return succ;
}

template <class T>
bool Array<T>::remove(int index)
{	
	bool succ = false;

	if (index >= 0) {
		for(int i = index; i < m_num - 1; i += 1) {
			m_pData[i] = m_pData[i + 1];
		}

		m_num -= 1;
		succ = true;
	}
	
	return succ;
}

template <class T>
int Array<T>::indexOf(const T & x) const
{
	for(int index = 0; index < m_num; index += 1) {
		if (m_pData[index] == x) return index;
	}

	return -1;
}

template <class T>
int Array<T>::lastIndexOf(const T & x) const
{
	for(int index = m_num - 1; index >= 0; index -= 1) {
		if (m_pData[index] == x) return index;
	}

	return -1;
}

template <class T>
void Array<T>::copy(const Array & a)
{
	int size = 0;
	while (size < a.m_num) size += Array::DEF_SIZE;
	
	if ( m_curSize < a.m_num || m_curSize > size) {
		delete[] m_pData;
		m_pData = new T[m_curSize = size];
	}

	for(int i = 0; i < a.m_num; i += 1) {
		m_pData[i] = a.m_pData[i];
	}

	m_num = a.m_num;
}

template <class T>
bool Array<T>::growBy()
{
	T* pNew = new T[m_curSize + Array::DEF_SIZE];
	
	if (pNew) {
		for(int i = 0; i < m_num; i += 1) {
			pNew[i] = m_pData[i];
		}

		delete[] m_pData;
		m_pData = pNew;
		m_curSize += Array::DEF_SIZE;

		return true;
	}
  
	return false;
}

#endif