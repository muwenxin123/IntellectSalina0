#ifndef _DIJKSTRA__
#define _DIJKSTRA__

#include <vector>

#define MAX_VALUE 1000

class MGraph{

private:
	std::vector<std::vector<int>> edges;
	int iVertexCount, iEdageCount;

public:
	MGraph() {}
	~MGraph() {}
	void setMGraph(std::vector<std::vector<int>> graph);
	void Dijkstra(std::vector<int>& pArrdis, std::vector<int>& pArrPath, int iBegin);
	std::vector<int> getResult(std::vector<int>& pArrPath, int iBegin, int iEnd);
	std::vector<int> getShortPath(std::vector<std::vector<int>> graph, int iBegin, int iEnd);
};

std::vector<int> MGraph::getShortPath(std::vector<std::vector<int>> graph, int iBegin, int iEnd)
{
	std::vector<int> pArrDis;
	pArrDis.resize(MAX_VALUE);
	std::vector<int> pArrPaht;
	pArrPaht.resize(MAX_VALUE);

	setMGraph(graph);
	Dijkstra(pArrDis, pArrPaht, iBegin);
	std::vector<int> stackVertices;
	stackVertices = getResult(pArrPaht, iBegin, iEnd);
	return stackVertices;
}

void MGraph::setMGraph(std::vector<std::vector<int>> graph)
{
	edges = graph;
	iVertexCount = graph.size();
	iEdageCount = graph.size();
}

void MGraph::Dijkstra(std::vector<int>& pArrdis, std::vector<int>& pArrPath, int iBegin)
{
	int iMin;
	bool bArrVisited[MAX_VALUE];
	memset(bArrVisited, false, sizeof(bArrVisited));
	for (int i = 0; i < iVertexCount; i++)
	{
		pArrPath[i] = -1;
		edges[i][i] = 0;
		pArrdis[i] = edges[iBegin][i] != -1 ? edges[iBegin][i] : INT_MAX;
	}
	int iNewCost;
	int iSelected = iBegin;

	for (int i = 0; i < iVertexCount; i++)
	{
		int iPre = iSelected;
		iMin = INT_MAX;
		for (int j = 0; j < iVertexCount; j++)
		{
			if (!bArrVisited[j] && pArrdis[j] < iMin)
			{
				iMin = pArrdis[j];
				iSelected = j;
			}
		}
		for (int j = 0; j < iVertexCount; j++)
		{
			iNewCost = pArrdis[iSelected] != -1 && edges[iSelected][j] != -1 ? pArrdis[iSelected] + edges[iSelected][j] : INT_MAX;
			if (!bArrVisited[j] && iNewCost < pArrdis[j])
			{
				pArrPath[j] = iSelected;
				pArrdis[j] = iNewCost;
			}
		}
		bArrVisited[iSelected] = true;
	}

}

std::vector<int> MGraph::getResult(std::vector<int>& pArrPath, int iBegin, int iEnd)
{
	std::vector<int> stackVertices;
	int k = iEnd;
	while (k != -1 && k != pArrPath[k])
	{
		k = pArrPath[k];
		stackVertices.push_back(k);
	}
	stackVertices.pop_back();
	return stackVertices;
}

#endif
