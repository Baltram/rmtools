#include "rmMap.h"

rmMap::rmMap(void)
{
	bitmapPath = "";
	name = "Map #1";
}

rmMap::rmMap(const rmMap & m)
{
	*this = m;
}

rmMap::rmMap(string name, string bitmapPath)
{
	this->name = name;
	this->bitmapPath = bitmapPath;
}

rmMap::rmMap(rmLPCChar name, rmLPCChar bitmapPath)
{
	this->name = name;
	this->bitmapPath = bitmapPath;
}

rmMap::~rmMap(void) {}

void rmMap::setBitmapPath(string path)
{
	bitmapPath = path;
}

string rmMap::getBitmapPath(void)
{
	return bitmapPath;
}

void rmMap::setName(string name)
{
	this->name = name;
}

string rmMap::getName(void)
{
	return name;
}

rmMap & rmMap::operator = (const rmMap & m)
{
	bitmapPath = m.bitmapPath;
	name = m.name;
	return *this;
}
