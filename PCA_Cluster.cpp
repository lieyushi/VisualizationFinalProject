#include "PCA_Cluster.h"

const float& TOR_1 = 0.999;
const int& CLUSTER = 8;

void PCA_Cluster::performPCA_Clustering(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter)
{
	MatrixXf cArray, SingVec;
	VectorXf meanTrajectory(Column);
	int PC_Number;

	performSVD(cArray, data, Row, Column, PC_Number, SingVec, meanTrajectory);
	performK_Means(cArray, Row, Column, PC_Number, SingVec, meanTrajectory, massCenter, CLUSTER);
}



void PCA_Cluster::performSVD(MatrixXf& cArray, float **data, const int& Row, const int& Column,
							 int& PC_Number, MatrixXf& SingVec, VectorXf& meanTrajectory)
{
	Eigen::MatrixXf temp(Row, Column);

#pragma omp parallel for num_threads(8)
	for (int i = 0; i < Row; ++i)
	{
		temp.row(i) = Eigen::VectorXf::Map(&data[i][0], Column); //copy each trajectory to temporary space
	}

#pragma omp parallel for num_threads(8)
	for (int i = 0; i < Column; ++i)
	{
		meanTrajectory(i) = temp.transpose().row(i).mean();
	}

#pragma omp parallel for num_threads(8)
	for (int i = 0; i < Row; ++i)
	{
		temp.row(i) = temp.row(i)-meanTrajectory.transpose();
	}

/* perform SVD decomposition for temp */
	JacobiSVD<MatrixXf> svd(temp, ComputeThinU | ComputeThinV);
	//const VectorXf& singValue = svd.singularValues();
	SingVec = svd.matrixV();

/* compute new attribute space based on principal component */
	MatrixXf coefficient = temp*SingVec;

/*  decide first r dorminant PCs with a threshold */
	const float& varianceSummation = coefficient.squaredNorm();
	float tempSum = 0.0;
	const float& threshold = TOR_1*varianceSummation;
	
	for (int i = 0; i < Column; ++i)
	{
		tempSum+=(coefficient.transpose().row(i)).squaredNorm();
		if(tempSum>threshold)
		{
			PC_Number = i;
			break;
		}
	}

	cArray = MatrixXf(Row, PC_Number);
	for (int i = 0; i < PC_Number; ++i)
	{
		cArray.transpose().row(i) = coefficient.transpose().row(i);
	}

	std::cout << "SVD completed!" << std::endl;

	SingVec.transposeInPlace();
}



void PCA_Cluster::performK_Means(const MatrixXf& cArray, const int& Row, const int& Column, const int& PC_Number, 
	                const MatrixXf& SingVec, const VectorXf& meanTrajectory,  std::vector< std::vector<float> >& massCenter, const int& Cluster)
{
/* perform K-means clustering */
	MatrixXf clusterCenter = MatrixXf::Random(Cluster, PC_Number);

	MatrixXf range(2, PC_Number);
	range.row(0) = cArray.colwise().maxCoeff();  //first row contains max
	range.row(1) = cArray.colwise().minCoeff();  //second row contains min
	VectorXf diffRange = range.row(0)-range.row(1);

	MatrixXf diagonalRange = MatrixXf::Zero(PC_Number,PC_Number);
	for (int i = 0; i < PC_Number; ++i)
	{
		diagonalRange(i,i) = diffRange(i);
	}
	clusterCenter = (clusterCenter+MatrixXf::Constant(Cluster,PC_Number,1.0))/2.0;
	for (int i = 0; i < Cluster; ++i)
	{
		clusterCenter.row(i) = clusterCenter.row(i)*diagonalRange+range.row(1);
	}

	std::cout << "Initialized center is: " << std::endl;

	float moving, tempMoving;
	int storage[CLUSTER];

	MatrixXf centerTemp;  //store provisional center coordinate

	int tag = 0, clusTemp;

	float temp;

	do
	{
		memset(storage,0,sizeof(int)*Cluster);
		centerTemp = MatrixXf::Zero(Cluster, PC_Number);
		// each iteration find the smallest to find belongings
		for (int i = 0; i < Row; ++i)
		{
			float dist = FLT_MAX;
			for (int j = 0; j < Cluster; ++j)
			{
				temp = (cArray.row(i)-clusterCenter.row(j)).norm();
				if(temp<dist)
				{
					dist = temp;
					clusTemp = j;
				}
			}
			storage[clusTemp]++;
			centerTemp.row(clusTemp)+=cArray.row(i);
		}

		moving = FLT_MIN;

		for (int i = 0; i < Cluster; ++i)
		{
			std::cout << storage[i] <<  " ";
			if(storage[i]>0)
			{
				centerTemp.row(i)/=storage[i];
				tempMoving = (centerTemp.row(i)-clusterCenter.row(i)).norm();
				clusterCenter.row(i) = centerTemp.row(i);
				if(moving<tempMoving)
					moving = tempMoving;
			}
		}
		std::cout << std::endl;

		std::cout << "K-means iteration " << ++tag << " completed, and moving is " << moving << "!" << std::endl;

	}while(moving >= 0.05 && tag <= 5);

	std::cout << "K-means with " << Cluster << " clusters  completed!" << endl;

	MatrixXf pcSing(PC_Number,Column);
	for (int i = 0; i < PC_Number; ++i)
	{
		pcSing.row(i) = SingVec.row(i);
	}

	MatrixXf massPos = clusterCenter*pcSing;

	for (int i = 0; i < Cluster; ++i)
	{
		if(storage[i]>0)
		{
			massPos.row(i) += meanTrajectory.transpose();
			std::vector<float> vecTemp;
			for (int j = 0; j < Column; ++j)
			{
				vecTemp.push_back(massPos(i,j));
			}
			massCenter.push_back(vecTemp);
		}
	}

	std::cout << "Mean position re-mapped to trajectory space completed!" << std::endl;
}


void PCA_Cluster::performAHC(const MatrixXf& cArray, const int& Row, const int& Column, const int& PC_Number, 
		      const MatrixXf& SingVec, const VectorXf& meanTrajectory, MatrixXf& clusterCenter, std::vector< std::vector<float> >& massCenter)
{
	return;
}


void PCA_Cluster::performDirectK_Means(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter)
{
	performFullK_MeansByClusters(data, Row, Column, massCenter, CLUSTER);
}


void PCA_Cluster::performPCA_Clustering(float **data, const int& Row, const int& Column, 
		                              std::vector< std::vector<float> >& massCenter, const int& Cluster)
{
	MatrixXf cArray, SingVec;
	VectorXf meanTrajectory(Column);
	int PC_Number;

	performSVD(cArray, data, Row, Column, PC_Number, SingVec, meanTrajectory);
	performK_Means(cArray, Row, Column, PC_Number, SingVec, meanTrajectory, massCenter, Cluster);
}


void PCA_Cluster::performDirectK_Means(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter,
									  const int& Cluster)
{
	performFullK_MeansByClusters(data, Row, Column, massCenter, Cluster);
}


void PCA_Cluster::performFullK_MeansByClusters(float **data, const int& Row, const int& Column, std::vector< std::vector<float> >& massCenter,
									   const int& Cluster)
{
	Eigen::MatrixXf temp(Row, Column);

#pragma omp parallel for num_threads(8)
	for (int i = 0; i < Row; ++i)
	{
		temp.row(i) = Eigen::VectorXf::Map(&data[i][0], Column); //copy each trajectory to temporary space
	}

	MatrixXf clusterCenter = MatrixXf::Random(Cluster, Column);

	MatrixXf range(2, Column);
	range.row(0) = temp.colwise().maxCoeff();
	range.row(1) = temp.colwise().minCoeff();
	VectorXf diffRange = range.row(0)-range.row(1);

	MatrixXf diagonalRange = MatrixXf::Zero(Column,Column);

#pragma omp parallel for num_threads(8)
	for (int i = 0; i < Column; ++i)
	{
		diagonalRange(i,i) = diffRange(i);
	}

	clusterCenter = (clusterCenter+MatrixXf::Constant(Cluster,Column,1.0))/2.0;

	for (int i = 0; i < Cluster; ++i)
	{
		clusterCenter.row(i) = clusterCenter.row(i)*diagonalRange+range.row(1);
	}

	float moving, tempMoving, dist, tempDist;
	int storage[Cluster];

	MatrixXf centerTemp;

	int tag = 0, clusTemp;

	do
	{

		memset(storage,0,sizeof(int)*Cluster);
		centerTemp = MatrixXf::Zero(Cluster,Column);
		// each iteration find the smallest to find belongings
		for (int i = 0; i < Row; ++i)
		{
			dist = FLT_MAX;
			for (int j = 0; j < Cluster; ++j)
			{
				tempDist = (temp.row(i)-clusterCenter.row(j)).norm();
				if(tempDist<dist)
				{
					dist = tempDist;
					clusTemp = j;
				}
			}
			storage[clusTemp]++;
			centerTemp.row(clusTemp)+=temp.row(i);
		}

		moving = FLT_MIN;
		for (int i = 0; i < Cluster; ++i)
		{
			std::cout << storage[i] <<  " ";
			if(storage[i]>0)
			{
				centerTemp.row(i)/=storage[i];
				tempMoving = (centerTemp.row(i)-clusterCenter.row(i)).norm();
				clusterCenter.row(i) = centerTemp.row(i);
				if(moving<tempMoving)
					moving = tempMoving;
			}
		}

		std::cout << std::endl;
		std::cout << "K-means iteration " << ++tag << " completed, and moving is " << moving << "!" << std::endl;

	}while(moving >= 0.05 && tag <= 40);

	for (int i = 0; i < Cluster; ++i)
	{
		if(storage[i]>0)
		{
			std::vector<float> vecTemp;
			for (int j = 0; j < Column; ++j)
			{
				vecTemp.push_back(clusterCenter(i,j));
			}
			massCenter.push_back(vecTemp);
		}
	}
}