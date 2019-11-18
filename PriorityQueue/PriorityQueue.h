
//***************************************************************
// PriorityQueue.h by Bojan Lovrovic (C) 2019 All Rights Reserved.
//
// This class augments the STL implementation of 
// the heap. Namely the priority update feature is missing
// in the STL implementation and that is the main purpose
// behind this code. Other than that, using this class
// should hide away some of the code necessary for STL
// heap to work. Similar to std::priority_queue.
//***************************************************************

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>

template <class T>
class PriorityQueue
{
	friend class ElementHandle;

public:
	class ElementHandle
	{
		friend class PriorityQueue<T>;

	public:
		~ElementHandle();
		
		// Updates the element to a new value and then updates the
		// containing queue. Returns true if successful.
		bool Update(const T& newVal);

	private:
		ElementHandle(PriorityQueue<T> *ownerPt, size_t thisPos);
		PriorityQueue<T> *const mOwnerPt;
		size_t mThisPos;
	};

public:
	// Default constructor
	PriorityQueue();

	// Constructor with a comparison function
	PriorityQueue(const std::function<bool(T, T)>& comp);

	// Check whether the queue has no elements.
	// Executes in O(1) time.
	bool Empty() const;

	// Add an element to the queue with an associated priority.
	// Executes in O(log n) time on average.
	void Add(const T& e);

	// Remove the element from the queue that has the highest priority, and return it.
	// Executes in O(1) time.
	T Remove();

	// Peek, which returns the highest - priority element but does not modify the queue.
	// Executes in O(1) time.
	T Peek() const;

	// Clears everything in the queue
	void Clear();

	// Get a handle that will allow to update elements in the heap
	ElementHandle GetElementHandle(const T& e);

private:
	size_t GetParentPosition(size_t child);
	size_t GetLeftChildPosition(size_t parent);
	size_t GetRightChildPosition(size_t parent);
	void Swap(size_t e1, size_t e2);
	void UpdatePriority(size_t ePos);

private:
	std::vector<T> mData;
	std::unordered_map<T, size_t> mValToPos;
	std::function<bool(T, T)> mComp;
	int mLock;
};

template<class T>
inline PriorityQueue<T>::PriorityQueue()
	: PriorityQueue<T>(std::less<int>())
{

}

template<class T>
inline PriorityQueue<T>::PriorityQueue(const std::function<bool(T, T)>& comp)
	: mComp(comp)
	, mLock(0)
{
}

template<class T>
inline size_t PriorityQueue<T>::GetParentPosition(size_t child)
{
	return ((child - 1) / 2);
}

template<class T>
inline size_t PriorityQueue<T>::GetLeftChildPosition(size_t parent)
{
	return (parent * 2 + 1);
}

template<class T>
inline size_t PriorityQueue<T>::GetRightChildPosition(size_t parent)
{
	return (parent * 2 + 2);
}

template<class T>
inline void PriorityQueue<T>::Swap(size_t e1, size_t e2)
{
	T& e1V = mData[e1];
	T& e2V = mData[e2];
	std::swap(mValToPos[e1V], mValToPos[e2V]);
	std::swap(e1V, e2V);
}

template<class T>
inline void PriorityQueue<T>::UpdatePriority(size_t ePos)
{
	// Bubble it up
	while (ePos != 0 && mComp(mData[GetParentPosition(ePos)], mData[ePos]))
	{
		size_t pPos = GetParentPosition(ePos);
		Swap(pPos, ePos);
		ePos = pPos;
	}

	// Or bubble down
	while (true)
	{
		size_t lcPos = GetLeftChildPosition(ePos);
		size_t rcPos = GetRightChildPosition(ePos);
		size_t higherPriorityChildPos;
		// The left child is out of bounds, meaning the right one is too
		// in which case we are done
		if (lcPos >= mData.size())
			break;
		// The right child is out of bounds, meaning the left one is the only option
		else if (rcPos >= mData.size())
			higherPriorityChildPos = lcPos;
		// Both children are within the bounds, meaning we need to prioritize
		// between them.
		else
			higherPriorityChildPos = (mComp(mData[lcPos], mData[rcPos])) ? rcPos : lcPos;

		if (mComp(mData[ePos], mData[higherPriorityChildPos]))
		{
			Swap(ePos, higherPriorityChildPos);
			ePos = higherPriorityChildPos;
		}
		else
			break;
	}
}

template<class T>
inline bool PriorityQueue<T>::Empty() const
{
	return mData.empty();
}

template<class T>
inline void PriorityQueue<T>::Add(const T& e)
{
	// Throw if we already have active handles
	if (mLock >= 1) throw;

	// Return early if the element is already present 
	if (mValToPos.find(e) != mValToPos.end())
		return;

	// Get the initial element position
	size_t ePos = mData.size();

	// Add it to the heap
	mData.push_back(e);

	// Save it's position
	mValToPos[e] = ePos;

	// Update the priority of the newly added element
	UpdatePriority(ePos);
}

template<class T>
inline T PriorityQueue<T>::Remove()
{
	// Throw if we already have active handles
	if (mLock >= 1) throw;

	// Return early if there are no elements
	if (mValToPos.empty())
		return T();

	// Get the highest priority element and swap it with the last
	// element in the queue (whilst keeping track of value-to-position map)
	Swap(0, mData.size() - 1);

	// Get a copy of the highest priority element (now at the back)
	T ret = mData.back();

	// Remove the last element
	mValToPos.erase(ret);
	mData.pop_back();

	// Update the priority of the newly moved element at the top
	UpdatePriority(0);

	// Return the best element
	return ret;
}

template<class T>
inline T PriorityQueue<T>::Peek() const
{
	return mData.front();
}

template<class T>
inline void PriorityQueue<T>::Clear()
{
	// Throw if we already have active handles
	if (mLock >= 1) throw;

	// Clear everything
	mData.clear();
	mValToPos.clear();
}

template<class T>
typename PriorityQueue<T>::ElementHandle PriorityQueue<T>::GetElementHandle(const T& e)
{
	size_t ePos = mValToPos[e];
	return ElementHandle(this, ePos);
}

template<class T>
inline PriorityQueue<T>::ElementHandle::ElementHandle(PriorityQueue<T>* ownerPt, size_t thisPos)
	: mOwnerPt(ownerPt)
	, mThisPos(thisPos)
{
	mOwnerPt->mLock++;

	// Throw if the number of handles is greater than one
	if (mOwnerPt->mLock > 1) throw;
}

template<class T>
inline PriorityQueue<T>::ElementHandle::~ElementHandle()
{
	mOwnerPt->mLock--;
}

template<class T>
inline bool PriorityQueue<T>::ElementHandle::Update(const T &newVal)
{
	// Return early if the element with the new value is already present 
	if (mOwnerPt->mValToPos.find(newVal) != mOwnerPt->mValToPos.end())
		return false;

	// Erase it from the map
	T& oldVal = mOwnerPt->mData[mThisPos];
	mOwnerPt->mValToPos.erase(oldVal);

	// Update and add the new enty to the map
	mOwnerPt->mData[mThisPos] = newVal;
	mOwnerPt->mValToPos[newVal] = mThisPos;

	// Fix the heap and update the index of this element
	mOwnerPt->UpdatePriority(mThisPos);
	mThisPos = mOwnerPt->mValToPos[newVal];
	return true;
}

#endif
