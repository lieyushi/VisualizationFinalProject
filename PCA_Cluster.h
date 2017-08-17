#ifndef _PCA_CLUSTER_H
#define _PCA_CLUSTER_H

#include <eigen3/Eigen/Dense>
#include <climits>
#include <float.h>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
using namespace Eigen;

class PCA_Cluster
{
public: 

	static void performPCA_Clustering(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter);

	static void performPCA_Clustering(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter, 
									  const int& Cluster);

	static void performDirectK_Means(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter);

	static void performDirectK_Means(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter,
									  const int& Cluster);

private:
	static void performSVD(MatrixXf& cArray, float **data, const int& Row, const int& Column, int& PC_Number, MatrixXf& SingVec,
	                       VectorXf& meanTrajectory);

	static void performK_Means(const MatrixXf& cArray, const int& Row, const int& Column, const int& PC_Number, const MatrixXf& SingVec, 
						   const VectorXf& meanTrajectory, std::vector< std::vector<float> >& massCenter, const int& Cluster);

	static void performAHC(const MatrixXf& cArray, const int& Row, const int& Column, const int& PC_Number, const MatrixXf& SingVec, 
		                   const VectorXf& meanTrajectory, MatrixXf& clusterCenter, std::vector< std::vector<float> >& massCenter);

	static void performFullK_MeansByClusters(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter,
									   const int& Cluster);
};

#endif

