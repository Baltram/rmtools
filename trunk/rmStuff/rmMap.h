#pragma once

#include <string>
#include "rmDataTypes.h"
using namespace std;

class rmMap
{
public:
	rmMap(void);
	rmMap(const rmMap & m);
	rmMap(string name, string bitmapPath);
	rmMap(rmLPCChar name, rmLPCChar bitmapPath);
	virtual ~rmMap(void);

	void setBitmapPath(string path);
	string getBitmapPath(void);

	void setName(string name);
	string getName(void);

	rmMap & operator = (const rmMap & m);

private:
	string bitmapPath;
	string name;
};
