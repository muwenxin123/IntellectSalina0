#include "kmeans.h"
#include <cmath>
#include <ctime>

using namespace std;

KMeans::KMeans(vector<int> dimVec,int clusterNum, int dimNum)
    : clusterNum_(clusterNum), dimNum_(dimNum), dimVec_(dimVec){
    initMode_	= INIT_RANDOM;
    maxIterNum_ = 1000;
    endError_   = 0.001;
    means_      = vector<vector<double>>(
                    clusterNum_, vector<double>(dimNum_, 0));
}

KMeans::~KMeans() {
}

void KMeans::setMean(int rowIndex, const vector<double>& u) {
    means_[rowIndex] = u;
}

void KMeans::resetDim(int dim) {
    dimNum_ = dim;
    for (int i = 0; i < clusterNum_; ++i) {
        means_[i].resize(dimNum_);
    }
}

void KMeans::resetClusterNum(int clusterNum) {
    clusterNum_ = clusterNum;
    means_.resize(clusterNum_);
}

void KMeans::cluster(const vector<vector<double>>& sampleData, vector<vector<double>>& selectData,
                     vector<int>& label) {
    int sampleNums = sampleData.size();
    init(sampleData, selectData);
    vector<double> x(dimNum_);
    int labelIndex = -1;
    double iterNum = 0;
    double lastCost = 0;
    double currCost = 0;
    int unchanged = 0;
    bool loop = true;
    vector<int> counts(clusterNum_);
    vector<vector<double>> next_means(clusterNum_, vector<double>(dimNum_));

    while (loop) {

        counts.clear();
        next_means.clear();
        counts.resize(clusterNum_);
        next_means.resize(clusterNum_);
        for (int i = 0; i < clusterNum_; ++i) {
            next_means[i].resize(dimNum_);
        }
        lastCost = currCost;
        currCost = 0;

        for (int i = 0; i < sampleNums; i++) {
            for (int j = 0; j < dimNum_; j++) {
                x[j] = sampleData[i][j];
            }

			currCost += GetLabel(x, labelIndex);
            counts[labelIndex]++;
            for (int d = 0; d < dimNum_; d++) {
                next_means[labelIndex][d] += x[d];
            }
        }
        currCost /= sampleNums;

        for (int i = 0; i < clusterNum_; i++) {
            if (counts[i] > 0) {
                for (int d = 0; d < dimNum_; d++) {
                    next_means[i][d] /= counts[i];
                }
                means_[i] = next_means[i];
            }
        }

        iterNum++;
        if (fabs(lastCost - currCost) < endError_ * lastCost) {
            unchanged++;
        }
        if (iterNum >= maxIterNum_ || unchanged >= 3) {
            loop = false;
        }

    }

    for (int i = 0; i < sampleNums; i++) {
        for (int j = 0; j < dimNum_; j++) {
            x[j] = sampleData[i][j];
        }

		GetLabel(x, labelIndex);
        label.push_back(labelIndex);
    }
}

void KMeans::init(const vector<vector<double>>& data, const vector<vector<double>>&selectData) {
    srand((unsigned)time(nullptr));
    int sampleNums = data.size();
    if (initMode_ == INIT_RANDOM) {
        int interval = sampleNums / clusterNum_;
        for (int i = 0; i < clusterNum_; i++) {
            int select = interval * i + (interval - 1) * rand() / RAND_MAX;
            means_[i] = data[select];
        }
    }
    else if (initMode_ == INIT_UNIFORM) {
        for (int i = 0; i < clusterNum_; i++) {
            int select = i * sampleNums / clusterNum_;
            means_[i] = data[select];
        }
    }
    else if (initMode_ == INIT_MANUAL) {

		if (!selectData.empty())
		{
			for (int i = 0; i < clusterNum_; i++)
			{
				means_[i] = selectData[i];;
			}
		}
    }
}

double KMeans::getLabel(const vector<double>& sample, int& label) {
    double dist = -1;
    for (int i = 0; i < clusterNum_; i++) {
        double curDist = calDistance(sample, means_[i], dimNum_);
        if (curDist < dist || dist == -1) {
            dist = curDist;
            label = i;
        }
    }
    return dist;
}

double KMeans::GetLabel(const vector<double>& sample, int & label)
{
	double dist = -1;
	for (int i = 0; i < clusterNum_; i++) {
		double curDist = calcDistance(sample, means_[i], dimVec_);
		if (curDist < dist || dist == -1) {
			dist = curDist;
			label = i;
		}
	}
	return dist;
}

double KMeans::calDistance(const vector<double>& x,
                           const vector<double>& u, int dimNum) {
    double dist = 0;
    for (int i = 0; i < dimNum; i++) {
        dist += (x[i] - u[i]) * (x[i] - u[i]);
    }
    return sqrt(dist);
}

double KMeans::calcDistance(const vector<double>& x, const vector<double>& u, vector<int> dimType)
{
	double dist = 0;
	for (int i = 0; i < dimType.size(); i++) {
		if (x.size() < dimType[i] + 1 || u.size() < dimType[i] + 1)
		{
			continue;
		}

		dist += (x[dimType[i]] - u[dimType[i]]) * (x[dimType[i]] - u[dimType[i]]);
	}
	return sqrt(dist);
}

ostream& operator<<(ostream& out, const KMeans& kmeans) {
    out << "<KMeans>" << endl;
    out << "<DimNum> " << kmeans.dimNum_ << " </DimNum>" << endl;
    out << "<ClusterNum> " << kmeans.clusterNum_ << " </CluterNum>" << endl;
    out << "<Mean>" << endl;
    for (int i = 0; i < kmeans.clusterNum_; i++) {
        for (int d = 0; d < kmeans.dimNum_; d++) {
            out << kmeans.means_[i][d] << " ";
        }
        out << endl;
    }
    out << "</Mean>" << endl;
    out << "</KMeans>" << endl;
    return out;
}
