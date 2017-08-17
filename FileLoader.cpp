#include "FileLoader.h"

FileLoader::FileLoader()
{
	dataList = vector<Data>();
	for (int i = 0; i < 7; ++i)
	{
		limit[i][0] = INT_MAX;
		limit[i][1] = INT_MIN;
	}
	number = 0;
}
	
FileLoader::~FileLoader()
{
	dataList.clear();
	memset(limit, 0, 14*sizeof(int));
}	
	
void FileLoader::readFile(const string& fileName)
{
	if(dataList.size()!=0)
	{
		dataList.clear();
	}
	for (int i = 0; i < 7; ++i)
	{
		limit[i][0] = INT_MAX;
		limit[i][1] = INT_MIN;
	}
	number = 0;

	ifstream fin(fileName.c_str(), ios::in);
	if(!fin)
	{
		cout << "Error opening file in current directory!" << endl;
		exit(-1);
	}
	string line;
	getline(fin, line);

	while(getline(fin, line))
	{
		Data data(line);
		dataList.push_back(line);

		if(limit[0][0]>data.velo_norm)
			limit[0][0]=data.velo_norm;
		if(limit[0][1]<data.velo_norm)
			limit[0][1]=data.velo_norm;

		if(limit[1][0]>data.density)
			limit[1][0]=data.density;
		if(limit[1][1]<data.density)
			limit[1][1]=data.density;

		if(limit[2][0]>data.neighbor)
			limit[2][0]=data.neighbor;
		if(limit[2][1]<data.neighbor)
			limit[2][1]=data.neighbor;

		if(limit[3][0]>data.dv_norm)
			limit[3][0]=data.dv_norm;
		if(limit[3][1]<data.dv_norm)
			limit[3][1]=data.dv_norm;

		if(limit[4][0]>data.den_norm)
			limit[4][0]=data.den_norm;
		if(limit[4][1]<data.den_norm)
			limit[4][1]=data.den_norm;

		if(limit[5][0]>data.ftle)
			limit[5][0]=data.ftle;
		if(limit[5][1]<data.ftle)
			limit[5][1]=data.ftle;

		if(limit[6][0]>data.rotation)
			limit[6][0]=data.rotation;
		if(limit[6][1]<data.rotation)
			limit[6][1]=data.rotation;

	}

	number = dataList.size();
	fin.close();
}
