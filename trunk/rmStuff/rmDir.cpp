#include "rmDir.h"

rmBool (* rmDir::getDirFunc)(string directoryPath, vector<string> & files, vector<string> & directories) = 0;

rmOut rmDir::out("rmDir.cpp");

void rmDir::registerGetDirFunc(rmBool (* getDirFunc)(string directoryPath, vector<string> & files, vector<string> & directories))
{
	rmDir::getDirFunc = getDirFunc;
}

rmDir::rmDir(string directoryPath) : path(directoryPath), iFile(0), iDirectory(0), sub(0)
{
	if (getDirFunc == 0) {
		OUT_SERR("No 'getDir' function registered")
	}
	thisExists = (*getDirFunc)(directoryPath, files, directories);
	sort(files.begin(), files.end(), &inAlphabeticOrder);
	sort(directories.begin(), directories.end(), &inAlphabeticOrder);
}

rmDir::~rmDir(void)
{
	delete sub;
}

string rmDir::getDirectoryPath(void)
{
	return path;
}

rmBool rmDir::exists(void)
{
	return thisExists;
}

rmBool rmDir::containsFile(string fileName)
{
	return binary_search(files.begin(), files.end(), fileName, &inAlphabeticOrder);
}

rmBool rmDir::containsSubDirectory(string subDirectoryName)
{
	return binary_search(directories.begin(), directories.end(), subDirectoryName, &inAlphabeticOrder);
}

rmBool rmDir::getFile(string & dest)
{
	if (iFile == files.size()) {
		return false;
	}
	dest = files[iFile++];
	return true;
}

rmBool rmDir::getFileAbs(string & dest)
{
	string tmp;
	if (getFile(tmp)) {
		dest = getDirectoryPath();
		if (dest[dest.size() - 1] != '\\') {
			dest.push_back('\\');
		}
		dest.append(tmp);
		return true;
	}
	return false;
}

rmBool rmDir::getSubDirectory(string & dest)
{
	if (iDirectory == directories.size()) {
		return false;
	}
	dest = directories[iDirectory++];
	return true;
}

rmBool rmDir::getSubDirectoryAbs(string & dest)
{
	string tmp;
	if (getSubDirectory(tmp)) {
		dest = getDirectoryPath();
		if (dest[dest.size() - 1] != '\\') {
			dest.push_back('\\');
		}
		dest.append(tmp);
		return true;
	}
	return false;
}

rmBool rmDir::getDeepFile(string & dest)
{
	string directory;
	if (!sub) {
		if (getFile(dest)) {
			return true;
		}
		if (!getSubDirectoryAbs(directory)) {
			return false;
		}
		sub = new rmDir(directory);
	}
	if (sub->getDeepFile(dest)) {
		return true;
	}
	if (!getSubDirectoryAbs(directory)) {
		return false;
	}
	delete sub;
	sub = new rmDir(directory);
	return getDeepFile(dest);
}

rmBool rmDir::getDeepFileAbs(string & dest)
{
	string tmp;
	if (getDeepFile(tmp)) {
		dest = getDirectoryPath();
		if (dest[dest.size() - 1] != '\\') {
			dest.push_back('\\');
		}
		dest.append(tmp);
		return true;
	}
	return false;
}

void rmDir::restart(void)
{
	iFile = 0;
	iDirectory = 0;
	delete sub;
	sub = 0;
}
