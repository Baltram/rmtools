#pragma once

#include "file.h"
#include <vector>

class mesh
{
public:
	//mesh(file *maxfile, int numverts, int numfaces, int numtverts, int vertspos, int facespos, int tvertspos, int tvfacespos, float firstvertx, float firstverty, float firstvertz);
	mesh(file *maxfile, char *objinf, int nummats, float angle);
	mesh(file *objfile, int numverts, int numfaces, int numtverts, int numvnormals, int numnovnfaces, vector<int> &breaks, float angle);
	mesh(file *asefile, int pos, int nummats, float angle);
	~mesh(void);
	int getnumverts(void);
	int getnumfaces(void);
	int getnumtverts(void);
	int getnumtvfaces(void);
	float* getvert(int num);
	int* getface(int num);
	float* gettvert(int num);
	int* gettvface(int num);
	void output(file *xmshfile, int matid, int *allindices, int *allindpos, int *numprintedverts, float *min, float *max);
	void getfnormal(float *normal, int facenum);
	void getftangent(float *tangent, int facenum, int *isrighthanded);
	void getftangent(float *tangent, int facenum);
	int getfsgs(int facenum);
	int getfmatid(int facenum);
	float cosofangle(float *vec1, float *vec2, int isnormalized1, int isnormalized2);
	void getboundingbox(int matnum);

private:
	file *xmsh;
	file *maxf;
	file *objf;
	file *asef;
	int numvnormals;
	vector<int> breaks;
	int nummats;
	int numverts;
	int numfaces;
	int numtverts;
	int numtvfaces;
	int vertspos;
	int facespos;
	int tvertspos;
	int tvfacespos;
	float *verts;
	int *faces;
	float *tverts;
	int *tvfaces;
	float *vnormals;
	int *vnfaces;
	int facespermat[1000];
	void orth(float *tan, float *norm, float *newtan);
	void normalize(float *vec);
	void calcvnormalsbysgs(void);
	void calcvnormalsbyangle(float angle);
	void initncalc(float *fnormals, float *fnmod);
	void fillvnfaces(void);
	//void getfnormal(float *normal, int facenum);
	//void getftangent(float *tangent, int facenum);
};
