#ifndef _FILELOADER_H
#define _FILELOADER_H

#include "Data.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <stdio.h>
#include <string.h>

class FileLoader
{
public:
	vector<Data> dataList;
	float limit[7][2];
	int number;

	FileLoader();
	~FileLoader();
	void readFile(const string& fileName);
	void readFileToMatrix(const string& fileName, float **data, const int& frame);
};

#endif