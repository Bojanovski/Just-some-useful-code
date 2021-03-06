//***************************************************************
// Graph.h by Bojan Lovrovic (C) 2016 All Rights Reserved.
//***************************************************************

#ifndef GRAPH_H
#define GRAPH_H

#include <cassert>
#include <vector>
#include <algorithm>

template<class WeightType>
struct Edge
{
	// vertices
	unsigned int v1, v2;

	// weight
	WeightType w;

	Edge(unsigned int v1, unsigned int v2, WeightType w) : v1(v1), v2(v2), w(w) {}
};

template<class VertexType, class WeightType>
class Graph
{
public:
	Graph(WeightType minValue, WeightType maxValue) : mMinValue(minValue), mMaxValue(maxValue) {}
	~Graph() {}

	void Clear();
	void AddVertex(const VertexType &vert) { mVertices.push_back(vert); }
	void RemoveVertexAt(unsigned int index);
	void AddEdge(unsigned int v1, unsigned int v2, const WeightType &weight);
	void AddEdge(const VertexType &vert1, const VertexType &vert2, WeightType weight);
	// Calculate internal data. Needs to be called before any use of the Graph object.
	void UpdateEdges();

	std::vector<VertexType> const *GetVertices() const { return &mVertices; }
	VertexType &GetVertexAt(unsigned int index) { return mVertices[index]; }
	unsigned int GetVertexIndex(const VertexType &vert) const;
	unsigned int GetNumberOfVertices() const { return mVertices.size(); }
	Edge<WeightType> &GetEdgeAt(unsigned int index) { return mEdges[index]; }
	Edge<WeightType> &GetEdge(unsigned int v1, unsigned int v2);
	WeightType &GetEdgeWeight(unsigned int v1, unsigned int v2);
	unsigned int GetNumberOfEdges() const { return mEdges.size(); }

	// Dijkstra algorithm will return true if path is found and false if it's not.
	// outPath		- found path consisting of vertices
	// d			- total weight of the found path
	bool Dijkstra(const VertexType &source, const VertexType &dest, std::vector<unsigned int> *outPath, WeightType *d) const;
	bool Dijkstra(const unsigned int sI, const unsigned int dI, std::vector<unsigned int> *outPath, WeightType *d) const;

	// Prim's algorithm, used to generate MST graph. It will return false if all vertices are not connected and true otherwise.
	// outGraph		- MST graph generated from the input graph
	bool Prim(const VertexType &source, Graph<VertexType, WeightType> *outGraph) const;
	bool Prim(const unsigned int sI, Graph<VertexType, WeightType> *outGraph) const;

private:
	// Holds the vertex object index and the some WeightType value associated with it.
	struct VertexWeight
	{
		unsigned int vI;
		WeightType w;
	};

	// STL does not have a decrease key operation so this is my implementation.
	// The argument 'newValue' needs to be smaller than the current value in heap at 'keyIndex' position.
	void HeapDecreaseKey(std::vector<VertexWeight> &heap, const WeightType &newValue, unsigned int keyIndex) const;

	// y must be smaller than x
	unsigned int GetIndexForCompact(unsigned int x, unsigned int y) const;

	// Data members
	std::vector<VertexType> mVertices;
	std::vector<Edge<WeightType>> mEdges;
	std::vector<WeightType> mEdgesCompact;
	WeightType mMinValue, mMaxValue;
};

// **************************************************************
//						Definitions
// **************************************************************
template<class VertexType, class WeightType>
inline void Graph<VertexType, WeightType>::Clear()
{
	mVertices.clear();
	mEdges.clear();
	mEdgesCompact.clear();
}

template<class VertexType, class WeightType>
inline void Graph<VertexType, WeightType>::RemoveVertexAt(unsigned int index)
{
	mVertices.erase(mVertices.begin() + index);
	int eI = 0;
	while (eI != mEdges.size())
	{
		if (mEdges[eI].v1 == index || mEdges[eI].v2 == index)
			mEdges.erase(mEdges.begin() + eI);
		else
		{
			// first check if vertex indices need to be updated
			if (mEdges[eI].v1 > index) --mEdges[eI].v1;
			if (mEdges[eI].v2 > index) --mEdges[eI].v2;
			++eI;
		}
	}
}

template<class VertexType, class WeightType>
inline void Graph<VertexType, WeightType>::AddEdge(unsigned int v1, unsigned int v2, const WeightType &weight)
{
	// v1 must be smaller than v2.
	assert(v1 != v2);
	if (v1 > v2)
	{ // switch values
		unsigned int temp = v2;
		v2 = v1;
		v1 = temp;
	}

	mEdges.push_back(Edge<WeightType>(v1, v2, weight));
}

template<class VertexType, class WeightType>
inline void Graph<VertexType, WeightType>::AddEdge(const VertexType &vert1, const VertexType &vert2, WeightType weight)
{
	unsigned int v1 = GetVertexIndex(vert1);
	unsigned int v2 = GetVertexIndex(vert2);
	AddEdge(v1, v2, weight);
}

template<class VertexType, class WeightType>
inline void Graph<VertexType, WeightType>::UpdateEdges()
{
	// Recalculate compact edges
	mEdgesCompact.resize((unsigned int)(mVertices.size() * (mVertices.size() - 1) * 0.5f));
	for (auto &edge : mEdgesCompact)
		edge = mMaxValue;

	for (auto &edge : mEdges)
	{
		unsigned int index = GetIndexForCompact(edge.v2, edge.v1);
		mEdgesCompact[index] = edge.w;
	}
}

template<class VertexType, class WeightType>
inline unsigned int Graph<VertexType, WeightType>::GetVertexIndex(const VertexType &vert) const
{
	auto it = find(mVertices.begin(), mVertices.end(), vert);
	assert(it != mVertices.end());
	return it - mVertices.begin();
}

template<class VertexType, class WeightType>
inline Edge<WeightType> &Graph<VertexType, WeightType>::GetEdge(unsigned int v1, unsigned int v2)
{
	// v1 must be smaller than v2.
	assert(v1 != v2);
	if (v1 > v2)
	{ // switch values
		unsigned int temp = v2;
		v2 = v1;
		v1 = temp;
	}

	unsigned int ceI = GetIndexForCompact(v2, v1);
	WeightType edgeW = mEdgesCompact[ceI];
	Edge<WeightType> retEdge(v1, v2, edgeW);
	return retEdge;
}

template<class VertexType, class WeightType>
inline WeightType &Graph<VertexType, WeightType>::GetEdgeWeight(unsigned int v1, unsigned int v2)
{
	// v1 must be smaller than v2.
	assert(v1 != v2);
	if (v1 > v2)
	{ // switch values
		unsigned int temp = v2;
		v2 = v1;
		v1 = temp;
	}

	unsigned int ceI = GetIndexForCompact(v2, v1);
	WeightType edgeW = mEdgesCompact[ceI];
	return edgeW;
}

template<class VertexType, class WeightType>
inline bool Graph<VertexType, WeightType>::Dijkstra(const VertexType &source, const VertexType &dest, std::vector<unsigned int> *outPath, WeightType *d) const
{
	unsigned int sI = GetVertexIndex(source);
	unsigned int dI = GetVertexIndex(dest);
	return Dijkstra(sI, dI, outPath, d);
}

template<class VertexType, class WeightType>
inline bool Graph<VertexType, WeightType>::Dijkstra(const unsigned int sI, const unsigned int dI, std::vector<unsigned int> *outPath, WeightType *d) const
{
	outPath->clear();
	*d = mMinValue;

	if (sI == dI) // Handle this special case.
	{
		outPath->push_back(sI); // just add source vertex
		// *d is already equal to mMinValue
		return true;
	}

	std::vector<int> predecessors;
	predecessors.resize(mVertices.size());
	std::vector<VertexWeight> toBeCh;
	toBeCh.resize(mVertices.size());
	for (unsigned int i = 0; i < toBeCh.size(); ++i)
	{
		toBeCh[i].vI = i;
		toBeCh[i].w = mMaxValue;
		predecessors[i] = -1;
	}
	toBeCh[sI].w = mMinValue;

	// make heap
	auto cmp = [](const VertexWeight &left, const VertexWeight &right) { return (left.w) >(right.w); };
	std::make_heap(toBeCh.begin(), toBeCh.end(), cmp);

	while (!toBeCh.empty())
	{
		// Remove the root of the min_heap structure (the index with the smallest w).
		std::pop_heap(toBeCh.begin(), toBeCh.end(), cmp);
		unsigned int vI = toBeCh.back().vI;
		WeightType vW = toBeCh.back().w;
		toBeCh.pop_back();
		if (vW == mMaxValue)
			return false; // source and destination are not connected

		if (vI == dI)
		{ // destination reached
			// reconstruct path
			unsigned int vertI = dI;
			do
			{
				outPath->push_back(vertI);
				vertI = predecessors[vertI];
			} while (vertI != sI); // until we arrive at the source
			*d = vW;
			outPath->push_back(sI); // just add source vertex
			return true;
		}

		for (unsigned int i = 0; i < toBeCh.size(); ++i)
		{
			unsigned int uI = toBeCh[i].vI;
			unsigned int ceI = GetIndexForCompact(max(vI, uI), min(vI, uI));
			WeightType edgeW = mEdgesCompact[ceI];
			if (edgeW == mMaxValue) continue; // vI and uI are not connected

			WeightType uW = toBeCh[i].w;
			WeightType uW_new = vW + edgeW;
			if (uW_new < uW)
			{
				predecessors[uI] = vI;
				HeapDecreaseKey(toBeCh, uW_new, i);
			}
		}
	}
	return false;
}

template<class VertexType, class WeightType>
inline bool Graph<VertexType, WeightType>::Prim(const VertexType &source, Graph<VertexType, WeightType> *outGraph) const
{
	unsigned int sI = GetVertexIndex(source);
	return Primm(sI, outGraph);
}

template<class VertexType, class WeightType>
inline bool Graph<VertexType, WeightType>::Prim(const unsigned int sI, Graph<VertexType, WeightType> *outGraph) const
{
	outGraph->mEdges.clear();
	std::vector<int> predecessors;
	predecessors.resize(mVertices.size());
	std::vector<VertexWeight> toBeCh;
	toBeCh.resize(mVertices.size());
	for (unsigned int i = 0; i < toBeCh.size(); ++i)
	{
		toBeCh[i].vI = i;
		toBeCh[i].w = mMaxValue;
		predecessors[i] = -1;
	}
	toBeCh[sI].w = mMinValue;

	// make heap
	auto cmp = [](const VertexWeight &left, const VertexWeight &right) { return (left.w) >(right.w); };
	std::make_heap(toBeCh.begin(), toBeCh.end(), cmp);

	while (!toBeCh.empty())
	{
		// Remove the root of the min_heap structure (the index with the smallest w).
		std::pop_heap(toBeCh.begin(), toBeCh.end(), cmp);
		unsigned int vI = toBeCh.back().vI;
		WeightType vW = toBeCh.back().w;
		toBeCh.pop_back();
		if (vW == mMaxValue)
			return false; // source and some vertex are not connected

		for (unsigned int i = 0; i < toBeCh.size(); ++i)
		{
			unsigned int uI = toBeCh[i].vI;
			unsigned int ceI = GetIndexForCompact(max(vI, uI), min(vI, uI));
			WeightType edgeW = mEdgesCompact[ceI];
			if (edgeW == mMaxValue) continue; // vI and uI are not connected

			WeightType uW = toBeCh[i].w;
			WeightType uW_new = edgeW;
			if (uW_new < uW)
			{
				predecessors[uI] = vI;
				HeapDecreaseKey(toBeCh, uW_new, i);
			}
		}
	}

	// All vertices connected, reconstruct graph.
	outGraph->mVertices.reserve(mVertices.size());
	for (unsigned int i = 0; i < mVertices.size(); ++i) // copy vertex data
	{
		outGraph->mVertices.push_back(mVertices[i]);
	}

	for (unsigned int i = 0; i < mVertices.size(); ++i) // create edges
	{
		if (i != sI) // root does not have a predecessor
		{
			unsigned int pred = predecessors[i];
			unsigned int ceI = GetIndexForCompact(max(pred, i), min(pred, i));
			WeightType edgeW = mEdgesCompact[ceI];
			outGraph->AddEdge(predecessors[i], i, edgeW);
		}
	}

	outGraph->UpdateEdges();
	return true;
}

template<class VertexType, class WeightType>
inline void Graph<VertexType, WeightType>::HeapDecreaseKey(std::vector<VertexWeight> &heap, const WeightType &newValue, unsigned int keyIndex) const
{
	heap[keyIndex].w = newValue; // 'decrease key' operation

	// update heap after 'decrease key'
	bool check = true;
	unsigned int currentNodeI = keyIndex;
	while (check && currentNodeI != 0)
	{
		check = false;
		unsigned parentI = (currentNodeI + 1) / 2 - 1;
		unsigned int leftChildI = 2 * (parentI + 1) - 1;
		unsigned int rightChildI = 2 * (parentI + 1) + 1 - 1;
		unsigned int childToSwapWithI;

		if (leftChildI >= heap.size() && rightChildI >= heap.size())
			break; // node has no children
		else if (leftChildI >= heap.size() && rightChildI < heap.size())
			childToSwapWithI = rightChildI; // has only right child
		else if (leftChildI < heap.size() && rightChildI >= heap.size())
			childToSwapWithI = leftChildI; // has only left child
		else // has both children
			childToSwapWithI = (heap[leftChildI].w < heap[rightChildI].w) ? leftChildI : rightChildI;

		if (heap[childToSwapWithI].w < heap[parentI].w)
		{ // swap
			VertexWeight temp = heap[parentI];
			heap[parentI] = heap[childToSwapWithI];
			heap[childToSwapWithI] = temp;
			currentNodeI = parentI;
			check = true;
		}
	}
}

template<class VertexType, class WeightType>
inline unsigned int Graph<VertexType, WeightType>::GetIndexForCompact(unsigned int x, unsigned int y) const
{
	return (unsigned int)(x - y - 1 + mVertices.size() * y - y * (y + 1) * 0.5f);
}

#endif
