//***************************************************************
// KDTreeMap.h by Bojan Lovrovic (C) 2016 All Rights Reserved.
// 
// Structure that enables finding a value of a point from
// the given set of points that is nearest to the input point.
//***************************************************************

#ifndef KDTREEMAP_H
#define KDTREEMAP_H

#include "KDTree.h"

template<class KeyComponentType, int KeyDimension, class ValueType>
class KDTreeMap : protected KDTree<KeyComponentType, KeyDimension>
{
private:
//***************************************************************
//	The idea behind the following structure is to break the key
//	into 'KeyDimension' pieces and associate it with index of a
//	value it represents. This indices will later be used to map
//	the rearranged keys to their respective values.
//***************************************************************
	template<class KeyComponentType>
	struct KeyFraction
	{
		KeyComponentType mKeyComponent;
		/* Index of a value in the given field of values where this KeyFraction
		is a fraction of a whole key associated with said value. Redundancy might
		appear as a problem because the same 'mValueIndex' is used for every
		fraction of a key, but bear in mind that this will only exist in memory
		during the initialization of the structure. */
		unsigned int mValueIndex;

		bool operator< (const KeyFraction<KeyComponentType> &right) const
		{
			return (mKeyComponent < right.mKeyComponent);
		}
	};
	typedef KeyFraction<KeyComponentType> KeyFracType;

public:
	KDTreeMap() {}
	KDTreeMap(const std::vector<std::array<KeyComponentType, KeyDimension>> &keys, const std::vector<ValueType> &values);
	~KDTreeMap() {}

	void Initialize(const std::vector<std::array<KeyComponentType, KeyDimension>> &keys, const std::vector<ValueType> &values);

	// The nearest neighbour search (NN) algorithm aims to find the point in the tree that is nearest to a given input point
	// and then return the value associated with it. Time complexity is O(log n).
	ValueType FindNearestNeighbourValue(const std::array<KeyComponentType, KeyDimension> &point) const;

private:
	std::vector<ValueType> mValues;
	std::vector<unsigned int> mKeyToValueMap;
};

// **************************************************************
//						Definitions
// **************************************************************

template<class KeyComponentType, int KeyDimension, class ValueType>
inline KDTreeMap<KeyComponentType, KeyDimension, ValueType>::KDTreeMap(
	const std::vector<std::array<KeyComponentType, KeyDimension>> &keys,
	const std::vector<ValueType> &values)
{
	Initialize(keys, values);
}

template<class KeyComponentType, int KeyDimension, class ValueType>
inline void KDTreeMap<KeyComponentType, KeyDimension, ValueType>::Initialize(
	const std::vector<std::array<KeyComponentType, KeyDimension>> &keys,
	const std::vector<ValueType> &values)
{
	assert(keys.size() == values.size());
	mValues.clear();
	mKeyToValueMap.clear();
	KDTree<KeyComponentType, KeyDimension>::Initialize(keys);
	mValues = values;

	// Copy the keys to keysAu, where 'Au' stands for augmented (because of the additional data they contain inside
	// 'KeyFraction<KeyComponentType, KeyDimension>::mValueIndexFraction').
	std::vector<std::array<KeyFracType, KeyDimension>> keysAu;
	keysAu.reserve(keys.size());
	for (unsigned int i = 0; i < keys.size(); ++i)
	{
		std::array<KeyFracType, KeyDimension> temp;
		for (unsigned int j = 0; j < KeyDimension; ++j)
		{
			temp[j].mKeyComponent = keys[i][j];
			temp[j].mValueIndex = i;
		}
		keysAu.push_back(temp);
	}

	KDTree<KeyFracType, KeyDimension> mKDT_temp(keysAu);
	mKeyToValueMap.resize(mKDT_temp.GetNumberOfElements());
	for (unsigned int i = 0; i < mKDT_temp.GetNumberOfElements(); ++i)
	{
		if (mKDT_temp.IsNode(i)) // is not an empty node (see implementation of BinaryTree for details)
		{
			std::array<KeyFracType, KeyDimension> temp = mKDT_temp.GetElement(i);
			mKeyToValueMap[i] = temp[0].mValueIndex;
		}
	}
}

template<class KeyComponentType, int KeyDimension, class ValueType>
inline ValueType KDTreeMap<KeyComponentType, KeyDimension, ValueType>::FindNearestNeighbourValue(
	const std::array<KeyComponentType, KeyDimension> &point) const
{
	unsigned int i = FindNearestNeighbourIndex(point); // index in the binary tree internal array
	i = mKeyToValueMap[i]; // remap to index in the values array
	return mValues[i];
}

#endif
