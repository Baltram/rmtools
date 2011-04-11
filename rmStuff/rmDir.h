#pragma once

#include <algorithm>
#include "rmOut.h"

class rmDir
{
public:
	static void registerGetDirFunc(rmBool (* getDirFunc)(string directoryPath, vector<string> & files, vector<string> & directories));

public:
	rmDir(string directoryPath);
	~rmDir(void);

	string getDirectoryPath(void);

	rmBool exists(void);
	rmBool containsFile(string fileName);
	rmBool containsSubDirectory(string subDirectoryName);

	rmBool getFile(string & dest);
	rmBool getFileAbs(string & dest);
	rmBool getSubDirectory(string & dest);
	rmBool getSubDirectoryAbs(string & dest);
	rmBool getDeepFile(string & dest);
	rmBool getDeepFileAbs(string & dest);
	void restart(void);

private:
	static rmBool (* getDirFunc)(string directoryPath, vector<string> & files, vector<string> & directories);

	static rmOut out;

private:
	string path;

	vector<string> files;
	rmU32 iFile;

	vector<string> directories;
	rmU32 iDirectory;

	rmBool thisExists;

	rmDir * sub;
};
