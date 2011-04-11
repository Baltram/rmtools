#include "StdAfx.h"
#include "twodim.h"
#include "output.h"
#include <iostream>

twodim::twodim(int size, int objcount)
{
	if ((size < 0) || (objcount < 0))
	{
		output::error("TWODIM: Got negative size or objcount integer", "TWODIM: Got negative size or objcount integer");
	}
	curobj = 0;
	curnum = 0;
	this->size = size;
	this->objcount = objcount;
	dpos = 0;
	drpos = 0;
	data = new char [size];
	nums = new int [objcount];
	starts = new int [objcount];
	memset(nums, 0, (sizeof(int) * objcount));
}

twodim::~twodim(void)
{
	delete[] data;
	delete[] nums;
	delete[] starts;
}

void twodim::follow(int num)
{
	for (int i = 0;i < num;i++)
	{
		drpos = ((int *) &data[drpos])[0];
	}
	curnum += num;
}

void twodim::addint(int obj, int val)
{
	if ((objcount <= obj) || (obj < 0))
	{
		output::error("ADDINT: Tried to acces inexistant object", "TWODIM: Tried to acces inexistant object");
	}
	if (size < (dpos + 8))
	{
		output::error("ADDINT: Container size exceeded", "TWODIM: Container size exceeded");
	}
	int objsc = nums[obj];
	if (objsc == 0)
	{
		starts[obj] = dpos;
		drpos = dpos;
		curobj = obj;
		curnum = 0;
	}
	else
	{
		drpos = starts[obj];
		curobj = obj;
		curnum = 0;
		follow(objsc - 1);
		((int *) &data[drpos])[0] = dpos;
	}
	((int *) &data[dpos + sizeof(int)])[0] = val;
	nums[obj]++;
	dpos += 8;
}

int twodim::getint(int obj, int num)
{
	if ((nums[obj] <= num) || (objcount <= obj) || (obj < 0) || (num < 0))
	{
		output::error("GETINT: Tried to acces inexistant element", "GETINT: Tried to acces inexistant element");
	}
	if ((curobj == obj) && (curnum <= num))
	{
		follow(num - curnum);
	}
	else
	{
		drpos = starts[obj];
		curobj = obj;
		curnum = 0;
		follow(num);
	}
	return ((int *) &data[drpos + sizeof(int)])[0];
}

void twodim::addstring(int obj, int count, char *val)
{
	if (count < 0)
	{
		output::error("ADDSTRING: Got negative char count", "ADDSTRING: Got negative char count");
	}
	if (((unsigned int) size) < (dpos + sizeof(int) + count + 1))
	{
		output::error("ADDSTRING: Container size exceeded", "ADDSTRING: Container size exceeded");
	}
	if ((objcount <= obj) || (obj < 0))
	{
		output::error("ADDSTRING: Tried to acces inexistant object", "ADDSTRING: Tried to acces inexistant object");
	}
	int objsc = nums[obj];
	if (objsc == 0)
	{
		starts[obj] = dpos;
		drpos = dpos;
		curobj = obj;
		curnum = 0;
	}
	else
	{
		drpos = starts[obj];
		curobj = obj;
		curnum = 0;
		follow(objsc - 1);
		((int *) &data[drpos])[0] = dpos;
	}
	memcpy(&data[dpos + sizeof(int)], val, count);
	data[dpos + sizeof(int) + count] = 0;
	nums[obj]++;
	dpos += (sizeof(int) + count + 1);
}

char * twodim::getstring(int obj, int num)
{
	if ((nums[obj] <= num) || (objcount <= obj) || (obj < 0) || (num < 0))
	{
		output::error("GETSTRING: Tried to acces inexistant element", "GETSTRING: Tried to acces inexistant element");
	}
	if ((curobj == obj) && (curnum <= num))
	{
		follow(num - curnum);
	}
	else
	{
		drpos = starts[obj];
		curobj = obj;
		curnum = 0;
		follow(num);
	}
	return &data[drpos + sizeof(int)];
}

void twodim::adddata(int obj, int size, char *data)
{
	if (size < 0)
	{
		output::error("ADDDATA: Got negative data size", "ADDDATA: Got negative data size");
	}
	if (((unsigned int) this->size) < (dpos + sizeof(int) + size))
	{
		output::error("ADDDATA: Container size exceeded", "ADDDATA: Container size exceeded");
	}
	if ((objcount <= obj) || (obj < 0))
	{
		output::error("ADDDATA: Tried to acces inexistant object", "ADDDATA: Tried to acces inexistant object");
	}
	int objsc = nums[obj];
	if (objsc == 0)
	{
		starts[obj] = dpos;
		drpos = dpos;
		curobj = obj;
		curnum = 0;
	}
	else
	{
		drpos = starts[obj];
		curobj = obj;
		curnum = 0;
		follow(objsc - 1);
		((int *) &(this->data)[drpos])[0] = dpos;
	}
	memcpy(&(this->data)[dpos + sizeof(int)], data, size);
	nums[obj]++;
	dpos += (sizeof(int) + size);
}

char * twodim::getdata(int obj, int num)
{
	return getstring(obj, num);
}

int twodim::getcount(int obj)
{
	if (obj < 0)
	{
		output::error("GETCOUNT: Got negative object num", "GETCOUNT: Got negative object num");
	}
	return nums[obj];
}

char * twodim::getcontainer(void)
{
	return data;
}

int twodim::getcontainerpos(int obj, int num)
{
	if ((nums[obj] <= num) || (objcount <= obj) || (obj < 0) || (num < 0))
	{
		output::error("GETCONTAINERPOS: Tried to acces inexistant element", "GETCONTAINERPOS: Tried to acces inexistant element");
	}
	if ((curobj == obj) && (curnum <= num))
	{
		follow(num - curnum);
	}
	else
	{
		drpos = starts[obj];
		curobj = obj;
		curnum = 0;
		follow(num);
	}
	return drpos;
}


