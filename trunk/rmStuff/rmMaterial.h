#pragma once

#include "rmDataTypes.h"
#include "rmMap.h"

class rmMaterial
{
public:
	rmMaterial(void);
	rmMaterial(string name);
	rmMaterial(const rmMaterial & m);
	virtual ~rmMaterial(void);

	rmMap * getDiffuseMap(void);
	rmMap * getSpecularMap(void);
	rmMap * getNormalMap(void);

	void setDiffuseMap(rmMap map);
	rmMap & setDiffuseMap(void);
	void setSpecularMap(rmMap map);
	rmMap & setSpecularMap(void);
	void setNormalMap(rmMap map);
	rmMap & setNormalMap(void);

	void setName(string name);
	string getName(void);

	rmMaterial & operator = (const rmMaterial & m);

	static rmBool compareByName(rmMaterial * m1, rmMaterial * m2);

private:
	string name;

	rmMap * diffuseMap;
	rmMap * specularMap;
	rmMap * normalMap;
};
