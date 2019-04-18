#pragma once

template <class T>
class Array
{
public:
	static const int DEF_SIZE;
private:
	T *m_pData;
	int m_curSize, m_num;
public:
	Array(int size = DEF_SIZE);
	Array(const Array & a);
	~Array();
	Array& operator = (const Array & a);

	T& operator [] (int index);
	const T& operator [] (int index) const;

	bool isFull() const;
	bool isEmpty() const;

	int getCount() const;

	bool insert(const T & x, int index = -1);
	bool remove(const T & x);
	bool insertLast(const T& x);
	T removeLast();

	int indexOf(const T & x) const;
	int lastIndexOf(const T & x) const;

private:
	void copy(const Array & a);
	bool growBy();
};

#include "Array.cpp"