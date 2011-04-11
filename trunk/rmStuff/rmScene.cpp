#include "rmScene.h"

rmOut rmScene::out = rmOut("rmScene.cpp");

rmScene::rmScene(void)
{
	name = "Scene #1";
}

rmScene::rmScene(string name)
{
	this->name = name;
}

rmScene::rmScene(const rmScene & s)
{
	this->name = s.name;
	nodes = vector<rmNode>(s.nodes);
	materials = vector<rmMaterial>(s.materials);
}

rmScene::~rmScene(void)
{
}

void rmScene::addNode(rmNode * node)
{
	nodes.push_back(*node);
}

rmNode & rmScene::addNode(void)
{
	nodes.push_back(rmNode());
	return *nodes.rbegin();
}

rmNode & rmScene::getNode(rmU32 num) const
{
	return const_cast<rmNode &>(nodes[num]);
}

void rmScene::addMat(rmMaterial * mat)
{
	materials.push_back(*mat);
}

rmMaterial & rmScene::addMat(void)
{
	materials.push_back(rmMaterial());
	return *materials.rbegin();
}

rmMaterial & rmScene::getMat(rmU32 num) const
{
	return const_cast<rmMaterial &>( materials[num]);
}

rmU32 rmScene::getNumNodes(void) const
{
	return nodes.size();
}

rmU32 rmScene::getNumMaterials(void) const
{
	return materials.size();
}

string rmScene::getName(void) const
{
	return name;
}

void rmScene::setName(string name)
{
	this->name = name;
}

void rmScene::sortMaterialsByName(void)
{
	rmU32 nMaterials = materials.size();

	vector<rmMaterial *> materialPointers;
	materialPointers.reserve(nMaterials);
	for (rmU32 i = 0; i != nMaterials; ++i) {
		materialPointers.push_back(&materials[i]);
	}

	sort(materialPointers.begin(), materialPointers.end(), rmMaterial::compareByName);
	
	vector<rmMaterial> newMaterials;
	newMaterials.reserve(nMaterials);
	for (rmU32 i = 0; i != nMaterials; ++i) {
		newMaterials.push_back(*materialPointers[i]);
	}

	materials.swap(newMaterials);
}

void rmScene::createMaterials(list<string> materialNames)
{
	materialNames.sort(&inAlphabeticOrder);
    materialNames.push_back("");
    for (list<string>::iterator i = materialNames.begin(), ie = (--list<string>::iterator(materialNames.end())); i != ie; ++i) {
	if (*i != *(++list<string>::iterator(i))) {
	    this->addMat().setName(*i);
	}
    }
}
