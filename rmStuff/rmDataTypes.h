#pragma once

#include "math.h"
#include <string>
#include <cstring> // In qt creator necessary for using 'memset' etc. but for some strange reason <string> must be also included.
#include <vector>
#include <list>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
using namespace std;

#ifdef WIN32
	typedef signed __int64		rmI64;
	typedef unsigned __int64	rmU64;
	typedef signed int			rmI32;
	typedef unsigned int		rmU32;
	typedef signed short		rmI16;
	typedef unsigned short		rmU16;
	typedef signed char			rmI8;
	typedef unsigned char		rmU8;
	typedef double				rmF64; 
	typedef float				rmF32;
	typedef char				rmChar;
	typedef char *				rmLPChar;
	typedef const char *		rmLPCChar;
	typedef bool				rmBool;
#else
	#error Platform not supported!
#endif

/*
#ifndef _MSC_VER
#define RM_QT
#endif // _MSC_VER.

#ifdef RM_QT
#define RM_IOS_OPENMODE std::_Ios_Openmode
#else
#define RM_IOS_OPENMODE std::ios_base::openmode
#endif // RM_QT.
*/

#define RM_IOS_OPENMODE std::ios_base::openmode

static void flexCopy(void * dest, const void * src, rmU32 nElements, rmU32 elementSize, rmU32 stride = 0, rmU32 destStride = 0, rmBool zeroMemory = false)
{
    destStride = max(destStride, elementSize);
    stride = max(stride, elementSize);
    if ((stride == elementSize) && (destStride == elementSize)) {
	memcpy(dest, src, (elementSize * nElements));
	return;
    }

    if (zeroMemory) {
	memset(dest, 0, nElements * destStride);
    }

    rmU8 * dest2 = reinterpret_cast<rmU8 *>(dest);
    const rmU8 * src2 = reinterpret_cast<const rmU8 *>(src);
    for (rmU32 i = 0; i != nElements; ++i) {
	memcpy(dest2, src2, elementSize);
	dest2 += destStride;
	src2 += stride;
    }
}

static rmU32 findCStringInArray(rmLPCChar stringToFind, rmLPCChar const * stringArray, rmU32 arraySize)
{
	rmI32 result;
	rmU32 curPos;
	rmI32 lowerBound = 0;
	rmI32 upperBound = arraySize - 1;

	while (true) {
		curPos = (lowerBound + upperBound) / 2;
		result = strcmp(stringToFind, stringArray[curPos]);
		if (result > 0) {
			lowerBound = curPos + 1;
		}
		else if (result < 0) {
			upperBound = curPos - 1;
		}
		else {
			return curPos;
		}
		if (lowerBound > upperBound) {
			return -1;
		}
	}
}

static rmU32 findCStringInUnsortedArray(rmLPCChar stringToFind, rmLPCChar const * stringArray, rmU32 arraySize)
{
    for (rmU32 i = 0; i != arraySize; ++i) {
	if (strcmp(stringToFind, stringArray[i]) == 0) {
	    return i;
	}
    }
    return -1;
}

static rmU32 findStringInArray(std::string stringToFind, std::string * stringArray, rmU32 arraySize)
{
	rmI32 result;
	rmU32 curPos;
	rmI32 lowerBound = 0;
	rmI32 upperBound = arraySize - 1;

	while (true) {
		curPos = (lowerBound + upperBound) / 2;
		result = stringToFind.compare(stringArray[curPos]);
		if (result > 0) {
			lowerBound = curPos + 1;
		}
		else if (result < 0) {
			upperBound = curPos - 1;
		}
		else {
			return curPos;
		}
		if (lowerBound > upperBound) {
			return -1;
		}
	}
}

static rmBool inAlphabeticOrder(const string & s1, const string & s2)
{
    return (s1.compare(s2) < 0);
}

static rmF32 zeroAffineFMod(rmF32 a, rmF32 b) 
{
	return (a - floor(a / b + ((float) 0.5)));
}

template <class T>
static void freeVector(std::vector<T> & toFree)
{
	std::vector<T> tmp(0);
	toFree.swap(tmp);
}

#define ROUGH_F32_TOLERANCE 0.0001
class rmRoughF32
{
public:
	rmRoughF32(const rmF32 & f)
	{
		val = f;
	}
	rmBool operator == (const rmF32 & f) const {
		if (f > val) {
			return (f - val < ((rmF32) ROUGH_F32_TOLERANCE));
		}
		else {
			return (val - f < ((rmF32) ROUGH_F32_TOLERANCE));
		}
	}
	rmF32 val;
};

#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif

class rmVec3
{
public:
	rmVec3() {}

	rmVec3(rmF32 x, rmF32 y, rmF32 z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	~rmVec3() {}
	rmVec3 operator + (const rmVec3 & v) const
    {
        return rmVec3(x + v.x, y + v.y, z + v.z);
    }
	rmVec3 operator - (const rmVec3 & v) const
    {
        return rmVec3(x - v.x, y - v.y, z - v.z);
    }
	rmVec3 operator * (const rmF32 & f) const
    {
        return rmVec3(x * f, y * f, z * f);
    }
	rmVec3 operator / (const rmF32 & f) const
    {
        return rmVec3(x / f, y / f, z / f);
    }
	void operator += (const rmVec3 & v)
    {
        x += v.x;
		y += v.y;
		z += v.z;
    }
	void operator -= (const rmVec3 & v)
    {
        x -= v.x;
		y -= v.y;
		z -= v.z;
    }
	void operator *= (const rmF32 & f)
    {
        x *= f;
		y *= f;
		z *= f;
    }
	void operator /= (const rmF32 & f)
    {
        x /= f;
		y /= f;
		z /= f;
    }
	rmF32 magnitude(void) const
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}
	void normalize(void)
	{
		rmF32 m = magnitude();
		*this /= m;
	}
	rmVec3 normalizeTo(void) const
	{
		rmVec3 tmp = *this;
		tmp.normalize();
		return tmp;
	}
	void orthogonalize(const rmVec3 normalizedOrientation)
	{
		*this -= (normalizedOrientation * dot(normalizedOrientation, *this));
	}
	rmVec3 orthogonalizeTo(const rmVec3 normalizedOrientation) const
	{
		rmVec3 tmp = *this;
		tmp.orthogonalize(normalizedOrientation);
		return tmp;
	}
	static rmVec3 cross(const rmVec3 a, const rmVec3 b)
	{
		return rmVec3((a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x));
	}
	static rmF32 dot(const rmVec3 a, const rmVec3 b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}
	static rmF32 cosOfAngle(rmVec3 v1, rmVec3 v2)
	{
		return dot(v1.normalizeTo(), v2.normalizeTo());
	}
	static rmF32 cosOfAngle(rmVec3 v1, rmBool isNormalized1, rmVec3 v2, rmBool isNormalized2)
	{
		rmF32 m = 1.0;
		if (isNormalized1 == false) {
			m *= v1.magnitude();
		}
		if (isNormalized2 == false) {
			m *= v2.magnitude();
		}
		return (dot(v1, v2) / m);
	}
	rmVec3 & swapYZ(void)
    {
	rmF32 ty = y;
	y = z;
	z = ty;
	return *this;
    }
	rmF32 x;
	rmF32 y;
	rmF32 z;
};

class rmVec4
{
public:
	rmVec4() {}
	rmVec4(rmVec3 vec3, rmF32 w)
	{
		this->vec3 = vec3;
		this->w = w;
	}
	~rmVec4() {}
	void swapYZ(void)
	{
	    vec3.swapYZ();
	    w *= (-1);
	}
	rmVec3 vec3;
	rmF32 w;
};

class rmPoint3
{
public:
	rmPoint3() {}
	rmPoint3(rmF32 x, rmF32 y, rmF32 z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	~rmPoint3() {}
	rmPoint3 operator + (const rmVec3 & v) const
    {
        return rmPoint3(x + v.x, y + v.y, z + v.z);
    }
	rmPoint3 operator - (const rmVec3 & v) const
    {
        return rmPoint3(x - v.x, y - v.y, z - v.z);
    }
	rmVec3 operator - (const rmPoint3 & p) const
    {
        return rmVec3(x - p.x, y - p.y, z - p.z);
    }
	void operator += (const rmVec3 & v)
    {
        x += v.x;
		y += v.y;
		z += v.z;
    }
	void operator -= (const rmVec3 & v)
    {
        x -= v.x;
		y -= v.y;
		z -= v.z;
    }
    rmPoint3 & swapYZ(void)
    {
	rmF32 ty = y;
	y = z;
	z = ty;
	return *this;
    }
	rmF32 x;
	rmF32 y;
	rmF32 z;
};

class rmBox
{
public:
    rmBox & swapYZ(void)
    {
	min.swapYZ();
	max.swapYZ();
	return *this;
    }
public:
    rmPoint3 min;
    rmPoint3 max;

};

class rmQuat
{
public:
	rmQuat() {}
	rmQuat(rmF32 x, rmF32 y, rmF32 z, rmF32 w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	~rmQuat() {}
	rmF32 x;
	rmF32 y;
	rmF32 z;
	rmF32 w;
};

class rmRGB
{
public:
	rmRGB() {}
	rmRGB(rmU8 r, rmU8 g, rmU8 b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
	~rmRGB() {}
	rmU8 r;
	rmU8 g;
	rmU8 b;
};

/*
class rmFace
{
public:
	rmFace() {}
	rmFace(rmU32 a, rmU32 b, rmU32 c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->sgs = 0;
		this->matId = 0;
	}
	rmFace(rmU32 a, rmU32 b, rmU32 c, rmU32 sgs, rmU32 matId)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->sgs = sgs;
		this->matId = matId;
	}
	~rmFace() {}
	union {
		rmU32 verts[3];
		struct {
			rmU32 a;
			rmU32 b;
			rmU32 c;
		};
	};
	rmU32 sgs;
	rmU16 matId;
};
*/

class rmFaceBase
{
public:
	rmFaceBase() {}
	rmFaceBase(rmU32 a, rmU32 b, rmU32 c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
	}
	~rmFaceBase() {}
	void invert(void)
	{
	    rmU32 ta = a;
	    a = c;
	    c = ta;
	}
	union {
		rmU32 verts[3];
		struct {
			rmU32 a;
			rmU32 b;
			rmU32 c;
		};
	};
};

class rmFace : public rmFaceBase
{
public:
	rmFace() {}
	rmFace(rmU32 a, rmU32 b, rmU32 c) : rmFaceBase(a, b, c) {}
	rmFace(rmU32 a, rmU32 b, rmU32 c, rmU32 sgs, rmU32 matId) : rmFaceBase(a, b, c)
	{
		this->sgs = sgs;
		this->matId = matId;
	}
	~rmFace() {}
	rmU32 sgs;
	rmU16 matId;
};

typedef rmFaceBase rmTVFace;
typedef rmFaceBase rmVNFace;
typedef rmFaceBase rmVTFace;

struct rmWFNormal
{
	rmVec3 normal;
	rmF32 weightings[3];
};

typedef rmU32 rmFVIndex; // rmFVIndex / 3: face index | rmFVIndex % 3: vert index in face (0, 1, 2)

struct PosDescriptor
{
	rmU32 position;
	std::string description;
};

struct rmWeighting
{
	rmF32 weight;
	rmU16 iBone;
	rmU16 padding;
};

struct rmVertWeightingsInfo
{
	rmU32 iFirstWeighting;
	rmU32 nWeightings;
};

struct rmGuid
{
    rmU8 value[16];
    std::string toText(void) const
    {
        rmU32 offsets[16] = {7, 5, 3, 1, 12, 10, 17, 15, 20, 22, 25, 27, 29, 31, 33, 35};
        rmChar buf[] = "{00000000-0000-0000-0000-000000000000}\0##########################################\0";
        for (rmU32 i = 0; i != 16; ++i) {
            rmChar * c = (buf + offsets[i]), tmp = c[2];
            c[0] = '0';
            _itoa(value[i], (c + ((value[i] < 16) ? 1 : 0)), 16);
            c[0] = toupper(c[0]);
            c[1] = toupper(c[1]);
            c[2] = tmp;
        }

        return std::string(buf);
    }
    void fromText(std::string text) // Must have the form '{HHHHHHHH-HHHH-HHHH-HHHH-HHHHHHHHHHHH}', 'H' for [0123456789ABCDEFabcdef].
    {
        rmU32 offsets[16] = {7, 5, 3, 1, 12, 10, 17, 15, 20, 22, 25, 27, 29, 31, 33, 35};
        if (text.size() != 38) {
            return;
        }
        for (rmU32 i = 0; i != 16; ++i) {
            rmChar & c = text[offsets[i] + 2], tmp = c;
            c = 0;
            value[i] = strtol((&c - 2), 0, 16);
            c = tmp;
        }
    }
};
