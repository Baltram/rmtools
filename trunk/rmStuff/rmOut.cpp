#include "rmOut.h"

rmChar rmException::buffer[100];

rmException::rmException(void) : id(0) {}

rmException::rmException(rmU32 errorId) : id(errorId) {}

rmU32 rmException::getId(void) const
{
    return id;
}

rmLPCChar rmException::what(void) const
{
    return itoa(id, buffer, 10);
}

rmOut::rmOut(void)
{
	file = string("[filename undefined]");
}

rmOut::rmOut(rmLPCChar filename)
{
	file = string(filename);
}

rmOut::~rmOut(void) {}

void rmOut::registerOutputFunction(void (* outputFunction)(string text), const rmU32 level_min, const rmU32 level_max)
{
	rmOut::OutputFunction of;
	of.function = reinterpret_cast<void *>(outputFunction);
	of.type = SIMPLE;
	of.level_min = level_min;
	of.level_max = level_max;
	outputFunctions.push_back(of);
}
void rmOut::registerOutputFunction(void (* outputFunction)(string caption, string text), const rmU32 level_min, const rmU32 level_max)
{
	rmOut::OutputFunction of;
	of.function = reinterpret_cast<void *>(outputFunction);
	of.type = STANDARD;
	of.level_min = level_min;
	of.level_max = level_max;
	outputFunctions.push_back(of);
}
void rmOut::registerOutputFunction(void (* outputFunction)(rmU32 level, rmU32 line, string filename, string caption, string text), const rmU32 level_min, const rmU32 level_max)
{
	rmOut::OutputFunction of;
	of.function = reinterpret_cast<void *>(outputFunction);
	of.type = COMPLEX;
	of.level_min = level_min;
	of.level_max = level_max;
	outputFunctions.push_back(of);
}

std::vector<rmOut::OutputFunction> rmOut::outputFunctions;

void rmOut::output(rmU32 line, rmU32 level, rmI32 exceptionNum, string caption, string text) const
{
	rmU32 numOutputFunctions = outputFunctions.size();
	for (rmU32 i = 0; i < numOutputFunctions; ++i) {
		OutputFunction & of = outputFunctions[i];
		if ((of.level_max >= level) && (of.level_min <= level)) {
			if (of.type == SIMPLE) {
				ostringstream o;
                                if (level < 9) {
					o << "**" << caption << "** " << file << ", line " << line << ": ";
				}
				o << text; 
				(reinterpret_cast<pointerToSimpleOutputFunc>(of.function))(o.str());
			}
			else if (of.type == STANDARD) {
				ostringstream o;
                                if (level < 9) {
					o << file << ", line " << line << ": ";
				}
				o << text;
				(reinterpret_cast<pointerToStandardOutputFunc>(of.function))(caption, o.str());
			}
			else {
				(reinterpret_cast<pointerToComplexOutputFunc>(of.function))(level, line, file, caption, text);
			}
		}
	}
        if (exceptionNum >= 0) {
                rmException rme(exceptionNum);
                throw rme;
        }
}

rmU32 rmOut::getNRegistredOutputFunctions(void)
{
    return outputFunctions.size();
}
