
#ifndef _DIJKSTRA_H
#define _DIJKSTRA_H

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

template<class VerticeType, class WeightType>
class Graph
{
public:
	Graph(WeightType minValue, WeightType maxValue) : mMinValue(minValue), mMaxValue(maxValue) {}
	~Graph() {}

	inline void Clear();
	inline void AddVertice(const VerticeType &vert) { mVertices.push_back(vert); }
	void RemoveVerticeAt(unsigned int index);
	void AddEdge(unsigned int v1, unsigned int v2, const WeightType &weight);
	void AddEdge(const VerticeType &vert1, const VerticeType &vert2, WeightType weight);
	inline void UpdateEdges();

	inline VerticeType &GetVerticeAt(unsigned int index) { return mVertices[index]; }
	unsigned int GetVerticeIndex(const VerticeType &vert) const;
	inline unsigned int GetNumberOfVertices() const { return mVertices.size(); }

	inline Edge<WeightType> &GetEdgeAt(unsigned int index) { return mEdges[index]; }
	inline Edge<WeightType> &GetEdge(unsigned int v1, unsigned int v2);
	inline WeightType &GetEdgeWeight(unsigned int v1, unsigned int v2);
	inline unsigned int GetNumberOfEdges() const { return mEdges.size(); }

	// Dijkstra algorithm will return true if path is found and false if it's not.
	// outPath		- found path consisting of vertices
	// d			- total weight of the found path
	bool Dijkstra(const VerticeType &source, const VerticeType &dest, std::vector<unsigned int> *outPath, WeightType *d) const;
	bool Dijkstra(const unsigned int sI, const unsigned int dI, std::vector<unsigned int> *outPath, WeightType *d) const;

private:
	// Holds the vertice object index and the currently calculated weight from the source to this vertice.
	struct VerticeWithWeightSum
	{
		unsigned int vI;
		WeightType w;
	};

	// y must be smaller than x
	inline unsigned int GetIndexForCompact(unsigned int x, unsigned int y) const
	{
		return (unsigned int)(x - y - 1 + mVertices.size() * y - y * (y + 1) * 0.5f);
	}

	// Data members
	std::vector<VerticeType> mVertices;
	std::vector<Edge<WeightType>> mEdges;
	std::vector<WeightType> mEdgesCompact;
	WeightType mMinValue, mMaxValue;
};

// **************************************************************
//						Definitions
// **************************************************************
template<class VerticeType, class WeightType>
void Graph<VerticeType, WeightType>::Clear()
{
	mVertices.clear();
	mEdges.clear();
	mEdgesCompact.clear();
}

template<class VerticeType, class WeightType>
void Graph<VerticeType, WeightType>::RemoveVerticeAt(unsigned int index)
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

template<class VerticeType, class WeightType>
void Graph<VerticeType, WeightType>::AddEdge(unsigned int v1, unsigned int v2, const WeightType &weight)
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

template<class VerticeType, class WeightType>
void Graph<VerticeType, WeightType>::AddEdge(const VerticeType &vert1, const VerticeType &vert2, WeightType weight)
{
	unsigned int v1 = GetVerticeIndex(vert1);
	unsigned int v2 = GetVerticeIndex(vert2);
	AddEdge(v1, v2, weight);
}

template<class VerticeType, class WeightType>
void Graph<VerticeType, WeightType>::UpdateEdges()
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

template<class VerticeType, class WeightType>
unsigned int Graph<VerticeType, WeightType>::GetVerticeIndex(const VerticeType &vert) const
{
	auto it = find(mVertices.begin(), mVertices.end(), vert);
	assert(it != mVertices.end());
	return it - mVertices.begin();
}

template<class VerticeType, class WeightType>
Edge<WeightType> &Graph<VerticeType, WeightType>::GetEdge(unsigned int v1, unsigned int v2)
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

template<class VerticeType, class WeightType>
inline WeightType &Graph<VerticeType, WeightType>::GetEdgeWeight(unsigned int v1, unsigned int v2)
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

template<class VerticeType, class WeightType>
bool Graph<VerticeType, WeightType>::Dijkstra(const VerticeType &source, const VerticeType &dest, std::vector<unsigned int> *outPath, WeightType *d) const
{
	unsigned int sI = GetVerticeIndex(source);
	unsigned int dI = GetVerticeIndex(dest);
	return Dijkstra(sI, dI, outPath, d);
}

template<class VerticeType, class WeightType>
bool Graph<VerticeType, WeightType>::Dijkstra(const unsigned int sI, const unsigned int dI, std::vector<unsigned int> *outPath, WeightType *d) const
{
	outPath->clear();
	*d = mMinValue;
	std::vector<int> predecessors;
	predecessors.resize(mVertices.size());
	std::vector<VerticeWithWeightSum> toBeCh;
	toBeCh.resize(mVertices.size());
	for (unsigned int i = 0; i < toBeCh.size(); ++i)
	{
		toBeCh[i].vI = i;
		toBeCh[i].w = mMaxValue;
		predecessors[i] = -1;
	}
	toBeCh[sI].w = mMinValue;

	// make heap
	auto cmp = [](const VerticeWithWeightSum &left, const VerticeWithWeightSum &right) { return (left.w) >(right.w); };
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
				unsigned int vertPrevI = vertI;
				vertI = predecessors[vertI];
			} while (vertI != sI); // until we arrive at the source
			*d = vW;
			outPath->push_back(sI); // just add source vertice
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
				toBeCh[i].w = uW_new; // 'decrease key' operation

									  // update heap after 'decrease key'
				bool check = true;
				unsigned int currentNodeI = i;
				while (check && currentNodeI != 0)
				{
					check = false;
					unsigned parentI = (currentNodeI + 1) / 2 - 1;
					unsigned int leftChildI = 2 * (parentI + 1) - 1;
					unsigned int rightChildI = 2 * (parentI + 1) + 1 - 1;
					unsigned int childToSwapWithI;

					if (leftChildI >= toBeCh.size() && rightChildI >= toBeCh.size())
						break; // node has no children
					else if (leftChildI >= toBeCh.size() && rightChildI < toBeCh.size())
						childToSwapWithI = rightChildI; // has only right child
					else if (leftChildI < toBeCh.size() && rightChildI >= toBeCh.size())
						childToSwapWithI = leftChildI; // has only left child
					else // has both children
						childToSwapWithI = (toBeCh[leftChildI].w < toBeCh[rightChildI].w) ? leftChildI : rightChildI;

					if (toBeCh[childToSwapWithI].w < toBeCh[parentI].w)
					{ // swap
						VerticeWithWeightSum temp = toBeCh[parentI];
						toBeCh[parentI] = toBeCh[childToSwapWithI];
						toBeCh[childToSwapWithI] = temp;
						currentNodeI = parentI;
						check = true;
					}
				}
			}
		}
	}

	return false;
}

#endif
