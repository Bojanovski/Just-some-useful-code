//***************************************************************
// KDTree.h by Bojan Lovrovic (C) 2016 All Rights Reserved.
// 
// Structure that enables finding a point from the given set of
// points that is nearest to the input point.
//***************************************************************

#ifndef KDTREE_H
#define KDTREE_H

#include <array>
#include "BinaryTree.h"

template<class Type, int k>
class KDTree : public BinaryTree<std::array<Type, k>>
{
public:
	KDTree() {}
	KDTree(const std::vector<std::array<Type, k>> &data);
	~KDTree() {}

	void Initialize(const std::vector<std::array<Type, k>> &data);

	// The nearest neighbour search (NN) algorithm aims to find the point in the tree that is nearest to a given input point.
	// Time complexity is O(log n).
	unsigned int FindNearestNeighbourIndex(const std::array<Type, k> &point) const;

private:
	Type GetDistanceSq(const std::array<Type, k> &p1, const std::array<Type, k> &p2) const;
	unsigned int GetSplittingAxis(unsigned int nodeIndex) const;
	// side: 0 - left, 1 - right
	unsigned int GetChildIndex(unsigned int nodeIndex, unsigned char side) const;
	void CreateNode(std::array<Type, k> *data, unsigned int dataElementsNumber, unsigned int nodeIndex);
	unsigned int NearestNeighbourIndexSearch(unsigned int nodeIndex, const std::array<Type, k> &point) const;
};

// **************************************************************
//						Definitions
// **************************************************************

template<class Type, int k>
inline KDTree<Type, k>::KDTree(const std::vector<std::array<Type, k>> &data)
{
	Initialize(data);
}

template<class Type, int k>
inline void KDTree<Type, k>::Initialize(const std::vector<std::array<Type, k>> &data)
{
	// Create a copy of 'data' to work with so that the original stays unchanged.
	std::vector<std::array<Type, k>> dataCopy = data;
	// Create the root node.
	CreateNode(&dataCopy[0], dataCopy.size(), 1);
}

template<class Type, int k>
inline unsigned int KDTree<Type, k>::FindNearestNeighbourIndex(const std::array<Type, k> &point) const
{
	return NearestNeighbourIndexSearch(GetRoot(), point);
}

template<class Type, int k>
inline unsigned int KDTree<Type, k>::NearestNeighbourIndexSearch(unsigned int nodeIndex, const std::array<Type, k> &point) const
{
	// Recursion base case
	if (!IsNode(nodeIndex))
		return nodeIndex;

	// Retrieve splitting axis
	unsigned int splittingAxis = GetSplittingAxis(nodeIndex);

	// Find the child to recurse into first.
	Type splittingValue = GetElement(nodeIndex)[splittingAxis];
	unsigned char first = point[splittingAxis] > splittingValue;
	unsigned int childIndex = GetChildIndex(nodeIndex, first);

	// Recursion
	unsigned int betterNodeI = nodeIndex;
	Type betterNodeDistanceSq = GetDistanceSq(point, GetElement(nodeIndex));
	if (IsNode(childIndex))
	{
		unsigned int childNodeI = NearestNeighbourIndexSearch(childIndex, point);
		Type childNodeDistanceSq = GetDistanceSq(point, GetElement(childNodeI));

		if (childNodeDistanceSq < betterNodeDistanceSq)
		{
			betterNodeI = childNodeI;
			betterNodeDistanceSq = childNodeDistanceSq;
		}
	}

	// Check if the other side of the splitting axes needs to be checked.
	Type distanceOnTheSplittingAxisSq = pow(point[splittingAxis] - splittingValue, 2);
	if (distanceOnTheSplittingAxisSq < betterNodeDistanceSq)
	{ // It needs to be checked.
		unsigned int otherSideChildIndex = GetChildIndex(nodeIndex, first ^ 1);

		// Check for new best.
		if (IsNode(otherSideChildIndex))
		{
			unsigned int bestOnOtherSideI = NearestNeighbourIndexSearch(otherSideChildIndex, point);
			Type bestOnOtherSideDistanceSq = GetDistanceSq(point, GetElement(bestOnOtherSideI));
			if (bestOnOtherSideDistanceSq < betterNodeDistanceSq)
			{
				betterNodeI = bestOnOtherSideI;
				betterNodeDistanceSq = bestOnOtherSideDistanceSq;
			}
		}
	}

	return betterNodeI;
}

template<class Type, int k>
inline Type KDTree<Type, k>::GetDistanceSq(const std::array<Type, k> &p1, const std::array<Type, k> &p2) const
{
	Type diff = p1[0] - p2[0];
	Type distSq = diff * diff;
	for (unsigned int i = 1; i < k; ++i)
	{
		diff = p1[i] - p2[i];
		distSq += diff * diff;
	}
	return distSq;
}

template<class Type, int k>
inline unsigned int KDTree<Type, k>::GetSplittingAxis(unsigned int nodeIndex) const
{
	return GetDepth(nodeIndex) % k;
}

template<class Type, int k>
inline unsigned int KDTree<Type, k>::GetChildIndex(unsigned int nodeIndex, unsigned char side) const
{
	if (side)
		return GetRightChildIndex(nodeIndex);
	else
		return GetLeftChildIndex(nodeIndex);
}

template<class Type, int k>
inline void KDTree<Type, k>::CreateNode(std::array<Type, k> *data, unsigned int dataElementsNumber, unsigned int nodeIndex)
{
	// Recursion base case
	if (dataElementsNumber == 0)
		return;

	// Select axis based on depth so that axis cycles through all valid values
	int axis = GetSplittingAxis(nodeIndex);

	// Sort point list and choose median as a pivot element
	auto cmp = [axis](const std::array<Type, k> &left, const std::array<Type, k> &right) -> bool { return left[axis] < right[axis]; };
	std::sort(data, data + dataElementsNumber, cmp);
	int medianIndex = dataElementsNumber / 2;
	std::array<Type, k> median = data[medianIndex];

	// Create subsets
	std::array<Type, k> *leftBegin = data;
	std::array<Type, k> *leftEnd = data + medianIndex;
	std::array<Type, k> *rightBegin = leftEnd + 1;
	std::array<Type, k> *rightEnd = data + dataElementsNumber;
	// Note: rightEnd and leftEnd represent the end of the field, meaning there is not
	// an element of this field on the location this pointer is pointing

	// Create node and construct subtrees
	if (nodeIndex >= mElements.size())
		IncreaseDepth();
	mElements[nodeIndex].first = true;
	mElements[nodeIndex].second = median;
	CreateNode(leftBegin, leftEnd - leftBegin, GetLeftChildIndex(nodeIndex)); // recursive call
	CreateNode(rightBegin, rightEnd - rightBegin, GetRightChildIndex(nodeIndex)); // recursive call
}

#endif
