//***************************************************************
// KDTree.h by Bojan Lovrovic (C) 2016 All Rights Reserved.
// 
// Structure that helps finding a point from the given set of
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
	inline KDTree(std::vector<std::array<Type, k>> &data);
	inline ~KDTree();

	// The nearest neighbour search (NN) algorithm aims to find the point in the tree that is nearest to a given input point.
	// Time complexity is O(log n).
	inline unsigned int FindNearestNeighbourIndex(const std::array<Type, k> &point) const;

private:
	inline Type GetDistanceSq(const std::array<Type, k> &p1, const std::array<Type, k> &p2) const;
	inline unsigned int GetSplittingAxis(unsigned int nodeIndex) const;
	// side: 0 - left, 1 - right
	inline unsigned int GetChildIndex(unsigned int nodeIndex, unsigned char side) const;
	inline void CreateNode(std::vector<std::array<Type, k>> &data, unsigned int nodeIndex);
	inline unsigned int NearestNeighbourIndexSearch(unsigned int nodeIndex, const std::array<Type, k> &point) const;
};

// **************************************************************
//						Definitions
// **************************************************************

template<class Type, int k>
KDTree<Type, k>::KDTree(std::vector<std::array<Type, k>> &data)
{
	// Create the root node.
	CreateNode(data, 1);
}

template<class Type, int k>
KDTree<Type, k>::~KDTree()
{

}

template<class Type, int k>
unsigned int KDTree<Type, k>::FindNearestNeighbourIndex(const std::array<Type, k> &point) const
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
	Type splittingValue = GetNode(nodeIndex)[splittingAxis];
	unsigned char first = point[splittingAxis] > splittingValue;
	unsigned int childIndex = GetChildIndex(nodeIndex, first);

	// Recursion
	unsigned int betterNodeI = nodeIndex;
	Type betterNodeDistanceSq = GetDistanceSq(point, GetNode(nodeIndex));
	if (IsNode(childIndex))
	{
		unsigned int childNodeI = NearestNeighbourIndexSearch(childIndex, point);
		Type childNodeDistanceSq = GetDistanceSq(point, GetNode(childNodeI));

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
			Type bestOnOtherSideDistanceSq = GetDistanceSq(point, GetNode(bestOnOtherSideI));
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
Type KDTree<Type, k>::GetDistanceSq(const std::array<Type, k> &p1, const std::array<Type, k> &p2) const
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
unsigned int KDTree<Type, k>::GetSplittingAxis(unsigned int nodeIndex) const
{
	return GetDepth(nodeIndex) % k;
}

template<class Type, int k>
unsigned int KDTree<Type, k>::GetChildIndex(unsigned int nodeIndex, unsigned char side) const
{
	if (side)
		return GetRightChildIndex(nodeIndex);
	else
		return GetLeftChildIndex(nodeIndex);
}

template<class Type, int k>
void KDTree<Type, k>::CreateNode(std::vector<std::array<Type, k>> &data, unsigned int nodeIndex)
{
	// Recursion base case
	if (data.empty())
		return;

	// Select axis based on depth so that axis cycles through all valid values
	int axis = GetSplittingAxis(nodeIndex);

	// Sort point list and choose median as a pivot element
	auto cmp = [axis](const std::array<Type, k> &left, const std::array<Type, k> &right) -> bool { return left[axis] < right[axis]; };
	std::sort(data.begin(), data.end(), cmp);
	int medianIndex = data.size() / 2;
	std::array<Type, k> median = data[medianIndex];

	// Create subsets
	std::vector<std::array<Type, k>>::iterator leftEnd = data.begin() + medianIndex;
	std::vector<std::array<Type, k>>::iterator rightBegin = leftEnd + 1;
	std::vector<std::array<Type, k>> leftData(data.begin(), leftEnd);
	std::vector<std::array<Type, k>> rightData(rightBegin, data.end());

	// Create node and construct subtrees
	if (nodeIndex >= mNodes.size())
		IncreaseDepth();
	mNodes[nodeIndex].first = true;
	mNodes[nodeIndex].second = median;
	CreateNode(leftData, GetLeftChildIndex(nodeIndex)); // recursive call
	CreateNode(rightData, GetRightChildIndex(nodeIndex)); // recursive call
}

#endif
