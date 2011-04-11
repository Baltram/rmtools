#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "rmDataTypes.h"
using namespace std;

//
// Since I use rmXXX files in different projects (e.g. Win32Console), I needed to provide
// output functions which work regardless of the output format (file, console, messageBox, etc.)
// This class represents my current approach.
//

/*
 * --level-- | --meaning--
 * 00 - 02:  | log 
 * 03 - 05:  | silent warning
 * 06 - 08:  | silent error
 * 09 - 11:  | warning
 * 12 - 14:  | error
 * 15 - 17:  | message
 */

class rmException : public exception
{
public:
    rmException(void);
    rmException(rmU32 errorId);
    rmU32 getId(void) const;
    rmLPCChar what(void) const;
private:
    rmU32 id;
    static rmChar buffer[100];
};

class rmOut
{      
private:
	typedef void (* pointerToSimpleOutputFunc)(string text);
	typedef void (* pointerToStandardOutputFunc)(string caption, string text);
	typedef void (* pointerToComplexOutputFunc)(rmU32 level, rmU32 line, string filename, string caption, string text);
	enum OutputFunctionType
	{
		SIMPLE,
		STANDARD,
		COMPLEX
	};
	struct OutputFunction
	{
		OutputFunctionType type;
		rmU32 level_min;
		rmU32 level_max;
		void * function;
	};
public:
	rmOut(void);
	rmOut(rmLPCChar filename);
	~rmOut(void);
	static rmU32 getNRegistredOutputFunctions(void);
	static void registerOutputFunction(void (* outputFunction)(string text), const rmU32 level_min, const rmU32 level_max);
	static void registerOutputFunction(void (* outputFunction)(string caption, string text), const rmU32 level_min, const rmU32 level_max);
	static void registerOutputFunction(void (* outputFunction)(rmU32 level, rmU32 line, string filename, string caption, string text), const rmU32 level_min, const rmU32 level_max);
	void output(const rmU32 line, const rmU32 level, const rmI32 exceptionNum, const string caption, const string text) const; 
private:
	static vector<OutputFunction> outputFunctions;
	string file;
};

//macros
//to use this macros, the object 'out' of the class 'rmOut' must be aviable. 

#define OUT_MSG(A) {\
ostringstream oss_tmp; \
oss_tmp << A; \
out.output(__LINE__, 16, (-1), "Message", oss_tmp.str());}

#define OUT_ERR(A) {\
ostringstream oss_tmp; \
oss_tmp << A; \
out.output(__LINE__, 13, (-1), "Error", oss_tmp.str());}

#define OUT_ERR_E(A, x) {\
ostringstream oss_tmp; \
oss_tmp << A; \
out.output(__LINE__, 13, x, "Error", oss_tmp.str());}

#define OUT_ERR_0(A) OUT_ERR_E(A, 0)

#define OUT_WARN(A) {\
ostringstream oss_tmp; \
oss_tmp << A; \
out.output(__LINE__, 10, (-1), "Warning", oss_tmp.str());}

#define OUT_SERR(A) {\
ostringstream oss_tmp; \
oss_tmp << A; \
out.output(__LINE__, 7, 0, "Error (/s)", oss_tmp.str());}

#define OUT_SERR_E(A, x) {\
ostringstream oss_tmp; \
oss_tmp << A; \
out.output(__LINE__, 7, x, "Error (/s)", oss_tmp.str());}

#define OUT_SWARN(A) {\
ostringstream oss_tmp; \
oss_tmp << A; \
out.output(__LINE__, 4, (-1), "Warning (/s)", oss_tmp.str());}

#define OUT_LOG(A) {\
ostringstream oss_tmp; \
oss_tmp << A; \
out.output(__LINE__, 1, (-1), "Log", oss_tmp.str());}

#define OUT_CPLX(y, B, A) {\
ostringstream oss_tmp; \
oss_tmp << A; \
ostringstream oss_tmp_2; \
oss_tmp_2 << B; \
out.output(__LINE__, y, (-1), oss_tmp_2.str(), oss_tmp.str());}

#define OUT_CPLX_E(y, B, A, x) {\
ostringstream oss_tmp; \
oss_tmp << A; \
ostringstream oss_tmp_2; \
oss_tmp_2 << B; \
out.output(__LINE__, y, x, oss_tmp_2.str(), oss_tmp.str());}

//end macros
