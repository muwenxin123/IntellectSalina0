#include "RRTStar.h"
#include <algorithm>

node::node(double x, double y) : m_x(x), m_y(y),
                                 m_parent(-1), m_cost(0.0) {

}

RRTStar::RRTStar(node* start, node* goal, const vector<vector<casic_vector2d>>& obstacleList,
                 const pair<double, double>& areaRange, double expandStep,
                 int goalSampleRate, int maxIter)
    : m_startNode(start), m_goalNode(goal), m_obstacleList(obstacleList),
    m_areaRange(areaRange), m_expandStep(expandStep),
    m_maxIter(maxIter), m_goalSampleRate(goalSampleRate) {

}

void RRTStar::planning(std::vector<casic_vector2d> &PointsVec) {

    const int imageSize = 20;
    const int imageResolution = 100;

    m_nodeList.push_back(m_startNode);
    int lastIndex = -1;
    for (int i = 0; i < m_maxIter; ++i) {
        pair<double, double> rnd = getRandomPoint();
        int nInd = getNearestListIndex(m_nodeList, rnd);
        node* newNode = steer(rnd, nInd);
        if (!newNode) {
            continue;
        }
        if (collisionCheck(newNode, m_obstacleList)) {
            vector<int> nearInds = findNeighborNodes(newNode);
            chooseParent(newNode, nearInds);
            m_nodeList.push_back(newNode);
            reWrite(newNode, nearInds);
            for (auto nearind : nearInds) {
                int pIndex = m_nodeList[nearind]->m_parent;

            }
        }

        lastIndex = getBestLastIndex();
        if (m_nodeList.size() > 3 && lastIndex != -1) {
            break;
        }
    }

    if (lastIndex == -1) {
        return;
    }

    vector<pair<double, double>> res = getCourse(lastIndex);
    cout << lastIndex << " " << res.size() << endl;
    for (int i = 0; i < res.size() - 1; ++i) {

		PointsVec.push_back(new casic_vector2d(res[i].first * imageResolution, res[i].second * imageResolution));
    }

}

void RRTStar::chooseParent(node* newNode, const vector<int>& nearInds) {
    if (nearInds.empty()) {
        return;
    }

    vector<double> dVec;
    for (auto ind : nearInds) {
        double dx = newNode->m_x - m_nodeList[ind]->m_x;
        double dy = newNode->m_y - m_nodeList[ind]->m_y;
        double dist = sqrt(pow(dx, 2) + pow(dy, 2));
        double theta = atan2(dy, dx);
        if (checkCollisionExtend(m_nodeList[ind], theta, dist)) {
            dVec.push_back(m_nodeList[ind]->m_cost + dist);
        } else {
            dVec.push_back(INT_MAX);
        }
    }
    auto minCostIter = min_element(dVec.begin(), dVec.end());
    double minCost = *minCostIter;
    int minInd = nearInds[distance(dVec.begin(), minCostIter)];
    if (minCost == INT_MAX) {
        return;
    }
    newNode->m_cost = minCost;
    newNode->m_parent = minInd;
}

node* RRTStar::steer(const pair<double, double>& randPos, int nInd) {
    node* nearestNode = m_nodeList[nInd];
    double dx = randPos.first - nearestNode->m_x;
    double dy = randPos.second - nearestNode->m_y;
    double theta = atan2(dy, dx);
    node* newNode = new node(randPos.first, randPos.second);
    double curDist = sqrt(pow(dx, 2) + pow(dy, 2));
    if (curDist <= m_expandStep) {
        return nullptr;
    } else {
        newNode->m_x = nearestNode->m_x + m_expandStep * cos(theta);
        newNode->m_y = nearestNode->m_y + m_expandStep * sin(theta);
    }
    newNode->m_cost = INT_MAX;
    newNode->m_parent = -1;
    return newNode;
}

pair<double, double> RRTStar::getRandomPoint() {
    if ((rand() % 100) > m_goalSampleRate) {

        double first = double(m_areaRange.first) + (double)(rand() % (int)((m_areaRange.second - m_areaRange.first + 1) * 1000)) / 1000.0;
        double second = double(m_areaRange.first) + (double)(rand() % (int)((m_areaRange.second - m_areaRange.first + 1) * 1000)) / 1000.0;
        return make_pair(first, second);
    } else {
        return make_pair(m_goalNode->m_x, m_goalNode->m_y);
    }
}

int RRTStar::getBestLastIndex() {
    int goalInd = -1;
    vector<int> goalInds;
    double minDist = m_expandStep * m_expandStep;
    for (int i = 0; i < m_nodeList.size(); ++i) {
        double dist2goal = calcDist2Goal(m_nodeList[i]);
        if (dist2goal <= minDist) {
            goalInds.push_back(i);
        }
    }
    if (goalInds.empty()) {
        return -1;
    }
    cout << goalInds.size() << endl;
    double minCost = INT_MAX;
    for (int i = 0; i < goalInds.size(); ++i) {
        if (m_nodeList[i]->m_cost < minCost) {
            minCost = m_nodeList[i]->m_cost;
            goalInd = goalInds[i];
        }
    }
    return goalInd;
}

vector<pair<double, double>> RRTStar::getCourse(int goalInd) {
    vector<pair<double, double>> res{{m_goalNode->m_x, m_goalNode->m_y}};
    while (m_nodeList[goalInd]->m_parent != -1) {
        res.emplace_back(m_nodeList[goalInd]->m_x, m_nodeList[goalInd]->m_y);
        goalInd = m_nodeList[goalInd]->m_parent;
    }
    res.emplace_back(m_startNode->m_x, m_startNode->m_y);
    return res;
}

double RRTStar::calcDist2Goal(const node* curNode) {
    double dx = curNode->m_x - m_goalNode->m_x;
    double dy = curNode->m_y - m_goalNode->m_y;
    return dx * dx + dy * dy;
}

vector<int> RRTStar::findNeighborNodes(const node* newNode) {
    vector<int> res;
    int nodeNum = m_nodeList.size();
    if (nodeNum == 0) {
        return res;
    }
    double r = 50.0 * sqrt(log(nodeNum) / nodeNum);
    for (int i = 0; i < nodeNum; ++i) {
        double dist = pow(m_nodeList[i]->m_x - newNode->m_x, 2) +
                         pow(m_nodeList[i]->m_y - newNode->m_y, 2);
        if (dist <= r * r) {
            res.push_back(i);
        }
    }
    return res;
}

void RRTStar::reWrite(const node* newNode, vector<int>& neighborInds) {
    int nodeNum = m_nodeList.size();
    for (int i = 0; i < neighborInds.size(); ++i) {
        node* nearNode = m_nodeList[i];
        double dx = newNode->m_x - nearNode->m_x;
        double dy = newNode->m_y - nearNode->m_y;
        double dist = sqrt(pow(dx, 2) + pow(dy, 2));
        double scost = newNode->m_cost + dist;
        if (nearNode->m_cost > scost) {
            double theta = atan2(dy, dx);
            if (checkCollisionExtend(nearNode, theta, dist)) {
                nearNode->m_parent = nodeNum - 1;
                nearNode->m_cost = scost;
            }
        }
    }
}

bool RRTStar::checkCollisionExtend(const node* neighborNode, double theta, double d) {
    node* curNode = new node(neighborNode->m_x, neighborNode->m_y);
    for (int i = 0; i < int(d / m_expandStep); ++i) {
        curNode->m_x += m_expandStep * cos(theta);
        curNode->m_y += m_expandStep * sin(theta);
        if (!collisionCheck(curNode, m_obstacleList)) {
            return false;
        }
    }
    return true;
}

int RRTStar::getNearestListIndex(const vector<node*>& nodeList,
                                 const pair<double, double>& rnd) {
    int ind = -1;
    double minDist = INT_MAX;
    for (int i = 0; i < m_nodeList.size(); ++i) {
        double curDist = pow(m_nodeList[i]->m_x - rnd.first, 2) + pow(m_nodeList[i]->m_y - rnd.second, 2);
        if (curDist < minDist) {
            minDist = curDist;
            ind = i;
        }
    }
    return ind;
}

bool RRTStar::collisionCheck(const node* curNode, const vector<vector<casic_vector2d>>& obstacleList)
{
	int index = 0;
    for (auto obstacle : obstacleList) {
		if (index == 0)
		{
			for (auto terr : obstacle)
			{
				if (curNode->m_x == terr.x && curNode->m_y == terr.y)
				{
					return false;
				}
			}
		}
		index++;
		if (insidePolygon(obstacle, curNode) == false)
		{
			return false;
		}
    }
    return true;
}

bool RRTStar::insidePolygon(const vector<casic_vector2d> &pointList, const node * curNode)
{
	double cost = 1.0;
	vector<casic_vector2d> newPointlist = pointList;
	if (newPointlist.empty())
	{
		return false;
	}
	newPointlist.push_back(newPointlist[0]);
	for (int i = 0; i < newPointlist.size() - 1; i++)
	{
		casic_vector2d vector1 = casic_vector2d(newPointlist[i].x - curNode->m_x, newPointlist[i].y - curNode->m_y);
		casic_vector2d vector2 = casic_vector2d(newPointlist[i+1].x - curNode->m_x, newPointlist[i+1].y - curNode->m_y);
		double plusOper = vector1.x * vector2.y - vector2.x * vector1.y;
		if (i == 0)
		{
			cost = plusOper;
			continue;
		}
		if (plusOper * cost > 0)
		{
			cost = plusOper;
		}
		else
		{
			return false;
		}

	}
	return true;
}

