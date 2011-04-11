#pragma once

#include "rmMurmurHash.h"
#include "rmFileStream.h"

class rmStringTable
{
public:
	rmStringTable(rmU32 nStringsExpected = 1500);
	void reset(void);

	rmU32 count(void) const;

    void readInStringList(rmInFileStream & src);
	void writeStringList(rmOutFileStream & dest) const;

	rmU16 registerString(const string & str);
	string getString(rmU16 index) const;
	rmU16 getIndex(const string & str) const;

private:
	struct Block
	{
		rmU32 hash;
		rmChar * data;
	};

private:
	rmBool equal(rmLPCChar s, rmU32 size, rmU32 hash, const Block & b) const;
	rmBool findString(rmLPCChar s, rmU32 size, const rmU16 *& bucket, rmU32 hash) const;
	void rehash(void);

private:
	static rmOut out;

	list< vector<rmChar> > buffer;
	vector<Block> blocks;
	vector<rmU16> buckets;
};

#if defined(_MSC_VER)
#define MY_INLINE __forceinline
#else // definded(_MSC_VER).
#define MY_INLINE inline
#endif // !definded(_MSC_VER).
