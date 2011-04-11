#pragma once

class twodim
{
public:
	twodim(int size, int objcount);
	~twodim(void);
	void addint(int obj, int val);
	int getint(int obj, int num); 
	void addstring(int obj, int count, char *val);
	void adddata(int obj, int size, char *data);
	char * getdata(int obj, int num);
	char * getstring(int obj, int num);
	int getcount(int obj);
	char * getcontainer(void);
	int getcontainerpos(int obj, int num);

private:
	int curobj;
	int curnum;
	int size;
	int objcount;
	char *data;
	int *nums;
	int *starts;
	int dpos;
	int drpos;
	void follow(int num);
};
