#ifndef _RRTSTAR_H
#define _RRTSTAR_H

#include <iostream>
#include <vector>
#include <cmath>
#include <random>

#include "vector2.h"
#include "vector3.h"

using namespace std;

class node {
public:
    double m_x, m_y;
    double m_cost;
    int  m_parent;
    node(double x, double y);
};

class RRTStar {

public:
    RRTStar(node* start, node* goal, const vector<vector<casic_vector2d>>& obstacleList,
            const pair<double, double>& areaRange, double expandStep = 0.5,
            int goalSampleRate = 20, int maxIter = 500);
	void planning(std::vector<casic_vector2d> &PointsVec);

private:
    int m_maxIter;
    int m_goalSampleRate;
    double m_expandStep;
    node* m_startNode, *m_goalNode;
    vector<vector<casic_vector2d>> m_obstacleList;
    vector<node*> m_nodeList;
    pair<double, double> m_areaRange;

private:
    void chooseParent(node* newNode, const vector<int>& nearInds);
    node* steer(const pair<double, double>& randPos, int nInd);
    pair<double, double> getRandomPoint();
    int getBestLastIndex();
    vector<pair<double, double>> getCourse(int goalInd);
    double calcDist2Goal(const node* curNode);
    vector<int> findNeighborNodes(const node* newNode);
    void reWrite(const node* newNode, vector<int>& neighborInds);
    bool checkCollisionExtend(const node* neighborNode, double theta, double d);
    int getNearestListIndex(const vector<node*>& nodeList, const pair<double, double>& rnd);
    bool collisionCheck(const node* curNode, const vector<vector<casic_vector2d>>& obstacleList);

	bool insidePolygon(const vector<casic_vector2d> &pointList, const node* curNode);

};

#endif
