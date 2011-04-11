#pragma once

/*
 * Twodimensional arrays that try to avoid waste and frequent reallocation of memory.
 * Optimized for large numbers of sub-arrays and moderate numbers of elements contained
 * therein. Fast if elements are called sequential (arr[347][0], arr[347][1], arr[347][2], ...)
 * Only append (as push_back in vector class) is aviable.
 * potential ToDos:
 * 	-Deletion of subArray::elements
 *	-Optimization for randem acces
 */

#include "rmOut.h"

//sorted Version (read only). Very fast access, of course
template <class U>
class rmSArr2
{
	template <class T> friend class rmArr2;
public:
	class SAList
	{
	public:
		SAList(U * firstElement, rmU32 eCount)
		{
			this->firstElement = firstElement;
			this->eCount = eCount;
		}
		~SAList() {}
		U& operator [] (const rmU32 index) const
		{
			if (eCount <= index) {
				OUT_SERR("Index out of range")
			}
			return firstElement[index];
		}
		rmU32 getCount(void)
		{
			return eCount;
		}
	private:
		U * firstElement;
		rmU32 eCount;
	};
public:
	rmSArr2(void)
	{
		numLists = 0;
		buffer = 0;
		lists = 0;
		eCounts = 0;
	}
	~rmSArr2(void)
	{
		delete [] buffer;
		buffer = 0;
		delete [] lists;
		lists = 0;
		delete [] eCounts;
		eCounts = 0;
	}
	SAList operator [] (const rmU32 index) const
	{
		if (numLists <= index) {
			OUT_SERR("Index out of range")
		}
		return SAList(lists[index], eCounts[index]);
	}
	rmU32 getNumLists(void) const
	{
		return numLists;
	}
	U * getBuffer(void)
	{
	    return buffer;
	}

private:
	void init(rmU32 numElements, rmU32 numLists)
	{
		this->numLists = numLists;
		delete [] buffer;
		buffer = new U [numElements];
		eSpace = buffer;
		delete [] lists;
		lists = new U * [numLists];
		lSpace = lists;
		*lSpace = eSpace;
		delete [] eCounts;
		eCounts = new rmU32 [numLists];
		memset(eCounts, 0, (sizeof(rmU32) * numLists));
		eCount = eCounts;
	}
	void insert(U &element)
	{
		*eSpace = element;
		++eSpace;
		++(*eCount);
	}
	void newList(void)
	{
		++lSpace;
		*lSpace = eSpace;
		++eCount;
	}
	U * buffer;
	U * eSpace;
	U ** lists;
	U ** lSpace;
	rmU32 * eCounts;
	rmU32 * eCount;
	rmU32 numLists;
	static rmOut out;
};

template <class U>
rmOut rmSArr2<U>::out = rmOut("rmArr2.h");

//unsorted Version
template <class T>
class rmArr2
{
private:
	struct Element
	{
		T data;
		Element * next;
	};
	struct List
	{
		Element * first;
		Element * last;
		rmU32 count;
	};
public:
	class arr2List
	{
	public:
		arr2List(const rmArr2<T> * arr, const rmU32 num)
		{
			this->arr = const_cast<rmArr2<T> *>(arr);
			this->num = num;
		}
		~arr2List() {}
		T & operator [] (const rmU32 index) const
		{
			return arr->getElement(num, index)->data;
		}
		void append(const T elem) const
		{
			arr->append(elem, num);
		}
		rmU32 getCount(void) const
		{
			return arr->getNumElements(num);
		}
	private:
		rmArr2<T> * arr;
		rmU32 num;
	};
public:
	rmArr2(void)
	{
		this->buffer = 0;
		this->lists = 0;
		this->reset(0, 0);
	}
	rmArr2(const rmU32 approxTotalElementCount, const rmU32 approxListCount)
	{
		this->buffer = 0;
		this->lists = 0;
		this->reset(approxTotalElementCount, approxListCount);
	}
	~rmArr2(void)
	{
		delete [] buffer;
		delete [] lists;
	}
	void reset(const rmU32 approxTotalElementCount, const rmU32 approxListCount)
	{
		delete [] buffer;
		delete [] lists;
		this->currentElement = 0;
		this->currentList = -1;
		this->currentListElement = 0;
		this->numLists = approxListCount;
		approxTotalElementCount;
		this->numElements = (approxTotalElementCount + 1);
		this->remainingSpace = (approxTotalElementCount + 1);
		this->buffer = new Element [(approxTotalElementCount + 1)];
		this->lists = new List [approxListCount];
		space = buffer;
		memset(lists, 0, sizeof(List) * numLists);
		memset(buffer, 0, sizeof(buffer) * numElements);
	}
	rmU32 getNumElements(const rmU32 & listNum)
	{
		if (listNum > this->numLists) {
			extendLists(listNum);
		}
		return this->lists[listNum].count;
	}
	void append(const T & data, const rmU32 & listNum)
	{
		if (remainingSpace <= 1) {
			extendBuffer();
		}
		if (listNum > this->numLists) {
			extendLists(listNum);
		}
		space->data = data;
		List & l = lists[listNum];
		if (l.count == 0) {
			l.first = space;
			l.last = space;
		}
		else {
			l.last->next = space;
			l.last = space;
		}
		++l.count;
		++space;
		--remainingSpace;
	}
	Element * getElement(const rmU32 listNum, const rmU32 elementNum)
	{
		if ((currentList != listNum) || (elementNum < currentListElement)) {
			if (listNum > this->numLists) {
				extendLists(listNum);
			}
			if (lists[listNum].count <= elementNum) {
				OUT_SERR("Index out of range")
			}
			currentElement = lists[listNum].first;
			currentList = listNum;
			currentListElement = 0;
		}
		else {
			if (lists[listNum].count <= elementNum) {
				OUT_SERR("Index out of range")
			}
		}
		for (; currentListElement < elementNum; ++currentListElement) {
			currentElement = currentElement->next;
		}
		return currentElement;
	}
	arr2List operator [] (const rmU32 index)
	{
		if (index > this->numLists) {
			extendLists(index);
		}
		return arr2List(this, index);
	}
	void sort(rmSArr2<T> &dest)
	{
		dest.init(numElements, numLists);
		if (numLists != 0) {
			rmU32 i = 0;
			for (; i < (numLists - 1); ++i) {
				for (rmU32 j = 0; j < lists[i].count; ++j) {
					dest.insert((operator [](i))[j]);
				}
				dest.newList();
			}
			for (rmU32 j = 0; j < lists[i].count; ++j) {
				dest.insert((operator [](i))[j]);
			}
		}
	}
private:
	void shiftElementReferences(Element * oldBuffer)
	{
		rmU32 numStoredElements = numElements - remainingSpace;
		for (rmU32 i = 0; i < numStoredElements; ++i) {
			Element * & e = buffer[i].next;
			if (e) {
				e = (buffer + (e - oldBuffer));
			}
		}
		for (rmU32 i = 0; i < numLists; ++i) {
			List &l = lists[i];
			if (l.count > 0) {
				l.first = (buffer + (l.first - oldBuffer));
				l.last = (buffer + (l.last - oldBuffer));
			}
		}
		currentElement = (buffer + (currentElement - oldBuffer));
		space = (buffer + (space - oldBuffer));
	}
	void extendBuffer(void)
	{
		try {
			Element * oldBuffer = buffer;
			rmU32 oldNumElements = numElements;
			rmU32 newNumElements = ((oldNumElements * 12) / 10 + 1);
			Element * tmp = new Element [oldNumElements];
			memcpy(tmp, buffer, (sizeof(Element) * oldNumElements));
			delete [] buffer;
			buffer = new Element [newNumElements];
			memset(buffer, 0, sizeof(buffer) * newNumElements);
			memcpy(buffer, tmp, (sizeof(Element) * oldNumElements));
			delete [] tmp;
			shiftElementReferences(oldBuffer);
			numElements = newNumElements;
			remainingSpace += (newNumElements - oldNumElements);
		}
		catch (bad_alloc &) {
			OUT_SERR("Bad alloc when trying to extend buffer")
		}
	}
	void extendLists(rmU32 minNewCount)
	{
		try {
			rmU32 oldNumLists = numLists;
			rmU32 newNumLists = ((minNewCount * 12) / 10 + 1);
			List * tmp = new List [oldNumLists];
			memcpy(tmp, lists, (sizeof(List) * oldNumLists));
			delete [] lists;
			lists = new List [newNumLists];
			memset(lists, 0, (sizeof(List) * newNumLists));
			memcpy(lists, tmp, (sizeof(List) * oldNumLists));
			delete [] tmp;
			numLists = newNumLists;
		}
		catch (bad_alloc &) {
			OUT_SERR("Bad alloc when trying to extend lists")
		}
	}
	List * lists;
	Element * buffer;
	rmU32 currentList;
	rmU32 currentListElement;
	Element * space;
	Element * currentElement;
	rmU32 numLists;
	rmU32 numElements;
	rmU32 remainingSpace;
	static rmOut out;
};

template <class T>
rmOut rmArr2<T>::out = rmOut("rmArr2.h");
