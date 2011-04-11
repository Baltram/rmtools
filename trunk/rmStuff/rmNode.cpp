#include "rmNode.h"

rmOut rmNode::out = rmOut("rmNode.cpp");
rmU32 rmNode::nNodes = 1;

rmNode::rmNode(void)
{
	mesh = 0;
	position = rmPoint3(0, 0, 0);
	name = "Node";
	rmChar buffer[20];
	_itoa_s(nNodes++, buffer, 20, 10);
	name.append(buffer);
}

rmNode::rmNode(const rmNode & n)
{
	mesh = 0;
	*this = n;
}

rmNode::rmNode(string name)
{
	mesh = 0;
	position = rmPoint3(0, 0, 0);
	this->name = name;
}

rmNode::~rmNode(void)
{
	delete mesh;
}

rmPoint3 rmNode::getPos(void)
{
	return position;
}

void rmNode::setPos(rmPoint3 pos)
{
	position = pos;
}

rmMesh * rmNode::getMesh(void)
{
	return mesh;
}

void rmNode::setMesh(rmMesh & mesh)
{
	delete this->mesh;
	this->mesh = new rmMesh(mesh);
}

rmMesh & rmNode::setMesh(void)
{
	delete this->mesh;
	this->mesh = new rmMesh();
	return *this->mesh;
}

string rmNode::getName(void)
{
	return name;
}

void rmNode::setName(string name)
{
	this->name = name;
}

rmNode & rmNode::operator = (const rmNode & n)
{
	name = n.name;
	position = n.position;
	delete mesh;
	mesh = 0;
	if (n.mesh != 0) {
		mesh = new rmMesh(*n.mesh);
	}
	return *this;
}
