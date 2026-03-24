#ifndef GRIDMAP_H
#define GRIDMAP_H

#include <vector>

using namespace std;

namespace UAVPlanner {

const double INVALID_DOUBLE_VAL = 999999;
const double INVALID_INT_VAL = 999999;

class GridMap
{
public:
    GridMap();
    void   init(int width, int height, double resolution, double center_x,
                double center_y, double init_val = 0.0);

    int    height() const {
        return m_height;
    }

    int    width() const {
        return m_width;
    }

    double get_value_from_xy_index(int x_ind, int y_ind);
    void   get_xy_index_from_xy_pos(double x_pos, double y_pos, int& x_ind, int& y_ind);
    bool   set_value_from_xy_pos(double x_pos, double y_pos, double val);
    bool   set_value_from_xy_index(int x_ind, int y_ind, double val);
    void   set_value_from_polygon(const vector<double>& opol_xVal,
                                  const vector<double>& opol_yVal, double val, bool bInside = true);
    int    calc_grid_index_from_xy_index(int x_ind, int y_ind);
    void   calc_grid_central_xy_position_from_xy_index(int x_ind, int y_ind, double& x_pos, double& y_pos);
    double calc_grid_central_xy_position_from_index(int index, double lower_pos);
    int    calc_xy_index_from_position(double pos, double lower_pos, int max_index);
    bool   check_occupied_from_xy_index(int x_ind, int y_ind, double occupied_val = 1.0);
    void   expand_grid();
    bool   check_inside_polygon(double iox, double ioy, const vector<double>& x,
                                const vector<double>& y);
private:
    int             m_width;
    int             m_height;
    double          m_init_value;
    double          m_resolution;
    double          m_center_x;
    double          m_center_y;
    double          m_left_lower_x;
    double          m_left_lower_y;
    int             m_size;
    vector<double>  m_dataArray;
};
}

#endif
