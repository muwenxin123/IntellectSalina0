#ifndef GRIDBASESWEEPPLANNER_H
#define GRIDBASESWEEPPLANNER_H

#include "gridmap.h"

#include <QPointF>

namespace UAVPlanner {

enum SweepDirection {
    UP   = 1,
    DOWN = -1
};

enum MovingDirection {
    RIGHT = 1,
    LEFT  = -1
};

class GridBaseSweepPlanner
{
public:
    GridBaseSweepPlanner();

    void init(const int& mDir, const int& sDir,
              const vector<int>& x_inds_goal_y, int goal_y);
    void move_target_grid(int c_x_index, int c_y_index,
                          int& next_x_ind, int& next_y_ind);
    void find_safe_turing_grid(int c_x_index, int c_y_index,
                               int& next_x_ind, int& next_y_ind);
    bool is_search_done();
    void update_turning_window();
    void swap_moving_direction();
    void search_start_grid(int& x_ind, int& y_ind);
    void find_sweep_direction_and_start_position(const vector<double>& ox,
                                                 const vector<double>& oy,
                                                 vector<double>& vec,
                                                 vector<double>& start_pos);
    void convert_grid_coordinate(const vector<double>& ox,
                                 const vector<double>& oy,
                                 const vector<double>& vec,
                                 const vector<double>& start_pos,
                                 vector<double>& rox,
                                 vector<double>& roy);
    void convert_global_coordinate(const vector<double>& x,
                                   const vector<double>& y,
                                   const vector<double>& vec,
                                   const vector<double>& start_pos,
                                   vector<double>& rx,
                                   vector<double>& ry);
    void search_free_grid_index_at_edge_y(vector<int>& x_indexs,
                                          int& y_index,
                                          bool from_upper = false);
    void setup_grid_map(vector<double>& ox,
                        vector<double>& oy,
                        std::vector<std::vector<QPointF>>foxy,
                        double resolution,
                        int sDir,
                        vector<int>& x_inds_goal_y,
                        int& goal_y, int offset_grid = 10);

    void sweep_path_search(vector<double>& px,
                           vector<double>& py);

    void planning(vector<double>& ox, vector<double>& oy,
                  std::vector<std::vector<QPointF>>foxy,
                  double resolution,
                  int mDir, int sDir,
                  vector<double>& rx,
                  vector<double>& ry);

private:
    GridMap*            m_grid_map = nullptr;
    int                 m_moving_direction;
    int                 m_sweep_direction;
    vector<vector<int>> m_turing_window;
    vector<int>         m_x_inds_goal_y;
    int                 m_goal_y;
};

}
#endif
