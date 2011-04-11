#pragma once

#include "rmNode.h"
#include "rmMaterial.h"
#include "rmMesh.h"
#include <vector>
#include <algorithm>

class rmScene
{
public:
        rmScene(void);
	rmScene(string name);
	rmScene(const rmScene & s);
	virtual ~rmScene(void);

	void addNode(rmNode * node);
	rmNode & addNode(void);
	rmNode & getNode(rmU32 num) const;

	void addMat(rmMaterial * mat);
	rmMaterial & addMat(void);
	rmMaterial & getMat(rmU32 num) const;

        void createMaterials(list<string> materialNames);

	rmU32 getNumNodes(void) const;
	rmU32 getNumMaterials(void) const;
	
	string getName(void) const;
	void setName(string name);

	void sortMaterialsByName(void);

private:
	string name;

	vector<rmNode> nodes;
	vector<rmMaterial> materials;

	static rmOut out;
};
