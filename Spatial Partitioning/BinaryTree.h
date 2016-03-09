//***************************************************************
// BinaryTree.h by Bojan Lovrovic (C) 2016 All Rights Reserved.
// 
// Structure that holds a binary tree inside the std::vector.
//***************************************************************

#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <vector>
#include <algorithm>

template<class BTNodeType>
class BinaryTree
{
public:
	inline BinaryTree();
	inline ~BinaryTree();

	inline void AddNode(const BTNodeType &newNode);
	inline const BTNodeType &GetNode(unsigned int nodeIndex) const;

	// Returns root index of the tree.
	inline unsigned int GetRoot() const;
	// Returns true only if the nodeIndex value represents the node that is not a root node.
	inline bool GetParentIndex(unsigned int nodeIndex, unsigned int *parentIndex) const;
	// Returns true only if the childIndex value represents the non-empty node.
	inline bool GetLeftChildIndex(unsigned int nodeIndex, unsigned int *childIndex) const;
	// Returns true only if the childIndex value represents the non-empty node.
	inline bool GetRightChildIndex(unsigned int nodeIndex, unsigned int *childIndex) const;
	// Returns depth of the passed node, with root node being at depth value equal to 0.
	inline static unsigned int GetDepth(unsigned int nodeIndex);
	// Unsafe GetParentIndex.
	inline unsigned int GetParentIndex(unsigned int nodeIndex) const;
	// Unsafe GetLeftChildIndex.
	inline unsigned int GetLeftChildIndex(unsigned int nodeIndex) const;
	// Unsafe GetRightChildIndex.
	inline unsigned int GetRightChildIndex(unsigned int nodeIndex) const;
	// Returns true if nodeIndex represents existent node.
	inline bool IsNode(unsigned int nodeIndex) const;

protected:
	// Increases the depth by one layer.
	inline void IncreaseDepth();

	std::vector<std::pair<bool, BTNodeType>> mNodes;
};

// **************************************************************
//						Definitions
// **************************************************************

template<class BTNodeType>
BinaryTree<BTNodeType>::BinaryTree()
{
	mNodes.resize(1);
}

template<class BTNodeType>
BinaryTree<BTNodeType>::~BinaryTree()
{

}

template<class BTNodeType>
const BTNodeType &BinaryTree<BTNodeType>::GetNode(unsigned int nodeIndex) const
{
	return mNodes[nodeIndex].second;
}

template<class BTNodeType>
unsigned int BinaryTree<BTNodeType>::GetRoot() const
{
	return 1;
}

template<class BTNodeType>
bool BinaryTree<BTNodeType>::GetParentIndex(unsigned int nodeIndex, unsigned int *parentIndex) const
{
	*parentIndex = nodeIndex / 2;
	return (*parentIndex > 0);
}

template<class BTNodeType>
bool BinaryTree<BTNodeType>::GetLeftChildIndex(unsigned int nodeIndex, unsigned int *childIndex) const
{
	*childIndex = nodeIndex * 2;
	return (*childIndex < mNodes.size() && mNodes[*childIndex].first);
}

template<class BTNodeType>
bool BinaryTree<BTNodeType>::GetRightChildIndex(unsigned int nodeIndex, unsigned int *childIndex) const
{
	*childIndex = nodeIndex * 2 + 1;
	return (*childIndex < mNodes.size() && mNodes[*childIndex].first);
}

template<class BTNodeType>
unsigned int BinaryTree<BTNodeType>::GetDepth(unsigned int nodeIndex)
{
	return (unsigned int)log2(nodeIndex);
}

template<class BTNodeType>
unsigned int BinaryTree<BTNodeType>::GetParentIndex(unsigned int nodeIndex) const
{
	return nodeIndex / 2;
}

template<class BTNodeType>
unsigned int BinaryTree<BTNodeType>::GetLeftChildIndex(unsigned int nodeIndex) const
{
	return nodeIndex * 2;
}

template<class BTNodeType>
unsigned int BinaryTree<BTNodeType>::GetRightChildIndex(unsigned int nodeIndex) const
{
	return nodeIndex * 2 + 1;
}

template<class BTNodeType>
bool BinaryTree<BTNodeType>::IsNode(unsigned int nodeIndex) const
{
	return (nodeIndex < mNodes.size() && mNodes[nodeIndex].first);
}

template<class BTNodeType>
void BinaryTree<BTNodeType>::IncreaseDepth()
{
	unsigned int newSize = 2 * mNodes.size();
	mNodes.resize(newSize);
}

#endif
