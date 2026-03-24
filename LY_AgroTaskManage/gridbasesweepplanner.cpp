#include "gridbasesweepplanner.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>


namespace UAVPlanner {

GridBaseSweepPlanner::GridBaseSweepPlanner()
{
}

void GridBaseSweepPlanner::init(const int& mDir,
                                const int& sDir,
                                const vector<int>& x_inds_goal_y,
                                int goal_y) {
    m_moving_direction = mDir;
    m_sweep_direction  = sDir;
    update_turning_window();
    m_x_inds_goal_y    = x_inds_goal_y;
    m_goal_y           = goal_y;
}

void GridBaseSweepPlanner::move_target_grid(int c_x_index, int c_y_index,
                                            int& next_x_ind, int& next_y_ind) {
    int next_x_index = m_moving_direction + c_x_index;
    int next_y_index = c_y_index;

    if (!m_grid_map->check_occupied_from_xy_index(next_x_index, next_y_index, 0.5)) {
        next_x_ind = next_x_index;
        next_y_ind = next_y_index;
        return;
    } else {
        int next_c_x_index, next_c_y_index;
        find_safe_turing_grid(c_x_index, c_y_index,
                              next_c_x_index, next_c_y_index);
        if (next_c_x_index == INVALID_INT_VAL
                && next_c_y_index == INVALID_INT_VAL) {
            next_c_x_index = -m_moving_direction + c_x_index;
            next_c_y_index = c_y_index;
            if (m_grid_map->check_occupied_from_xy_index(next_c_x_index,
                                                         next_c_y_index)) {
                next_x_ind = INVALID_INT_VAL;
                next_y_ind = INVALID_INT_VAL;
                return;
            }
        } else {
            while (!m_grid_map->check_occupied_from_xy_index(
                       next_c_x_index + m_moving_direction,
                       next_c_y_index, 0.5)) {
                next_c_x_index += m_moving_direction;
            }
            swap_moving_direction();
        }
        next_x_ind = next_c_x_index;
        next_y_ind = next_c_y_index;
    }
}

void GridBaseSweepPlanner::find_safe_turing_grid(int c_x_index, int c_y_index,
                                                 int& next_x_ind, int& next_y_ind) {
    for (int i = 0; i < (int)m_turing_window.size(); ++i) {
        next_x_ind = m_turing_window[i][0] + c_x_index;
        next_y_ind = m_turing_window[i][1] + c_y_index;
        if (!m_grid_map->check_occupied_from_xy_index(next_x_ind, next_y_ind, 0.5)) {
            return;
        }
    }
    next_x_ind = INVALID_INT_VAL;
    next_y_ind = INVALID_INT_VAL;
}

bool GridBaseSweepPlanner::is_search_done() {
    for (int i = 0; i < (int)m_x_inds_goal_y.size(); ++i) {
        if (!m_grid_map->check_occupied_from_xy_index(
                    m_x_inds_goal_y[i], m_goal_y, 0.5)) {
            return false;
        }
    }
    return true;
}

void GridBaseSweepPlanner::update_turning_window() {
    m_turing_window = vector<vector<int>>{
    {m_moving_direction, 0},
    {m_moving_direction, m_sweep_direction},
    {0, m_sweep_direction},
    {-m_moving_direction, m_sweep_direction}
    };
}

void GridBaseSweepPlanner::swap_moving_direction() {
    m_moving_direction *= -1;
    update_turning_window();
}

void GridBaseSweepPlanner::search_start_grid(int& x_ind, int& y_ind) {
    vector<int> x_inds;
    y_ind = 0;
    if (m_sweep_direction == (int)SweepDirection::DOWN) {
        search_free_grid_index_at_edge_y(x_inds, y_ind, true);
    } else if (m_sweep_direction == (int)SweepDirection::UP) {
        search_free_grid_index_at_edge_y(x_inds, y_ind, false);
    }

    if (m_moving_direction == (int)MovingDirection::RIGHT) {
        x_ind = *min_element(x_inds.begin(), x_inds.end());
    } else if (m_moving_direction == (int)MovingDirection::LEFT) {
        x_ind = *max_element(x_inds.begin(), x_inds.end());
    }
}

void GridBaseSweepPlanner::find_sweep_direction_and_start_position(const vector<double>& ox,
                                             const vector<double>& oy,
                                             vector<double>& vec,
                                             vector<double>& start_pos) {
    double max_dist = 0.0;
    vec.clear();
    start_pos.clear();
    for (int i = 0; i < (int)ox.size() - 1; ++i) {
        double dx = ox[i + 1] - ox[i];
        double dy = oy[i + 1] - oy[i];
        double d  = sqrt(dx * dx + dy * dy);

        if (d > max_dist) {
            max_dist = d;
            vec = vector<double>{dx, dy};
            start_pos = vector<double>{ox[i], oy[i]};        }
    }
}

void GridBaseSweepPlanner::convert_grid_coordinate(const vector<double>& ox,
                             const vector<double>& oy,
                             const vector<double>& vec,
                             const vector<double>& start_pos,
                             vector<double>& rox,
                             vector<double>& roy) {
    rox.clear();
    roy.clear();
    Eigen::MatrixXd origMax;
    origMax.resize(ox.size(), 2);
    for (int i = 0; i < (int)ox.size(); ++i) {
        origMax(i, 0) = (ox[i] - start_pos[0]);
        origMax(i, 1) = (oy[i] - start_pos[1]);
    }
    double th = atan2(vec[1], vec[0]);
    Eigen::AngleAxisd rotation_vector(th, Eigen::Vector3d(0,0,1));
    Eigen::Matrix3d rot3 = rotation_vector.toRotationMatrix();
    Eigen::Matrix2d rot2 = rot3.block<2, 2>(0, 0);
    Eigen::MatrixXd resMatrix = origMax * rot2;
    for (int i = 0; i < resMatrix.rows(); ++i) {
        rox.push_back(resMatrix(i, 0));
        roy.push_back(resMatrix(i, 1));
    }
}

void GridBaseSweepPlanner::convert_global_coordinate(const vector<double>& x,
                               const vector<double>& y,
                               const vector<double>& vec,
                               const vector<double>& start_pos,
                               vector<double>& rx,
                               vector<double>& ry) {
    double th = atan2(vec[1], vec[0]);
    Eigen::AngleAxisd rotation_vector(-th, Eigen::Vector3d(0, 0, 1));
    Eigen::Matrix3d rot3 = rotation_vector.toRotationMatrix();
    Eigen::Matrix2d rot2 = rot3.block<2, 2>(0, 0);

    rx.clear();
    ry.clear();
    Eigen::MatrixXd origMax;
    origMax.resize(x.size(), 2);
    for (int i = 0; i < (int)x.size(); ++i) {
        origMax(i, 0) = x[i];
        origMax(i, 1) = y[i];
    }
    Eigen::MatrixXd resMatrix = origMax * rot2;
    for (int i = 0; i < resMatrix.rows(); ++i) {
        rx.push_back(resMatrix(i, 0) + start_pos[0]);
        ry.push_back(resMatrix(i, 1) + start_pos[1]);
    }
}

void GridBaseSweepPlanner::search_free_grid_index_at_edge_y(
        vector<int>& x_indexs, int& y_index, bool  from_upper) {
    y_index = INVALID_INT_VAL;

    if (from_upper) {
        for (int iy = m_grid_map->height() - 1; iy >= 0; iy--) {
            for (int ix = m_grid_map->width() - 1; ix >= 0; ix--) {
                if (!m_grid_map->check_occupied_from_xy_index(ix, iy)) {
                    y_index = iy;
                    x_indexs.push_back(ix);
                }
            }
            if (y_index != INVALID_INT_VAL) {
                break;
            }
        }
    } else {
        for (int iy = 0; iy < m_grid_map->height(); iy++) {
            for (int ix = 0; ix < m_grid_map->width(); ix++) {
                if (!m_grid_map->check_occupied_from_xy_index(ix, iy)) {
                    y_index = iy;
                    x_indexs.push_back(ix);
                }
            }
            if (y_index != INVALID_INT_VAL) {
                break;
            }
        }
    }
}

void GridBaseSweepPlanner::setup_grid_map(vector<double>& ox,
                    vector<double>& oy,
                    std::vector<std::vector<QPointF>>foxy,
                    double resolution,
                    int sDir,
                    vector<int>& x_inds_goal_y,
                    int& goal_y, int offset_grid) {
    double max_ox = *max_element(ox.begin(), ox.end());
    double min_ox = *min_element(ox.begin(), ox.end());
    double max_oy = *max_element(oy.begin(), oy.end());
    double min_oy = *min_element(oy.begin(), oy.end());
    int width  = ceil((max_ox - min_ox) / resolution) + offset_grid;
    int height = ceil((max_oy - min_oy) / resolution) + offset_grid;
    double center_x = (max_ox + min_ox) / 2.0;
    double center_y = (max_oy - min_oy) / 2.0;
    m_grid_map = new GridMap;
    m_grid_map->init(width, height, resolution, center_x, center_y, 0.0);
    m_grid_map->set_value_from_polygon(ox, oy, 1.0, false);
    std::vector<double>fox;
    std::vector<double>foy;

    for(int i = 0; i < foxy.size();i++)
    {
		fox.clear();
		foy.clear();
        for(int j = 0; j < foxy[i].size();j++)
        {
            fox.push_back(foxy[i][j].x());
            foy.push_back(foxy[i][j].y());
        }
		if (fox.empty() || foy.empty())
		{
			continue;
		}
        m_grid_map->set_value_from_polygon(fox, foy, 1.0, true);
    }

    m_grid_map->expand_grid();

    x_inds_goal_y.clear();
    goal_y = 0;
    if (sDir == SweepDirection::UP) {
        search_free_grid_index_at_edge_y(x_inds_goal_y, goal_y, true);
    } else if (sDir == SweepDirection::DOWN) {
        search_free_grid_index_at_edge_y(x_inds_goal_y, goal_y, false);
    }
}

void GridBaseSweepPlanner::sweep_path_search(vector<double>& px, vector<double>& py) {
    int c_x_index, c_y_index;
    px.clear();
    py.clear();
    search_start_grid(c_x_index, c_y_index);
    if (!m_grid_map->set_value_from_xy_index(c_x_index, c_y_index, 0.5)) {
        return;
    }
    double x, y;
    m_grid_map->calc_grid_central_xy_position_from_xy_index(c_x_index, c_y_index, x, y);
    px.push_back(x);
    py.push_back(y);

    int next_c_x_index, next_c_y_index;
    while (1) {
        move_target_grid(c_x_index, c_y_index, next_c_x_index, next_c_y_index);
        c_x_index = next_c_x_index;
        c_y_index = next_c_y_index;
        if (is_search_done() || (next_c_x_index == INVALID_INT_VAL
                || next_c_y_index == INVALID_INT_VAL)) {
            break;
        }
        m_grid_map->calc_grid_central_xy_position_from_xy_index(next_c_x_index,
                                                                next_c_y_index,
                                                                x, y);
        px.push_back(x);
        py.push_back(y);
        m_grid_map->set_value_from_xy_index(next_c_x_index, next_c_y_index, 0.5);
    }
}

void GridBaseSweepPlanner::planning(vector<double>& ox, vector<double>& oy,
                                    std::vector<std::vector<QPointF>>foxy,
                                    double resolution,
                                    int mDir, int sDir,
                                    vector<double>& rx, vector<double>& ry) {

    if (ox[0] != ox[ox.size() - 1] || oy[0] != oy[oy.size() -1]) {
        ox.push_back(ox[0]);
        oy.push_back(oy[0]);
    }
    vector<double> vec, start_position;
    find_sweep_direction_and_start_position(ox, oy, vec, start_position);
    vector<double> rox, roy;
    convert_grid_coordinate(ox, oy, vec, start_position, rox, roy);
    vector<int> x_inds_goal_y;
    int goal_y;

	vector<double> rox1, roy1;
	std::vector<double>fox;
	std::vector<double>foy;
	std::vector<std::vector<QPointF>>fmvp;
	for (int i = 0; i < foxy.size(); i++)
	{
		fox.clear();
		foy.clear();
		rox1.clear();
		roy1.clear();
		for (int j = 0; j < foxy[i].size(); j++)
		{
			fox.push_back(foxy[i][j].x());
			foy.push_back(foxy[i][j].y());
		}
		std::vector<QPointF>ffoxy;
		convert_grid_coordinate(fox, foy, vec, start_position, rox1, roy1);
		for (int i = 0; i < rox1.size(); i++)
		{
			ffoxy.push_back(QPointF(rox1[i], roy1[i]));
		}
		fmvp.push_back(ffoxy);

	}

	setup_grid_map(rox, roy, fmvp, resolution, sDir, x_inds_goal_y, goal_y);
    init(mDir, sDir, x_inds_goal_y, goal_y);
    vector<double> px, py;
    sweep_path_search(px, py);
    convert_global_coordinate(px, py, vec, start_position, rx, ry);
}
}
