#include "rmMaterial.h"

rmMaterial::rmMaterial(void)
{
	name = "Material #1";
	diffuseMap = 0;
	specularMap = 0;
	normalMap = 0;
}

rmMaterial::rmMaterial(string name)
{
	this->name = name;
	diffuseMap = 0;
	specularMap = 0;
	normalMap = 0;
}

rmMaterial::rmMaterial(const rmMaterial & m)
{
	diffuseMap = 0;
	specularMap = 0;
	normalMap = 0;
	*this = m;	
}

rmMaterial::~rmMaterial(void)
{
	delete [] diffuseMap;
	delete [] specularMap;
	delete [] normalMap;
}

rmMap * rmMaterial::getDiffuseMap(void)
{
	return diffuseMap;
}

rmMap * rmMaterial::getSpecularMap(void)
{
	return specularMap;
}

rmMap * rmMaterial::getNormalMap(void)
{
	return normalMap;
}

void rmMaterial::setDiffuseMap(rmMap map)
{
	delete [] diffuseMap;
	diffuseMap = new rmMap(map);
}

rmMap & rmMaterial::setDiffuseMap(void)
{
	delete [] diffuseMap;
	diffuseMap = new rmMap();
	return *diffuseMap;
}

void rmMaterial::setSpecularMap(rmMap map)
{
	delete [] specularMap;
	specularMap = new rmMap(map);
}

rmMap & rmMaterial::setSpecularMap(void)
{
	delete [] specularMap;
	specularMap = new rmMap();
	return *specularMap;
}

void rmMaterial::setNormalMap(rmMap map)
{
	delete [] normalMap;
	normalMap = new rmMap(map);
}

rmMap & rmMaterial::setNormalMap(void)
{
	delete [] normalMap;
	normalMap = new rmMap();
	return *normalMap;
}

void rmMaterial::setName(string name)
{
	this->name = name;
}

string rmMaterial::getName(void)
{
	return name;
}

rmMaterial & rmMaterial::operator = (const rmMaterial & m)
{
	name = m.name;

	delete [] diffuseMap;
	delete [] specularMap;
	delete [] normalMap;

	if (m.diffuseMap != 0) {
		diffuseMap = new rmMap(*m.diffuseMap);
	}
	if (m.specularMap != 0) {
		specularMap = new rmMap(*m.specularMap);
	}
	if (m.normalMap != 0) {
		normalMap = new rmMap(*m.normalMap);
	}

	return *this;
}

rmBool rmMaterial::compareByName(rmMaterial * m1, rmMaterial * m2)
{
	return (m1->name.compare(m2->name) < 0);
}
