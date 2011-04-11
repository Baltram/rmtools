#include "StdAfx.h"
#include "mesh.h"
#include "file.h"
#include <math.h>
#include "output.h"
#include "twodim.h"
#include <vector>

#include <iostream>


mesh::mesh(file *asefile, int pos, int nummats, float angle)
{
	char filter[4] = {9, 32, 58, 44}; // {tab, space, double point, comma}

	this->asef = asefile;
	asef->jump(pos);

	asef->linein(filter, 2);
	numverts = asef->lint(1);
	asef->linein(filter, 2);
	numfaces = asef->lint(1);

	numtverts = 0;
	numtvfaces = numfaces;

	verts = new float [3 * numverts];
	faces = new int [5 * numfaces];
	vnfaces = new int [3 * numfaces];
	int vpos = 0;
	int fpos = 0;
	int tvpos = 0;

	asef->searchbytes("*MESH_VERTEX", 12); //finds first "*MESH_VERTEX_LIST {"
	asef->skip(1); //any small number more than 0 would work here too
	asef->searchbytes("*MESH_VERTEX", 12);
	for (int i = 0;i < numverts;i++)
	{
		asef->linein(filter, 2);
		verts[vpos + 0] = asef->lfloat(2);
		verts[vpos + 1] = asef->lfloat(3);
		verts[vpos + 2] = asef->lfloat(4);
		vpos += 3;
	}

	asef->searchbytes("*MESH_FACE", 10); //finds first "*MESH_FACE_LIST {"
	asef->skip(1); //any small number more than 0 would work here too
	asef->searchbytes("*MESH_FACE", 10);
	for (int i = 0;i < numfaces;i++)
	{
		asef->linein(filter, 4);
		faces[fpos + 0] = asef->lint(3);
		faces[fpos + 1] = asef->lint(5);
		faces[fpos + 2] = asef->lint(7);
		
		int sgcount = (asef->numlineparts() - 17);
		int sgs = 0;
		for (int j = 0;j < sgcount;j++)
		{
			sgs |= (1 << (asef->lint(15 + j) - 1));
		}
		faces[fpos + 3] = sgs;

		((short int *) &faces[fpos + 4])[1] = ((short int) (asef->lint(16 + sgcount) % nummats));

		fpos += 5;
	}
	asef->linein("", 0);
	asef->linein(filter, 2);
	if (strcmp("*MESH_NUMTVERTEX", asef->lascii(0)) == 0) //mesh has texture mapping
	{
		numtverts = asef->lint(1);

		tverts = new float [3 * numtverts + 9];
		
		if (numtverts > 0)
		{
			asef->searchbytes("*MESH_TVERT", 11); //finds first "*MESH_TVERTLIST {"
			asef->skip(1); //any small number more than 0 would work here too
			asef->searchbytes("*MESH_TVERT", 11);
		}
		for (int i = 0;i < numtverts;i++)
		{
			asef->linein(filter, 2);
			tverts[tvpos + 0] = asef->lfloat(2);
			tverts[tvpos + 1] = asef->lfloat(3);
			tverts[tvpos + 2] = asef->lfloat(4);
			tvpos += 3;
		}
	}
	else
	{
		tverts = new float [3 * numtverts + 9]; //3 * 0 + 9 = 9
	}
	
	tverts[tvpos + 0] = 0.0;
	tverts[tvpos + 1] = 0.0;
	tverts[tvpos + 2] = 0.0;
	tverts[tvpos + 3] = 1.0;
	tverts[tvpos + 4] = 0.0;
	tverts[tvpos + 5] = 0.0;
	tverts[tvpos + 6] = 0.0;
	tverts[tvpos + 7] = 1.0;
	tverts[tvpos + 8] = 0.0;
	tvpos += 9; //not necessary
	numtverts += 3;
	
	tvfaces = new int [3 * numtvfaces];
	int id = 0;
	int tvr = numtverts - 3;
	for (int i = 0;i < numtvfaces;i++)
	{
		tvfaces[id + 0] = tvr + 0;
		tvfaces[id + 1] = tvr + 1;
		tvfaces[id + 2] = tvr + 2;
		id += 3;
	}
	if (numtverts > 3)
	{
		asef->linein("", 0);
		asef->linein(filter, 2);
		if (strcmp("*MESH_NUMTVFACES", asef->lascii(0)) == 0) //mesh has texture faces
		{
			int numstoredtvf = asef->lint(1);
			if (numstoredtvf > 0)
			{
				asef->searchbytes("*MESH_TFACE", 11); //finds first "*MESH_TFACELIST {"
				asef->skip(1); //any small number more than 0 would work here too
				asef->searchbytes("*MESH_TFACE", 11);
			}
			int tvfpos;
			for (int i = 0;i < numstoredtvf;i++)
			{
				asef->linein(filter, 2);
				tvfpos = 3 * asef->lint(1);
				tvfaces[tvfpos + 0] = asef->lint(2);
				tvfaces[tvfpos + 1] = asef->lint(3);
				tvfaces[tvfpos + 2] = asef->lint(4);	
			}
		}
	}

	memset(facespermat, 0, (1000 * sizeof(int)));
	short int *matids = (short int *) faces; //matids[9], matids[19], matids[29] and so on are the matids 
	int n = 9;
	for (int i = 0;i < numfaces;i++)
	{
		facespermat[matids[n]]++;
		n += 10;
	}

	if (angle >= 0)
	{
		calcvnormalsbyangle(angle);
	}
	else
	{
		calcvnormalsbysgs();
	}
}

mesh::mesh(file *objfile, int numverts, int numfaces, int numtverts, int numvnormals, int numnovnfaces, vector<int> &breaks, float angle)
{
	this->objf = objfile;
	this->numverts = numverts;
	this->numfaces = numfaces;
	this->numtverts = numtverts + 3;
	this->numtvfaces = numfaces;
	verts = new float [3 * numverts];
	faces = new int [5 * numfaces];
	tverts = new float [3 * numtverts + 9];
	tvfaces = new int [3 * numfaces];
	if (angle < 0)
	{
		this->numvnormals = numvnormals + numnovnfaces;
		vnormals = new float [3 * (numvnormals + numnovnfaces)];
	}
	vnfaces = new int [3 * numfaces];

	float fnormal[3];
	int vpos = 0;
	int fpos = 0;
	int tvpos = 0;
	int tvfpos = 0;
	int vnpos = 0;
	int vnfpos = 0;
	int fnpos = (3 * numvnormals);
	tverts[(3 * numtverts) + 0] = 0.0;
	tverts[(3 * numtverts) + 1] = 0.0;
	tverts[(3 * numtverts) + 2] = 0.0;
	tverts[(3 * numtverts) + 3] = 1.0;
	tverts[(3 * numtverts) + 4] = 0.0;
	tverts[(3 * numtverts) + 5] = 0.0;
	tverts[(3 * numtverts) + 6] = 0.0;
	tverts[(3 * numtverts) + 7] = 1.0;
	tverts[(3 * numtverts) + 8] = 0.0;

	memset(facespermat, 0, (1000 * sizeof(int)));

	objf->jump(0);

	int numlindices = 3;
	int matid = 0;
	int hastverts = 0;
	int hasvnormals = 0;
	int bcount = 0;

	char filter[3] = {47, 32, 9};
	char *text;

	int linenum = -1;

	while (!objf->eof())
	{
		objf->linein(filter, 3);
		linenum++;
		if (objf->numlineparts())
		{
			text = objf->lascii(0);
			if (text[0] == 'v')
			{
				if (text[1] == 't')
				{
					tverts[tvpos + 0] = objf->lfloat(1);
					tverts[tvpos + 1] = objf->lfloat(2);
					//tverts[tvpos + 2] = objf->lfloat(3);
					tverts[tvpos + 2] = 0;
					tvpos += 3;
				}
				else if (text[1] == 'n')
				{
					if (angle < 0)
					{
						vnormals[vnpos + 0] = objf->lfloat(1);
						vnormals[vnpos + 1] = objf->lfloat(2);
						vnormals[vnpos + 2] = objf->lfloat(3);
						normalize(&vnormals[vnpos]);
						vnpos += 3;
					}
				}
				else
				{
					verts[vpos + 0] = objf->lfloat(1);
					verts[vpos + 1] = objf->lfloat(3) * (-1);
					verts[vpos + 2] = objf->lfloat(2);
					vpos += 3;
				}
			}
			else if (text[0] == 'f')
			{
				if (linenum == breaks[5 * bcount])
				{
					matid = breaks[5 * bcount + 1];
					numlindices = breaks[5 * bcount + 2];
					hastverts = breaks[5 * bcount + 3];
					hasvnormals = breaks[5 * bcount + 4];
					bcount++;
				}
				for (int i = 0;i < (numlindices - 2);i++)
				{
					int shift = ((1 + hasvnormals + hastverts) * i);
					if (shift > 0)//
					{
						shift += 0;
					}
					facespermat[matid]++; 
					if (hastverts)
					{
						if (hasvnormals)
						{
							faces[fpos + 0] = objf->lint(1) - 1;
							faces[fpos + 1] = objf->lint(4 + shift) - 1;
							faces[fpos + 2] = objf->lint(7 + shift) - 1;
							((short int *) &faces[fpos + 4])[1] = matid;
							fpos += 5;
							tvfaces[tvfpos + 0] = objf->lint(2) - 1;
							tvfaces[tvfpos + 1] = objf->lint(5 + shift) - 1;
							tvfaces[tvfpos + 2] = objf->lint(8 + shift) - 1;
							tvfpos += 3;
							if (angle < 0)
							{
								vnfaces[vnfpos + 0] = objf->lint(3) - 1;
								vnfaces[vnfpos + 1] = objf->lint(6 + shift) - 1;
								vnfaces[vnfpos + 2] = objf->lint(9 + shift) - 1;
								vnfpos += 3;
							}
						}
						else
						{
							faces[fpos + 0] = objf->lint(1) - 1;
							faces[fpos + 1] = objf->lint(3 + shift) - 1;
							faces[fpos + 2] = objf->lint(5 + shift) - 1;
							((short int *) &faces[fpos + 4])[1] = matid;
							fpos += 5;
							tvfaces[tvfpos + 0] = objf->lint(2) - 1;
							tvfaces[tvfpos + 1] = objf->lint(4 + shift) - 1;
							tvfaces[tvfpos + 2] = objf->lint(6 + shift) - 1;
							tvfpos += 3;
							vnfaces[vnfpos + 0] = (fnpos / 3);
							vnfaces[vnfpos + 1] = (fnpos / 3);
							vnfaces[vnfpos + 2] = (fnpos / 3);
							vnfpos += 3;
							getfnormal(fnormal, ((fpos / 5) - 1));
							vnormals[fnpos + 0] = fnormal[0];
							vnormals[fnpos + 1] = fnormal[1];
							vnormals[fnpos + 2] = fnormal[2];
							fnpos += 3;
						}
					}
					else
					{
						if (hasvnormals)
						{
							faces[fpos + 0] = objf->lint(1) - 1;
							faces[fpos + 1] = objf->lint(3 + shift) - 1;
							faces[fpos + 2] = objf->lint(5 + shift) - 1;
							((short int *) &faces[fpos + 4])[1] = matid;
							fpos += 5;
							tvfaces[tvfpos + 0] = numtverts + 0;
							tvfaces[tvfpos + 1] = numtverts + 1;
							tvfaces[tvfpos + 2] = numtverts + 2;
							tvfpos += 3;
							if (angle < 0)
							{
								vnfaces[vnfpos + 0] = objf->lint(2) - 1;
								vnfaces[vnfpos + 1] = objf->lint(4 + shift) - 1;
								vnfaces[vnfpos + 2] = objf->lint(6 + shift) - 1;
								vnfpos += 3;
							}
						}
						else
						{
							faces[fpos + 0] = objf->lint(1) - 1;
							faces[fpos + 1] = objf->lint(2 + shift) - 1;
							faces[fpos + 2] = objf->lint(3 + shift) - 1;
							((short int *) &faces[fpos + 4])[1] = matid;
							fpos += 5;
							tvfaces[tvfpos + 0] = numtverts + 0;
							tvfaces[tvfpos + 1] = numtverts + 1;
							tvfaces[tvfpos + 2] = numtverts + 2;
							tvfpos += 3;
							vnfaces[vnfpos + 0] = (fnpos / 3);
							vnfaces[vnfpos + 1] = (fnpos / 3);
							vnfaces[vnfpos + 2] = (fnpos / 3);
							vnfpos += 3;
							getfnormal(fnormal, ((fpos / 5) - 1));
							vnormals[fnpos + 0] = fnormal[0];
							vnormals[fnpos + 1] = fnormal[1];
							vnormals[fnpos + 2] = fnormal[2];
							fnpos += 3;
						}
					}
				}
			}
		}
	}
	if (angle >= 0)
	{
		calcvnormalsbyangle(angle);
	}
}
mesh::mesh(file *maxfile, char *objinf, int nummats, float angle)
{
	int rv = 0;
	int rf = 0;
	int rtv = 0;
	int rtf = 0;
	this->nummats = nummats;
	maxf = maxfile;
	vertspos = -1;
	facespos = -1;
	tvertspos = -1;
	tvfacespos = -1;
	numverts = file::fakeintasint(((float *) &objinf[0])[0]);
	numfaces = file::fakeintasint(((float *) &objinf[4])[0]);
	numtverts = file::fakeintasint(((float *) &objinf[8])[0]);
	numtvfaces = numfaces;
	/*
	numtvfaces = 0;
	if (numtverts > 0)
	{
		numtvfaces = numfaces;
	}
	*/
	verts = new float [numverts * 3];
	faces = new int [numfaces * 5];
	tverts = new float [numtverts * 3];
	tvfaces = new int [numtvfaces * 3];
	vnfaces = new int [3 * numfaces];
	if (numverts >= 5)
	{
		vertspos = maxf->findverts((float *) &objinf[12], numverts);
		rv = maxf->getreadpos(1);
		if (vertspos == (-1))
		{
			output::error("Vertices nicht gefunden. Probieren Sie in 3ds Max / gmax 'Risen Tools' > 'Meshes erneuern'", "Vertices not found. In 3ds Max / gmax try 'Risen Tools' > 'Renew Meshes'");
		}
		maxf->skip(numverts * 12);
		maxf->jump(vertspos);
		maxf->read(((char *) &verts[0]), (numverts * 12));
		float vx = (((float *) &objinf[12])[0] - verts[0]);
		float vy = (((float *) &objinf[16])[0] - verts[1]);
		float vz = (((float *) &objinf[20])[0] - verts[2]);
		for (int i = 0;i < numverts;i++)
		{
			verts[3 * i] += vx;
			verts[3 * i + 1] += vy;
			verts[3 * i + 2] += vz;
		}
	}
	else
	{
		memcpy(&verts[0], &objinf[12], (numverts * 12));
	}
	if (numfaces >= 5)
	{
		facespos = maxf->findfaces(&objinf[72], numfaces);
		rf = maxf->getreadpos(1);
		if (facespos == (-1))
		{
			output::error("Faces not found", "Faces not found");
		}
		maxf->skip(numfaces * 20);
		maxf->jump(facespos);
		maxf->read(((char *) &faces[0]), (numfaces * 20));
		facespos = maxf->getreadpos(1);
	}
	else
	{
		for (int i = 0;i < numfaces;i++)
		{
			faces[i * 5 + 0] = file::fakeintasint(((float*) &objinf[72 + i * 12 + 0])[0]);
			faces[i * 5 + 1] = file::fakeintasint(((float*) &objinf[72 + i * 12 + 4])[0]);
			faces[i * 5 + 2] = file::fakeintasint(((float*) &objinf[72 + i * 12 + 8])[0]);
			faces[i * 5 + 3] = 0;
			((char*) &faces[i * 5 + 4])[2] = 0; 
			((char*) &faces[i * 5 + 4])[3] = 0; 
		}
	}
	if (numtverts >= 5)
	{
		tvertspos = maxf->findtverts(&objinf[132], numtverts);
		rtv = maxf->getreadpos(1);
		if (tvertspos == (-1))
		{
			output::error("Texture verts not found", "Texture verts not found");
		}
		maxf->skip(numtverts * 12);
		maxf->jump(tvertspos);
		maxf->read(((char *) &tverts[0]), (numtverts * 12));
	}
	else
	{
		memcpy(&tverts[0], &objinf[132], (numtverts * 12));
	}
	if ((numtverts > 0) && (numtvfaces >= 5))
	{
		tvfacespos = maxf->findtvfaces(&objinf[192], numtvfaces);
		rtf = maxf->getreadpos(1);
		if (tvfacespos == (-1))
		{
			output::error("Texture Faces not found", "Texture Faces not found");
		}
		maxf->skip(numtvfaces * 12);
		maxf->jump(tvfacespos);
		maxf->read(((char *) &tvfaces[0]), (numtvfaces * 12));
	}
	else if (numtverts > 0)
	{
		for (int i = 0;i < numtvfaces;i++)
		{
			tvfaces[i * 3 + 0] = file::fakeintasint(((float*) &objinf[192 + i * 12 + 0])[0]);
			tvfaces[i * 3 + 1] = file::fakeintasint(((float*) &objinf[192 + i * 12 + 4])[0]);
			tvfaces[i * 3 + 2] = file::fakeintasint(((float*) &objinf[192 + i * 12 + 8])[0]); 
		}
	}
	if (numtverts == 0)
	{
		numtverts = 3;
		tverts = new float [9];
		tverts[0] = 0.0;
		tverts[1] = 0.0;
		tverts[2] = 0.0;
		tverts[3] = 0.0;
		tverts[4] = 1.0;
		tverts[5] = 0.0;
		tverts[6] = 1.0;
		tverts[7] = 0.0;
		tverts[8] = 0.0;
		for (int i = 0;i < (numtvfaces * 3); i++)
		{
			tvfaces[i] = (i % 3);
		}
	}
	memset(facespermat, 0, (1000 * sizeof(int)));
	short int *matid;
	for (int i = 0;i < numfaces;i++)
	{
		//matid = ((short int*) &((char*) faces)[i * 20 + 18]);
		matid = &((short int*) faces)[i * 10 + 9];
		matid[0] %= nummats;
		facespermat[matid[0]] += 1;
		if (i < 10)
		{
			//cout << "MatID Face " << (i + 1) << ": " << matid[0] << endl;
		}
	}
	//cout << endl;
	for (int i = 0;i < nummats;i++)
	{
		//cout << facespermat[i] << " Faces with mat " << (i + 1) << endl;
	}
	//cout << endl;
	//cout << "Num Mats: " << this->nummats << endl;
	//cout << "Numverts: " << numverts << endl;
	//cout << "Numfaces: " << numfaces << endl;
	//cout << "Numtverts: " << numtverts << endl;
	//cout << "Verts at " << rv << endl;
	//cout << "Faces at " << rf << endl;
	//cout << "TVerts at " << rtv << endl;
	//cout << "TVFaces at " << rtf << endl;

	if (angle >= 0)
	{
		calcvnormalsbyangle(angle);
	}
	else
	{
		calcvnormalsbysgs();
	}
}

mesh::~mesh(void)
{
	delete[] verts;
	delete[] faces;
	delete[] tverts;
	delete[] tvfaces;
	delete[] vnormals;
	delete[] vnfaces;
}

int mesh::getnumverts(void)
{
	return numverts;
}

int mesh::getnumfaces(void)
{
	return numfaces;
}

int mesh::getnumtverts(void)
{
	return numtverts;
}

int mesh::getnumtvfaces(void)
{
	return numtvfaces;
}

float* mesh::getvert(int num)
{
	return &verts[num * 3];
}

int* mesh::getface(int num)
{
	return &faces[num * 5];
}

float* mesh::gettvert(int num)
{
	return &tverts[num * 3];
}

int* mesh::gettvface(int num)
{
	return &tvfaces[num * 3];
}

void mesh::getfnormal(float *normal, int facenum)
{
	int *f = &faces[facenum * 5];
	float *v1 = &verts[f[0] * 3];
	float *v2 = &verts[f[1] * 3];
	float *v3 = &verts[f[2] * 3];
	normal[0] = ((v1[2] - v2[2]) * (v3[1] - v2[1])) - ((v1[1] - v2[1]) * (v3[2] - v2[2]));
    normal[1] = ((v1[0] - v2[0]) * (v3[2] - v2[2])) - ((v1[2] - v2[2]) * (v3[0] - v2[0]));
	normal[2] = ((v1[1] - v2[1]) * (v3[0] - v2[0])) - ((v1[0] - v2[0]) * (v3[1] - v2[1]));
	normalize(normal);
}

void mesh::getftangent(float *tangent, int facenum, int *isrighthanded)
{
	float normal[3];
	float cross[3];
	float binormal[3];
	int *f = &faces[facenum * 5];
	int *tf = &tvfaces[facenum * 3];
	float *v1 = &verts[f[0] * 3];
	float *v2 = &verts[f[1] * 3];
	float *v3 = &verts[f[2] * 3];
	float *tv1 = &tverts[tf[0] * 3];
	float *tv2 = &tverts[tf[1] * 3];
	float *tv3 = &tverts[tf[2] * 3];
	float dv1 = tv1[1] - tv2[1];
	float dv2 = tv3[1] - tv2[1];
	float du1 = tv1[0] - tv2[0];
	float du2 = tv3[0] - tv2[0];
	float edge1[3]; 
	float edge2[3];
	
	edge1[0] = v1[0] - v2[0];
	edge1[1] = v1[1] - v2[1];
	edge1[2] = v1[2] - v2[2];
	edge2[0] = v3[0] - v2[0];
	edge2[1] = v3[1] - v2[1];
	edge2[2] = v3[2] - v2[2];
	
	tangent[0] = (dv2 * edge1[0]) - (dv1 * edge2[0]);
	tangent[1] = (dv2 * edge1[1]) - (dv1 * edge2[1]);
	tangent[2] = (dv2 * edge1[2]) - (dv1 * edge2[2]);

	binormal[0] = (du2 * edge1[0]) - (du1 * edge2[0]);
	binormal[1] = (du2 * edge1[1]) - (du1 * edge2[1]);
	binormal[2] = (du2 * edge1[2]) - (du1 * edge2[2]);
	
	normalize(tangent);
	normalize(binormal);

	cross[0] = ((tangent[1] * binormal[2]) - (tangent[2] * binormal[1]));
	cross[1] = ((tangent[2] * binormal[0]) - (tangent[0] * binormal[2]));
	cross[2] = ((tangent[0] * binormal[1]) - (tangent[1] * binormal[0]));

	getfnormal(normal, facenum);

	if ((cross[0] * normal[0] + cross[1] * normal[1] + cross[2] * normal[2]) >= 0)
	{
		isrighthanded[0] = 1;
	}
	else
	{
		isrighthanded[0] = 0;
	}
}

void mesh::getftangent(float *tangent, int facenum)
{
	int *f = &faces[facenum * 5];
	int *tf = &tvfaces[facenum * 3];
	float *v1 = &verts[f[0] * 3];
	float *v2 = &verts[f[1] * 3];
	float *v3 = &verts[f[2] * 3];
	float *tv1 = &tverts[tf[0] * 3];
	float *tv2 = &tverts[tf[1] * 3];
	float *tv3 = &tverts[tf[2] * 3];
	float dv1 = tv1[1] - tv2[1];
	float dv2 = tv3[1] - tv2[1];
	float edge1[3]; 
	float edge2[3];
	
	edge1[0] = v1[0] - v2[0];
	edge1[1] = v1[1] - v2[1];
	edge1[2] = v1[2] - v2[2];
	edge2[0] = v3[0] - v2[0];
	edge2[1] = v3[1] - v2[1];
	edge2[2] = v3[2] - v2[2];
	
	tangent[0] = (dv2 * edge1[0]) - (dv1 * edge2[0]);
	tangent[1] = (dv2 * edge1[1]) - (dv1 * edge2[1]);
	tangent[2] = (dv2 * edge1[2]) - (dv1 * edge2[2]);
	
	normalize(tangent);
}

void mesh::orth(float *tan, float *norm, float *newtan)
{
	float dp = ((tan[0] * norm[0]) + (tan[1] * norm[1]) + (tan[2] * norm[2]));
	newtan[0] = (tan[0] - (norm[0] * dp));
	newtan[1] = (tan[1] - (norm[1] * dp));
	newtan[2] = (tan[2] - (norm[2] * dp));
	normalize(newtan);
}

void mesh::normalize(float *vec)
{
	float nlen = sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2));
	if (nlen == 0.0)
	{
		nlen = 1.0;
		vec[0] = 1;
	}
	vec[0] /= nlen;
	vec[1] /= nlen;
	vec[2] /= nlen;
}


int mesh::getfsgs(int facenum)
{
	return faces[facenum * 5 + 3];
}

int mesh::getfmatid(int facenum)
{
	return ((int) ((short int*) faces)[facenum * 10 + 9]);
}

float mesh::cosofangle(float *vec1, float *vec2, int isnormalized1, int isnormalized2)
{
	float d = 1;
	if (isnormalized1 == 0)
	{
		d *= sqrt(pow(vec1[0], 2) + pow(vec1[1], 2) + pow(vec1[2], 2));
	}
	if (isnormalized2 == 0)
	{
		d *= sqrt(pow(vec2[0], 2) + pow(vec2[1], 2) + pow(vec2[2], 2));
	}
	return (((vec1[0] * vec2[0]) + (vec1[1] * vec2[1]) + (vec1[2] * vec2[2])) / d);
}

void mesh::output(file *xmshfile, int matid, int *allindices, int *allindpos, int *numprintedverts, float *min, float *max)
{
	min[0] = 0; //-y
	min[1] = 0; //z
	min[2] = 0; //x
	max[0] = 0;
	max[1] = 0;
	max[2] = 0;
	struct record
	{
		int type;
		int refer;
		float x;
		float y;
		float z;
	};
	float *v;
	float *tv;
	int vertposinface = 0;
	float vertnorm[3];
	float tangent[3];
	float tangent2[3];
	int isrighthanded;
	int isrighthanded2;
	int tvert;
	int tvert2;
	int c;
	int *allind = &allindices[allindpos[0]]; 
	int numnewprintedverts = 0;
	char *container;
	//int *face;
	int fmatid;
	float *vnorm;
	int vert;
	record *rdrec;
	record *rdrec2;
	//float rdfnorm[3];
	record rec;
	twodim verts(((sizeof(record) + sizeof(int)) * 3 * facespermat[matid]), numverts);
	for (int i = 0;i < numfaces;i++)
	{
		fmatid = getfmatid(i);
		if (fmatid == matid)
		{
			rec.refer = i;
			for (int j = 0;j < 3;j++)
			{
				vert = faces[5 * i + j];
				vnorm = &vnormals[vnfaces[i * 3 + j] * 3];
				rec.x = vnorm[0];
				rec.y = vnorm[1];
				rec.z = vnorm[2];
				rec.type = j;
				verts.adddata(vert, sizeof(record), ((char *) &rec));
			}
		}
	}

	container = verts.getcontainer();
	


	for (int i = 0;i < numverts;i++)
	{
		c = verts.getcount(i);
		for (int k = 0;k < c;k++)
		{
			rdrec = ((record *) verts.getdata(i, k));
			if (rdrec->type >= 0)
			{
				vertnorm[0] = rdrec->x;
				vertnorm[1] = rdrec->y;
				vertnorm[2] = rdrec->z;
				tvert = tvfaces[3 * rdrec->refer + rdrec->type];
				getftangent(tangent, rdrec->refer, &isrighthanded);
				for (int l = (k + 1);l < c;l++)
				{
					rdrec2 = ((record *) verts.getdata(i, l));
					if (rdrec2->type >= 0)
					{
						tvert2 = tvfaces[3 * rdrec2->refer + rdrec2->type];
						if ((abs(tverts[3 * tvert] - tverts[3 * tvert2]) < 0.00001) && (abs(tverts[3 * tvert + 1] - tverts[3 * tvert2 + 1]) < 0.00001))
						//if (1 == 1)//
						{
							if (cosofangle(vertnorm,&rdrec2->x,1,1) > 0.98)
							//if (1 == 1)//
							{
								getftangent(tangent2, rdrec2->refer, &isrighthanded2);
								//if ((isrighthanded == isrighthanded2) && (cosofangle(tangent, tangent2, 1, 1) > 0.6))
								if (1 == 1)//
								{
									if (rdrec->type < 3)
									{
										if (isrighthanded)
										{
											rdrec->type = 101;
										}
										else
										{
											rdrec->type = 100;
										}
										rdrec2->refer = rdrec->refer;
										rdrec->refer = verts.getcontainerpos(i, l);
										rdrec->x = tangent[0];
										rdrec->y = tangent[1];
										rdrec->z = tangent[2];
									}
									rdrec2->type = ((verts.getcontainerpos(i, k)) * (-1) - 1);
									rdrec->x += tangent2[0];
									rdrec->y += tangent2[1];
									rdrec->z += tangent2[2];
								}
							}
						}
					}
				}
				if (rdrec->type < 3)
				{
					rdrec->type = isrighthanded;
				}
			}
		}
	}
	for (int i = 0;i < numverts;i++)//
	{
		if (verts.getcount(i) == 0)
		{
			i = i;
		}
	}
	for (int i = 0;i < 3 * facespermat[matid];i++)
	{
		rdrec = ((record *) &container[i * (sizeof(record) + sizeof(int)) + sizeof(int)]);
		if (rdrec->type < 0)
		{
			rdrec2 = ((record *) &container[rdrec->type * (-1) - 1 + sizeof(int)]);
			allind[i] = rdrec2->refer;
		}
		else
		{
			if (rdrec->type > 1)
			{
				rdrec2 = ((record *) &container[rdrec->refer + sizeof(int)]);
				normalize(&rdrec->x);
				orth(&rdrec->x, &rdrec2->x, tangent);
				v = &this->verts[3 * faces[5 * rdrec2->refer + vertposinface]];
				tv = &tverts[3 * tvfaces[3 * rdrec2->refer + vertposinface]];

				xmshfile->writefloat(v[0]);
				xmshfile->writefloat(v[2]);
				xmshfile->writefloat(v[1]);

				//min / max
				if (i == 0)
				{
					min[0] = v[0];
					min[1] = v[2];
					min[2] = v[1];
					max[0] = v[0];
					max[1] = v[2];
					max[2] = v[1];
				}
				if (v[1] < min[2])
				{
					min[2] = v[1];
				}
				if (v[0] < min[0])
				{
					min[0] = v[0];
				}
				if (v[2] < min[1])
				{
					min[1] = v[2];
				}
				if (v[1] > max[2])
				{
					max[2] = v[1];
				}
				if (v[0] > max[0])
				{
					max[0] = v[0];
				}
				if (v[2] > max[1])
				{
					max[1] = v[2];
				}
				
				xmshfile->writefloat(rdrec2->x);
				xmshfile->writefloat(rdrec2->z);
				xmshfile->writefloat(rdrec2->y);
				
				if (rdrec->type == 101)
				{
					xmshfile->writefloat(tangent[0]);
					xmshfile->writefloat(tangent[2]);
					xmshfile->writefloat(tangent[1]);
					xmshfile->writelong(4294967295); //0
				}
				else
				{
					xmshfile->writefloat(tangent[0] * (-1));
					xmshfile->writefloat(tangent[2] * (-1));
					xmshfile->writefloat(tangent[1] * (-1));
					xmshfile->writelong(4294967295); //16711680
				}
				xmshfile->writelong(4278190080);

				xmshfile->writefloat(tv[0]);
				xmshfile->writefloat(tv[1] * (-1));

				rdrec->refer = (numnewprintedverts + numprintedverts[0]);
			}
			else
			{
				getftangent(tangent, rdrec->refer);
				memcpy(tangent2, tangent, 12);
				orth(tangent2, &rdrec->x, tangent);
				v = &this->verts[3 * faces[5 * rdrec->refer + vertposinface]];
				tv = &tverts[3 * tvfaces[3 * rdrec->refer + vertposinface]];

				xmshfile->writefloat(v[0]);
				xmshfile->writefloat(v[2]);
				xmshfile->writefloat(v[1]);

				//min / max
				if (i == 0)
				{
					min[0] = v[0];
					min[1] = v[2];
					min[2] = v[1];
					max[0] = v[0];
					max[1] = v[2];
					max[2] = v[1];
				}
				if (v[1] < min[2])
				{
					min[2] = v[1];
				}
				if (v[0] < min[0])
				{
					min[0] = v[0];
				}
				if (v[2] < min[1])
				{
					min[1] = v[2];
				}
				if (v[1] > max[2])
				{
					max[2] = v[1];
				}
				if (v[0] > max[0])
				{
					max[0] = v[0];
				}
				if (v[2] > max[1])
				{
					max[1] = v[2];
				}
				
				/*
				xmshfile->writefloat(rdrec->x);
				xmshfile->writefloat(rdrec->z);
				xmshfile->writefloat(rdrec->y);
				*/

				xmshfile->writefloat(0.0);
				xmshfile->writefloat(1.0);
				xmshfile->writefloat(0.0);
				
				if (rdrec->type == 1)
				{
					xmshfile->writefloat(tangent[0]);
					xmshfile->writefloat(tangent[2]);
					xmshfile->writefloat(tangent[1]);
					xmshfile->writelong(4294967295); //0
				}
				else
				{
					xmshfile->writefloat(tangent[0] * (-1));
					xmshfile->writefloat(tangent[2] * (-1));
					xmshfile->writefloat(tangent[1] * (-1));
					xmshfile->writelong(4294967295); //16711680
				}
				xmshfile->writelong(4278190080);

				xmshfile->writefloat(tv[0]);
				xmshfile->writefloat(tv[1] * (-1));	
			}
			allind[i] = (numnewprintedverts + numprintedverts[0]);
			numnewprintedverts++;
		}
		vertposinface++;
		if (vertposinface == 3)
		{
			vertposinface = 0;
		}	
	}
	allindpos[0] += (3 * facespermat[matid]);
	numprintedverts[0] += numnewprintedverts;
}

void mesh::calcvnormalsbysgs(void)
{
	//weighted by angle
	numvnormals = numfaces * 3;
	vnormals = new float [numfaces * 9];
	float *fnormals = new float [numfaces * 3];
	float *fnweights = new float [numfaces * 3]; //weighting for each vert in the face
	initncalc(fnormals, fnweights); //fills fnormals, fnweights and vnormals
	twodim facesatvert((numfaces * sizeof(int) * 2 * 3), numverts);
	for (int i = 0;i < numfaces;i++)
	{
		int *fc = &faces[5 * i];
		for (int j = 0;j < 3;j++)
		{
			int vert = fc[j];
			int c = facesatvert.getcount(vert);
			int data = (i * 3 + j);
			for (int k = 0;k < c;k++)
			{
				int val = facesatvert.getint(vert, k);
				int f = val / 3;
				if (getfsgs(i) & getfsgs(f))
				{
					int vp = val % 3;
					float *ivn = &vnormals[9 * i + 3 * j];
					float *fvn = &vnormals[9 * f + 3 * vp];
					fvn[0] += (fnormals[3 * i + 0] * fnweights[data]);
					fvn[1] += (fnormals[3 * i + 1] * fnweights[data]);
					fvn[2] += (fnormals[3 * i + 2] * fnweights[data]);
					ivn[0] += (fnormals[3 * f + 0] * fnweights[val]);
					ivn[1] += (fnormals[3 * f + 1] * fnweights[val]);
					ivn[2] += (fnormals[3 * f + 2] * fnweights[val]);
				}
			}
			facesatvert.addint(vert, data);
		}
	}
	int border = 3 * numfaces;
	for (int i = 0;i < border;i++)
	{
		normalize(&vnormals[3 * i]);
	}
	fillvnfaces();
	delete[] fnormals;
	delete[] fnweights;
}

void mesh::calcvnormalsbyangle(float angle)
{
	float mc = cos(angle / 180.0 * 3.141592654); //minimal cosinus
	//weighted by angle
	numvnormals = numfaces * 3;
	vnormals = new float [numfaces * 9];
	float *fnormals = new float [numfaces * 3];
	float *fnweights = new float [numfaces * 3]; //weighting for each vert in the face
	initncalc(fnormals, fnweights); //fills fnormals, fnweights and vnormals
	twodim facesatvert((numfaces * sizeof(int) * 2 * 3), numverts);
	for (int i = 0;i < numfaces;i++)
	{
		int *fc = &faces[5 * i];
		for (int j = 0;j < 3;j++)
		{
			int vert = fc[j];
			int c = facesatvert.getcount(vert);
			int data = (i * 3 + j);
			for (int k = 0;k < c;k++)
			{
				int val = facesatvert.getint(vert, k);
				int f = val / 3;
				if (cosofangle(&fnormals[3 * i], &fnormals[3 * f], 1, 1) > mc)
				{
					int vp = val % 3;
					float *ivn = &vnormals[9 * i + 3 * j];
					float *fvn = &vnormals[9 * f + 3 * vp];
					fvn[0] += (fnormals[3 * i + 0] * fnweights[data]);
					fvn[1] += (fnormals[3 * i + 1] * fnweights[data]);
					fvn[2] += (fnormals[3 * i + 2] * fnweights[data]);
					ivn[0] += (fnormals[3 * f + 0] * fnweights[val]);
					ivn[1] += (fnormals[3 * f + 1] * fnweights[val]);
					ivn[2] += (fnormals[3 * f + 2] * fnweights[val]);
				}
			}
			facesatvert.addint(vert, data);
		}
	}
	int border = 3 * numfaces;
	for (int i = 0;i < border;i++)
	{
		normalize(&vnormals[3 * i]);
	}
	fillvnfaces();
	delete[] fnormals;
	delete[] fnweights;
}

void mesh::initncalc(float *fnormals, float *fnweights)
{
	for (int i = 0;i < numfaces;i++)
	{
		float *fn = &fnormals[3 * i];
		getfnormal(fn, i);
		//memcpy(&vnormals[9 * i + 0], &fnormals[3 * i], 12); -> only if there were no weights
		//memcpy(&vnormals[9 * i + 3], &fnormals[3 * i], 12);
		//memcpy(&vnormals[9 * i + 6], &fnormals[3 * i], 12);
		
		//calc weights
		int *f = &faces[5 * i];
		float *va = &verts[3 * f[0]];
		float *vb = &verts[3 * f[1]];
		float *vc = &verts[3 * f[2]];
		float sa[3];
		float sb[3];
		float sc[3];
		sa[0] = vc[0] - vb[0];
		sa[1] = vc[1] - vb[1];
		sa[2] = vc[2] - vb[2];
		sb[0] = va[0] - vc[0];
		sb[1] = va[1] - vc[1];
		sb[2] = va[2] - vc[2];
		sc[0] = vb[0] - va[0];
		sc[1] = vb[1] - va[1];
		sc[2] = vb[2] - va[2];
		normalize(sa);
		normalize(sb);
		normalize(sc);
		float *mod = &fnweights[3 * i];
		mod[0] = acos((-1) * cosofangle(sb, sc, 1, 1)); //each time 1 vector is the
		mod[1] = acos((-1) * cosofangle(sc, sa, 1, 1)); //wrong way round so it's 
		mod[2] = acos((-1) * cosofangle(sa, sb, 1, 1)); //necessary to multiply by -1

		//set initial vertex normals
		vnormals[9 * i + 0] = fn[0] * mod[0];
		vnormals[9 * i + 1] = fn[1] * mod[0];
		vnormals[9 * i + 2] = fn[2] * mod[0];
		vnormals[9 * i + 3] = fn[0] * mod[1];
		vnormals[9 * i + 4] = fn[1] * mod[1];
		vnormals[9 * i + 5] = fn[2] * mod[1];
		vnormals[9 * i + 6] = fn[0] * mod[2];
		vnormals[9 * i + 7] = fn[1] * mod[2];
		vnormals[9 * i + 8] = fn[2] * mod[2];
	}
}

void mesh::fillvnfaces(void)
{
	int border = 3 * numfaces;
	for (int i = 0;i < border;i++)
	{
		vnfaces[i] = i;
	}
}
