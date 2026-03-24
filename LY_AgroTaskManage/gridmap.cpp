#include "gridmap.h"
#include <iostream>
#include <cmath>

namespace UAVPlanner {

GridMap::GridMap()
{

}

void GridMap::init(int width, int height, double resolution, double center_x,
                   double center_y, double init_val) {
    m_width         = width;
    m_height        = height;
    m_resolution    = resolution;
    m_center_x      = center_x;
    m_center_y      = center_y;
    m_left_lower_x  = m_center_x - m_width / 2.0 * m_resolution;
    m_left_lower_y  = m_center_y - m_height / 2.0 * m_resolution;
    m_init_value    = init_val;
    m_size          = m_width * m_height;
    m_dataArray     = vector<double>(m_size, init_val);
}

double GridMap::get_value_from_xy_index(int x_ind, int y_ind) {
    int grid_ind = calc_grid_index_from_xy_index(x_ind, y_ind);
    if (grid_ind >= 0 && grid_ind < m_size) {
        return m_dataArray[grid_ind];
    }
    return INVALID_DOUBLE_VAL;
}

void   GridMap::get_xy_index_from_xy_pos(double x_pos, double y_pos,
                                          int& x_ind, int& y_ind) {
    x_ind = calc_xy_index_from_position(x_pos, m_left_lower_x, m_width);
    y_ind = calc_xy_index_from_position(y_pos, m_left_lower_y, m_height);
}

bool   GridMap::set_value_from_xy_pos(double x_pos, double y_pos, double val) {
    int x_ind, y_ind;
    get_xy_index_from_xy_pos(x_pos, y_pos, x_ind, y_ind);
    if (x_ind == INVALID_INT_VAL || y_ind == INVALID_INT_VAL) {
        return false;
    }
    return set_value_from_xy_index(x_ind, y_ind, val);
}

bool   GridMap::set_value_from_xy_index(int x_ind, int y_ind, double val) {
    if (x_ind == INVALID_INT_VAL || y_ind == INVALID_INT_VAL) {
        return false;
    }

    int grid_ind = (int)(y_ind * m_width + x_ind);
    if (grid_ind >= 0 && grid_ind < m_size) {
        m_dataArray[grid_ind] = val;
        return true;
    }
    return false;
}

void   GridMap::set_value_from_polygon(const vector<double>& opol_xVal,
                                       const vector<double>& opol_yVal, double val, bool bInside) {
    vector<double> pol_xVal = opol_xVal;
    vector<double> pol_yVal = opol_yVal;

    if (pol_xVal[0] != pol_xVal[pol_xVal.size() - 1] ||
            pol_yVal[0] != pol_yVal[pol_yVal.size() - 1]) {
        pol_xVal.push_back(pol_xVal[0]);
        pol_yVal.push_back(pol_yVal[0]);
    }

    for (int i = 0; i < m_width; ++i) {
        for (int j = 0; j < m_height; ++j) {
            double x_pos, y_pos;
            calc_grid_central_xy_position_from_xy_index(i, j, x_pos, y_pos);
            bool flag = check_inside_polygon(x_pos, y_pos, pol_xVal, pol_yVal);
            if (flag == bInside) {
                set_value_from_xy_index(i, j, val);
            }
        }
    }
}

int    GridMap::calc_grid_index_from_xy_index(int x_ind, int y_ind) {
    return y_ind * m_width + x_ind;
}

void GridMap::calc_grid_central_xy_position_from_xy_index(int x_ind, int y_ind,
                                                          double& x_pos, double& y_pos) {
    x_pos = calc_grid_central_xy_position_from_index(x_ind, m_left_lower_x);
    y_pos = calc_grid_central_xy_position_from_index(y_ind, m_left_lower_y);
}

double GridMap::calc_grid_central_xy_position_from_index(int index, double lower_pos) {
    return lower_pos + index * m_resolution + m_resolution / 2.0;
}

int    GridMap::calc_xy_index_from_position(double pos, double lower_pos, int max_index) {
    int ind = (int)floor((pos - lower_pos) / m_resolution);
    if (ind >= 0 && ind <= max_index) {
        return ind;
    }
    return INVALID_INT_VAL;
}

bool   GridMap::check_occupied_from_xy_index(int x_ind, int y_ind, double occupied_val) {
    double val = get_value_from_xy_index(x_ind, y_ind);
    if (val == INVALID_DOUBLE_VAL || val >= occupied_val) {
        return true;
    }
    return false;
}

void   GridMap::expand_grid() {
    vector<int> xinds, yinds;
    for (int i = 0; i < m_width; ++i) {
        for (int j = 0; j < m_height; ++j) {
            if (check_occupied_from_xy_index(i, j)) {
                 xinds.push_back(i);
                 yinds.push_back(j);
            }
        }
    }

    for (int i = 0; i < (int)xinds.size(); ++i) {
        int ix = xinds[i];
        int iy = yinds[i];
        set_value_from_xy_index(ix + 1, iy, 1.0);
        set_value_from_xy_index(ix, iy + 1, 1.0);
        set_value_from_xy_index(ix + 1, iy + 1, 1.0);
        set_value_from_xy_index(ix - 1, iy, 1.0);
        set_value_from_xy_index(ix, iy - 1, 1.0);
        set_value_from_xy_index(ix - 1, iy - 1, 1.0);
    }
}

bool   GridMap::check_inside_polygon(double iox, double ioy, const vector<double>& x,
                                     const vector<double>& y) {
    bool inside = false;
    int  nSize  = x.size() - 1;
    double min_x, max_x;
    for (int l1 = 0; l1 < nSize; ++l1) {
        int l2 = (l1 + 1);
        if (x[l1] >= x[l2]) {
            min_x = x[l2];
            max_x = x[l1];
        } else {
            min_x = x[l1];
            max_x = x[l2];
        }

        if (!(min_x < iox && iox < max_x)) {
            continue;
        }

        double tmp1 = (y[l2] - y[l1]) / (x[l2] - x[l1]);
        if (y[l1] + tmp1 * (iox - x[l1]) - ioy > 0) {
            inside = !inside;
        }
    }
    return inside;
}
}
