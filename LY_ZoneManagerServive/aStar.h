#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <memory>
#include <queue>
#include <QPointF>

#include "vector3.h"
#include "vector2.h"

using std::vector;
using std::shared_ptr;

class AStar {

public:

	class PlanNode {
	public:
		PlanNode();
		PlanNode(double _x, double _y, double _xindex, double yindex, double _gCost = 0, double _hCost = 0);
		~PlanNode();

		void setObstacle();

		bool isObstacle();

		bool operator ==(const PlanNode& rhs);
		PlanNode& operator +=(const PlanNode& rhs);
		PlanNode& operator =(const PlanNode& rhs);

		double coordinateX() const;

		double coordinateY() const;

		void setBackpoint(std::shared_ptr<PlanNode> rhs);

		std::shared_ptr<PlanNode> getBackpoint();

		void setHCost(double _hCost);

		double getHCost() const;

		void setG(double _gCost);

		double getGCost() const;

		void setTotalCost();

		double getTotalCost() const;

		bool isVisited();

		void setVisited();

		double indexX() const;

		double indexY() const;

		void setindexX(double index);

		void setindexY(double index);

	private:
		double	m_x, m_y;
		double  m_totalCost;
		double	m_gCost;
		double	m_hCost;
		bool	m_obstacle;
		bool	m_isVisited;
		double  m_indexX,m_indexY;
		std::shared_ptr<PlanNode>	m_backpoint;
	};

public:
	AStar();
	~AStar();

	void init(int mapSize, int gridSize, double Height,double safedis,
		const casic_vector3d& start, const casic_vector3d& goal);

	void pathPlan(std::vector<casic_vector3d> &PointsVec);

	int getMapSize() const {
		return m_mapSize;
	}

	int getGridSize() const {
		return m_gridSize;
	}

	void setFlyParam(double straightSpeed, double angularSpeed) {
		straight_speed = straightSpeed;
		angular_speed = angularSpeed;
	}

	double getFlyTime() const {
		return fly_time;
	}

	bool getGoalisFound() const {
		return is_found;
	}

private:

	void defineObstacle(double Height);

	double calDist(const PlanNode& n1, const PlanNode& n2) {
		return (double)sqrt(pow(n1.coordinateX() - n2.coordinateX(), 2)
			+ pow(n1.coordinateY() - n2.coordinateY(), 2));
	}

	bool verifyNode(const PlanNode& rhs) {
		return rhs.indexX() >= 0 && rhs.indexX() < m_mapSize
			&& rhs.indexY() >= 0 && rhs.indexY() < m_mapSize;
	}

	bool is_point_in_zone(const QPointF point, const std::vector<QPointF>ZonePoints);

private:
	double   m_mapSize;
	double   m_gridSize;
	double   m_Height;
	double   m_safeDis = 5;
	double fly_time = 0;
	double straight_speed = 0;
	double angular_speed = 0;

	bool is_found = false;

	int      m_lonPointNum;
	int      m_latPointNum;

	PlanNode m_startNode;
	PlanNode m_goalNode;
	vector<vector<shared_ptr<PlanNode>>> m_roadmap;

	vector<shared_ptr<PlanNode>> m_countvec;
	std::shared_ptr<PlanNode>	m_point;
public:
	std::vector<casic_vector3d>ObstacleUAVs;
	std::vector<std::vector<QPointF>> NoFlyZonePoints;
	std::vector<std::vector<std::vector<double>>> GridMap;

};

#endif