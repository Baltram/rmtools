#pragma once

#include "rmDataTypes.h"
#include "rmMesh.h"
#include "rmSkin.h"
#include "rmAnimation.h"

class rmNode
{
public:
	rmNode(void);
	rmNode(const rmNode & n);
	rmNode(string name);
	virtual ~rmNode(void);

	rmPoint3 getPos(void);
	void setPos(rmPoint3 pos);

	rmMesh * getMesh(void);
	void setMesh(rmMesh & mesh);
	rmMesh & setMesh(void);

	string getName(void);
	void setName(string name);

	rmNode & operator = (const rmNode & n);

private:
	string name;
	rmPoint3 position;
	rmMesh * mesh;
	static rmU32 nNodes;
	static rmOut out;
};
