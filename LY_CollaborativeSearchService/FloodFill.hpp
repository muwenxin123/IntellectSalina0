#ifndef FLOODFILL_HPP
#define FLOODFILL_HPP

#include <vector>

class Floodfill {
public:
	Floodfill(const std::vector<std::vector<int>>& grid) {
        grid_ = grid;
        row_ = grid_.size();
        if (row_ > 0) {
            col_ = grid_[0].size();
        }

        visited_ =
            std::vector<std::vector<int>>(row_, std::vector<int>(col_, 0));
    }

    int getRegionNum() {
        int regionNo = 0;
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < col_; j++) {
                if (grid_[i][j] == 1 && visited_[i][j] == 0){
                    regionNo++;
                    visited_[i][j] = regionNo;
                    dfs(i, j, regionNo);
                }
            }
        }
        return regionNo;
    }

    std::vector<std::vector<int>> visited() const {
        return visited_;
    }

private:
    void dfs(int x, int y, int regionNo){
        visited_[x][y] = regionNo;
        for (int i = 0; i < 8;i++){
            int newx = x + d[i][0];
            int newy = y + d[i][1];
            if (inArry(newx, newy) && visited_[newx][newy] == 0
                    && grid_[newx][newy] == 1) {
                dfs(newx, newy, regionNo);
            }
        }
        return;
    }

    bool inArry(int x, int y){
        return x >= 0 && y >= 0 && x < row_ && y < col_;
    }

private:
    std::vector<std::vector<int>> visited_;
    std::vector<std::vector<int>> grid_;

    const int d[8][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0},
                         {1, -1}, {1, 1}, {-1, 1}, {-1, -1}
                        };
    int row_ = 0;
    int col_ = 0;;
};

#endif
