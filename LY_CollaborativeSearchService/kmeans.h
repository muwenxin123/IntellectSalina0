#ifndef KMEANS_H
#define KMEANS_H

#include <iostream>
#include <vector>

using std::vector;

class KMeans {
public:
    enum InitMode {
        INIT_RANDOM = 0,
        INIT_MANUAL,
        INIT_UNIFORM
    };

    KMeans(vector<int> dimVec,int clusterNum = 1, int dimNum = 1);
    ~KMeans();

    void cluster(const vector<vector<double>>& sampleData, vector<vector<double>>& selectData, vector<int>& label);
    friend std::ostream& operator<<(std::ostream& out, const KMeans& kmeans);
	void setInitMode(InitMode initMode) {
		initMode_ = initMode;
	}

private:
    void init(const vector<vector<double>>& data,const vector<vector<double>>&selectData);

    void setMean(int i, const vector<double>& u);

    void setMaxIterNum(int i) {
        maxIterNum_ = i;
    }

    void setEndError(double f) {
        endError_ = f;
    }

    void resetDim(int dim);
    void resetClusterNum(int clusterNum);

    vector<double> mean(int i) const {
        return means_[i];
    }

    int initMode() const	{
        return initMode_;
    }

    int maxIterNum() const {
        return maxIterNum_;
    }

    double endError() const {
        return endError_;
    }

    double getLabel(const vector<double>& x, int& label);
	double GetLabel(const vector<double>& sample, int& label);

    double calDistance(const vector<double>& x,
                        const vector<double>& u, int dimNum);

	double calcDistance(const vector<double>& x,
		const vector<double>& u, vector<int> dimType);

private:
    int                     clusterNum_;
    int                     dimNum_;
    InitMode                initMode_;
    int                     maxIterNum_;
    double                  endError_;
    vector<vector<double>>  means_;

	vector<int> dimVec_;
};

#endif
