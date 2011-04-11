#include "StdAfx.h"
#include "scene.h"
#include <string>
using namespace std;

scene::scene(void)
{
	tv.x = 5;
	tv.y = 6;
	tv.z = 44;
	tf.a = 1;
	tf.b = 2;
	tf.c = 3;
	tf.matid = 1;
}

scene::~scene(void)
{
}
