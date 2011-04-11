#pragma once

#include <string>
using namespace std;

class scene
{
	struct vec3d
	{
		float x;
		float y;
		float z;
	};
	struct face
	{
		int a;
		int b;
		int c;
		int sgs;
		int matid;
	};
	struct object
	{
		int type; //1 - mesh

	};

public:
	scene(void);
	~scene(void);
private:
	vec3d tv;
	face tf;
};
