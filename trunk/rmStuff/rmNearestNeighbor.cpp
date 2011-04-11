#include "rmNearestNeighbor.h"

rmOut rmNearestNeighbor::out = rmOut("rmNearestNeighbor.cpp");

rmU32 * rmNearestNeighbor::foundAt;
rmNearestNeighbor::LookUpPoint * rmNearestNeighbor::points;
rmNearestNeighbor::LookUpPoint * rmNearestNeighbor::endPoints;
rmNearestNeighbor::LookUpCube * rmNearestNeighbor::cubes;
rmNearestNeighbor::LookUpCube * rmNearestNeighbor::curCube;
vector<rmNearestNeighbor::FoundPoint> rmNearestNeighbor::foundPoints;

rmU64 * rmNearestNeighbor::buffer = 0;
rmU64 * rmNearestNeighbor::buffer2 = 0;
rmU64 * rmNearestNeighbor::searchLevel[4] = {0, 0, 0, 0};

rmU64 rmNearestNeighbor::first[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

rmU32 rmNearestNeighbor::system3[3] = {9, 3, 1};
rmU32 rmNearestNeighbor::system4[3] = {16, 4, 1};
rmU32 rmNearestNeighbor::system5[3] = {25, 5, 1};

rmU32 rmNearestNeighbor::systemCur[3];
rmU32 rmNearestNeighbor::extentsCur[3];

rmU64 rmNearestNeighbor::tmp;

rmU64 rmNearestNeighbor::save;
rmU64 rmNearestNeighbor::keep;
rmU32 rmNearestNeighbor::rotate;
rmU32 rmNearestNeighbor::backrotate;
rmU32 rmNearestNeighbor::next;

rmU64 * rmNearestNeighbor::curBlock;
rmU64 * rmNearestNeighbor::curLevel;

rmU32 rmNearestNeighbor::x;
rmU32 rmNearestNeighbor::y;
rmU32 rmNearestNeighbor::z;

rmU32 rmNearestNeighbor::maxX;
rmU32 rmNearestNeighbor::maxY;
rmU32 rmNearestNeighbor::maxZ;

string rmNearestNeighbor::toString(rmU64 & src)
{
	string result;
	result.resize(64);

	rmI64 tmp = 1;

	for (rmU32 i = 0; i != 64; ++i) {
		result[i] = (48 + (1 && (src & tmp)));
		tmp <<= 1;
	}

	return result;
}

void rmNearestNeighbor::printToObj(const rmU64 & src)
{
	rmOutFileStream obj("C:\\aNearestNeighborBlock.obj");
	rmScene s;
	rmNode n;
	rmMesh m;
	
	s.addNode(&n);
	m.setNumVerts(64);
	
	tmp = 1;
	rmU32 i = 0;

	for (rmU32 xx = 0; xx != 4; ++xx) {
		for (rmU32 yy = 0; yy != 4; ++yy) {
			for (rmU32 zz = 0; zz != 4; ++zz) {
				if (src & tmp) {
					m.setVert(i, rmPoint3((static_cast<rmF32>(xx) * 10 + 5), (static_cast<rmF32>(yy) * 10 + 5), (static_cast<rmF32>(zz) * 10 + 5)));
				}
				tmp <<= 1;
				++i;
			}
		}
	}

	s.getNode(0).setMesh(m);
	rmObjWriter::writeSceneToObjFile(&obj, &s, 0);
}

void rmNearestNeighbor::printToObj(rmU64 * src, rmU32 iSystem)
{
	rmOutFileStream obj("C:\\aNearestNeighborBlock.obj");
	rmScene s;
	rmNode n;
	rmMesh m;
	rmU32 i = 0;
	
	rmU32 oldSystem[3];
	rmU32 oldExtents[3];
	oldSystem[0] = systemCur[0];
	oldSystem[1] = systemCur[1];
	oldSystem[2] = systemCur[2];
	oldExtents[0] = extentsCur[0];
	oldExtents[1] = extentsCur[1];
	oldExtents[2] = extentsCur[2];

	rmU32 * sys = (iSystem == 3) ? system3 : system5;
	extentsCur[0] =  extentsCur[1] =  extentsCur[2] = iSystem;
	systemCur[0] = sys[0];
	systemCur[1] = sys[1];
	systemCur[2] = sys[2];

	s.addNode(&n);
	m.setNumVerts(extentsCur[0] * extentsCur[1] * extentsCur[2] * 64);

	curBlock = src;
	init3dLoop(extentsCur[0], extentsCur[1], extentsCur[2]);
	while (curBlock != 0) {
		tmp = 1;
		for (rmU32 xx = 0; xx != 4; ++xx) {
			for (rmU32 yy = 0; yy != 4; ++yy) {
				for (rmU32 zz = 0; zz != 4; ++zz) {
					if (*curBlock & tmp) {
						m.setVert(i, rmPoint3((static_cast<rmF32>(xx + 4 * x) * 10 + 5), (static_cast<rmF32>(yy + 4 * y) * 10 + 5), (static_cast<rmF32>(zz + 4 * z) * 10 + 5)));
					}
					tmp <<= 1;
					++i;
				}
			}
		}
		nextBlock();
	}

	systemCur[0] = oldSystem[0];
	systemCur[1] = oldSystem[1];
	systemCur[2] = oldSystem[2];
	extentsCur[0] = oldExtents[0];
	extentsCur[1] = oldExtents[1];
	extentsCur[2] = oldExtents[2];

	s.getNode(0).setMesh(m);
	rmObjWriter::writeSceneToObjFile(&obj, &s, 0);
}


void rmNearestNeighbor::initShift(rmU32 * system, rmU32 direction, rmU32 count)
{
	next = system[direction];
	save = ~first[3 * direction + 3 - count];
	keep = ~first[3 * direction + count - 1];
	rotate = count * system4[direction];
	backrotate = 4 * system4[direction] - rotate;
}

void rmNearestNeighbor::shift(void)
{	
	tmp = (*curBlock & save);
	tmp >>= backrotate;
	*curBlock = ((*curBlock << rotate) & keep);
	if (tmp != 0) {
		curBlock[next] |= tmp;
	}
}

void rmNearestNeighbor::init3dLoop(rmU32 xCount, rmU32 yCount, rmU32 zCount)
{
	x = y = z = 0;
	maxX = xCount;
	maxY = yCount;
	maxZ = zCount;
}

void rmNearestNeighbor::nextBlock(void)
{
	++z;
	++curBlock;
	if (z == maxZ) {
		z = 0;
		curBlock += (extentsCur[2] - maxZ);
		++y;
		if (y == maxY) {
			y = 0;
			curBlock += ((extentsCur[1] - maxY) * systemCur[1]);
			++x;
			if (x == maxX) {
				curBlock = 0;
			}
		}
	}
}

void rmNearestNeighbor::nextCube(void)
{
	++z;
	++curCube;
	if (z == maxZ) {
		z = 0;
		curCube += (extentsCur[2] - maxZ);
		++y;
		if (y == maxY) {
			y = 0;
			curCube += ((extentsCur[1] - maxY) * systemCur[1]);
			++x;
			if (x == maxX) {
				curCube = 0;
			}
		}
	}
}

void rmNearestNeighbor::init(void)
{
	if (buffer != 0) {
		return;
	}

	tmp = 1;
	for (rmU32 i = 0; i != 15; ++i) {
		tmp = ((tmp << 1) | 1);
	}
	first[0] = tmp; // 1111111111111111000000000000000000000000000000000000000000000000
	first[1] = ((first[0] << 16) | tmp);
	first[2] = ((first[1] << 16) | tmp);

	tmp >>= 12;
	for (rmU32 i = 0; i != 3; ++i) {
		tmp = ((tmp << 16) | tmp);
	}
	first[3] = tmp; // 1111000000000000111100000000000011110000000000001111000000000000
	first[4] = ((first[3] << 4) | tmp);
	first[5] = ((first[4] << 4) | tmp);

	tmp = 1;
	for (rmU32 i = 0; i != 15; ++i) {
		tmp = ((tmp << 4) | 1);
	}
	first[6] = tmp; // 1000100010001000100010001000100010001000100010001000100010001000
	first[7] = ((first[6] << 1) | tmp);
	first[8] = ((first[7] << 1) | tmp);

	rmF32 min = -SECURITY_PUFFER_0;
	rmF32 max = 2 + min;

	buffer = new rmU64 [304];
	memset(buffer, 0, 304 * sizeof(rmU64));
	buffer2 = new rmU64 [125];
	memset(buffer2, 0, 125 * sizeof(rmU64));
	rmU64 curLevel[125];

	rmU64 * inBuffer = buffer;
	rmU64 ** inSearchLevel = searchLevel; 

	systemCur[0] = 25;
	systemCur[1] = 5;
	systemCur[2] = 1;
	extentsCur[0] = 5;
	extentsCur[1] = 5;
	extentsCur[2] = 5;

	for (rmU32 h = 0; h != 4; ++h) {
		memset(curLevel, 0, 125 * sizeof(rmU64));
		curBlock = curLevel;
		init3dLoop(2, 2, 2);

		while (curBlock != 0) {
			tmp = 1;
			for (rmU32 xx = 0; xx != 4; ++xx) {
				for (rmU32 yy = 0; yy != 4; ++yy) {
					for (rmU32 zz = 0; zz != 4; ++zz) {
						rmF32 len = rmVec3(static_cast<rmF32>(7 - (4 * x + xx)), static_cast<rmF32>(7 - (4 * y + yy)), static_cast<rmF32>(7 - (4 * z + zz))).magnitude();
						if ((len >= min) && (len < max)) {
							*curBlock |= tmp;
						}
						tmp <<= 1;
					}
				}
			}
			nextBlock();
		}

		rmU32 counts[3] = {2, 2, 2};
		rmU32 * pos[3];
		pos[0] = &x;
		pos[1] = &y;
		pos[2] = &z;

		for (rmU32 i = 0; i != 3; ++i) {
			rmU32 offset = system4[i];
			rmU64 filter = first[3 * i];

			curBlock = curLevel;
			init3dLoop(counts[0], counts[1], counts[2]);

			while (curBlock != 0) {
				rmU64 & b = *curBlock;
				tmp = 0;

				tmp |= ((b & filter) << (3 * offset));
				filter <<= offset;
				tmp |= ((b & filter) << (1 * offset));
				filter <<= offset;
				tmp |= ((b & filter) >> (1 * offset));
				filter <<= offset;
				tmp |= ((b & filter) >> (3 * offset));
				filter >>= (3 * offset);

				if (*pos[i] == 0) {
					curBlock[system5[i] * 3] = tmp;
				}
				else {
					curBlock[system5[i]] = tmp;
				}


				nextBlock();
			}

			counts[i] = 4;
		}

		rmU64 tmp2;
		counts[0] = counts[1] = counts[2] = 5;
		for (rmU32 i = 0; i != 3; ++i) {
			rmU64 & filter = first[3 * i];
			counts[i] = 3;
			initShift(system5, i, 1);
			curBlock = (curLevel + 124);
			for (rmU32 xx = counts[0]; xx != 0; --xx) {
				for (rmU32 yy = counts[1]; yy != 0; --yy) {
					for (rmU32 zz = counts[2]; zz != 0; --zz) {
						tmp2 = (*curBlock & filter);
						shift();
						*curBlock |= tmp2;
						--curBlock;
					}
					if (i == 2) {
						curBlock -= 2;
					}
				}
				if (i == 1) {
					curBlock -= 10;
				}
			}
			counts[i] = 5;
		}

		min += 2;
		max += 2;

		*(inSearchLevel++) = inBuffer;
		if (h < 2) {
			curBlock = curLevel + 25 + 5 + 1;
			init3dLoop(3, 3, 3);
			while (curBlock != 0) {
				*(inBuffer++) = *curBlock;
				nextBlock();
			}
		}
		else {
			memcpy(inBuffer, curLevel, 125 * sizeof(rmU64));
			inBuffer += 125;
		}
	}
}

void rmNearestNeighbor::searchPoints(LookUpPoint * p)
{
	rmU32 nFoundPoints = 0;
	FoundPoint fp;

	if (p->nLevelsSearched < 4) {
		rmU32 * system = (p->nLevelsSearched < 2) ? system3 : system5;
		rmU32 edgeLenght = system[1];
		rmU32 size = (edgeLenght * edgeLenght * edgeLenght);
		rmU32 offset = (systemCur[0] + systemCur[1] + systemCur[2]) * ((edgeLenght == 5) ? 2 : 1);
		curBlock = buffer2;

		memcpy(buffer2, searchLevel[p->nLevelsSearched], size * sizeof(rmU64));
		for (rmU32 i = 0; i != 3; ++i) {
			if (p->xyzInCube[i] != 0) {
				initShift(system, i, p->xyzInCube[i]);
				for (curBlock = buffer2 + size; curBlock != buffer2;) {
					--curBlock;
					shift();
				}
			}
		}

		curCube = (cubes + p->iCube - offset);
		init3dLoop(edgeLenght, edgeLenght, edgeLenght);
		while (curCube != 0) {
			if ((curCube->pointSpread & *curBlock) != 0) {
				for (LookUpPoint ** cp = curCube->points + curCube->nPoints; cp != curCube->points;) {
					--cp;
					if (((**cp).location & *curBlock) != 0) {
						if (*cp != p) {
							fp.point = *cp;
							fp.distance = (*((**cp).pos) - *(p->pos)).magnitude();
							rmU32 & fa = foundAt[fp.point - points];
							if (fa != (-1)) {
								FoundPoint * fp2 = &foundPoints[fa];
								if (fp2->distance > fp.distance) {
									if (fp2[fp2->offsetToNext].offsetToNext != 0) {
										foundAt[fp2[fp2->offsetToNext].point - points] = (fp2 - &foundPoints[0]);
										fp2[fp2->offsetToNext].offsetToNext += fp2->offsetToNext;
									}
									*fp2 = fp2[fp2->offsetToNext];
									foundPoints.push_back(fp);
									++nFoundPoints;
								}
							}
							else {
								foundPoints.push_back(fp);
								++nFoundPoints;
							}
						}
					}
				}
			}
			++curBlock;
			nextCube();
		}
	}
	else {
		for (LookUpPoint * p2 = points; p2 != endPoints; ++p2) {
			if (p2 != p) {
				fp.point = p2;
				fp.distance = (*(p2->pos) - *(p->pos)).magnitude();
				rmU32 & fa = foundAt[fp.point - points];
				if (fa != (-1)) {
					FoundPoint * fp2 = &foundPoints[fa];
					if (fp2->distance > fp.distance) {
						if (fp2[fp2->offsetToNext].offsetToNext != 0) {
							foundAt[fp2[fp2->offsetToNext].point - points] = (fp2 - &foundPoints[0]);
							fp2[fp2->offsetToNext].offsetToNext += fp2->offsetToNext;
						}
						*fp2 = fp2[fp2->offsetToNext];
						foundPoints.push_back(fp);
						++nFoundPoints;
					}
				}
				else {
					foundPoints.push_back(fp);
					++nFoundPoints;
				}
			}
		}
	}

	if (nFoundPoints != 0) {
		rmI32 offset;
		FoundPoint * oldP = &foundPoints[0];
		FoundPoint * newP = &foundPoints[foundPoints.size() - nFoundPoints];
		FoundPoint * endP = newP + nFoundPoints;
		sort((foundPoints.end() - nFoundPoints), foundPoints.end(), isInRightOrder);
		rmU32 i = newP - oldP;
		for (; newP != endP; ++newP) {
			foundAt[newP->point - points] = i++;
			while (newP->distance > oldP[oldP->offsetToNext].distance) {
				rmI32 dd = oldP - &foundPoints[0];
				oldP += oldP->offsetToNext;
			}
			offset = (newP - oldP);
			newP->offsetToNext = oldP->offsetToNext - offset;
			oldP->offsetToNext = offset;
			oldP = newP;
		}
	}

	++(p->nLevelsSearched);
}

void rmNearestNeighbor::findNearestTargetVerts(const rmPoint3 * vertsToMatch, const rmU32 nVertsToMatch, const rmPoint3 * targetVerts, const rmU32 nTargetVerts, vector<rmU32> & dest)
{
	if (nTargetVerts == 0) {
		OUT_SERR("Got no target verts")
	}
	if (nVertsToMatch == 0) {
		OUT_SERR("Got no verts to match")
	}

	init();

	rmU32 nPoints = (nVertsToMatch + nTargetVerts);

	rmPoint3 min = *targetVerts;
	rmPoint3 max = *targetVerts;

	const rmPoint3 * v = vertsToMatch;
	const rmPoint3 * end = v + nVertsToMatch;
	for (; v != end; ++v) {
		if (v->x < min.x) {
			min.x = v->x;
		}
		else if (v->x > max.x) {
			max.x = v->x;
		}
		if (v->y < min.y) {
			min.y = v->y;
		}
		else if (v->y > max.y) {
			max.y = v->y;
		}
		if (v->z < min.z) {
			min.z = v->z;
		}
		else if (v->z > max.z) {
			max.z = v->z;
		}
	}

	v = targetVerts;
	end = v + nTargetVerts;
	for (; v != end; ++v) {
		if (v->x < min.x) {
			min.x = v->x;
		}
		else if (v->x > max.x) {
			max.x = v->x;
		}
		if (v->y < min.y) {
			min.y = v->y;
		}
		else if (v->y > max.y) {
			max.y = v->y;
		}
		if (v->z < min.z) {
			min.z = v->z;
		}
		else if (v->z > max.z) {
			max.z = v->z;
		}
	}

	rmVec3 sec = max - min;
	sec *= SECURITY_PUFFER_1;
	min = max - sec;
	sec *= SECURITY_PUFFER_1;
	max = min + sec;

	rmVec3 extents = max - min;

	rmF32 * a, * b, * c, * p1, * p2, * p3;
	p1 = (extents.y > extents.z) ? &extents.y : &extents.z;
	p2 = (extents.z < extents.x) ? &extents.z : &extents.x;
	p3 = (extents.x > extents.y) ? &extents.x : &extents.y;
	a = (extents.x > *p1) ? &extents.x : p1;
	b = (*p1 < *p3) ? p1 : p3;
	c = (extents.y < *p2) ? &extents.y : p2;

	*a = (*a == 0) ? 1.0 : *a;
	*b = std::max(*b, ((*a) / nPoints));
	*c = std::max(*c, ((*a) / nPoints));
	*c = std::max(*c, static_cast<rmF32>(pow(static_cast<rmF64>((*a) * (*b) / nPoints), 0.5)));

	rmF32 volumePerVert = (extents.x * extents.y * extents.z / static_cast<rmF32>((nTargetVerts + nVertsToMatch) / 2)); 
	rmF32 edgeLength = pow(volumePerVert, static_cast<rmF32>(1.0 / 3.0));
	rmF32 edgeLength_4 = ((edgeLength / 4) * SECURITY_PUFFER_1);
	rmF32 searchLevelRadiusDelta = ((edgeLength / 2) / SECURITY_PUFFER_1);

	extentsCur[0] = static_cast<rmU32>((extents.x / edgeLength) + 1.0) + 4;
	extentsCur[1] = static_cast<rmU32>((extents.y / edgeLength) + 1.0) + 4;
	extentsCur[2] = static_cast<rmU32>((extents.z / edgeLength) + 1.0) + 4;
	systemCur[2] = 1;
	systemCur[1] = extentsCur[2];
	systemCur[0] = extentsCur[2] * extentsCur[1];

	points = new LookUpPoint [nPoints];
	foundAt = new rmU32 [nPoints];
	LookUpPoint ** pointPointers = new LookUpPoint * [nPoints];
	LookUpPoint * pointsToProcess = (points + nTargetVerts);
	endPoints = points + nPoints;
	memset(points, 0, (sizeof(LookUpPoint) * nPoints));
	
	for (LookUpPoint * p = points; p != pointsToProcess; ++p) {
		p->next = p;
	}

	cubes = new LookUpCube [extentsCur[0] * extentsCur[1] * extentsCur[2]];
	LookUpCube * endCubes = cubes + (extentsCur[0] * extentsCur[1] * extentsCur[2]);
	memset(cubes, 0, (sizeof(LookUpCube) * (endCubes - cubes)));

	rmPoint3 v2;
	rmVec3 offset = rmPoint3(2 * edgeLength, 2 * edgeLength, 2 * edgeLength) - min;

	v = targetVerts;
	for (LookUpPoint * p = points; p != endPoints; ++p) {
		if (p == pointsToProcess) {
			v = vertsToMatch;
		}
		v2 = *v + offset;

		p->pos = v;
		p->iCube += static_cast<rmU32>(v2.x / edgeLength) * systemCur[0];
		p->iCube += static_cast<rmU32>(v2.y / edgeLength) * systemCur[1];
		p->iCube += static_cast<rmU32>(v2.z / edgeLength) * systemCur[2];
		++cubes[p->iCube].nPoints;

		p->xyzInCube[0] = static_cast<rmU8>(fmod(v2.x, edgeLength) / edgeLength_4);
		p->xyzInCube[1] = static_cast<rmU8>(fmod(v2.y, edgeLength) / edgeLength_4);
		p->xyzInCube[2] = static_cast<rmU8>(fmod(v2.z, edgeLength) / edgeLength_4);
		
		tmp = 1;
		tmp <<= p->xyzInCube[0] * system4[0];
		tmp <<= p->xyzInCube[1] * system4[1];
		tmp <<= p->xyzInCube[2] * system4[2];
		p->location = tmp;
		cubes[p->iCube].pointSpread |= tmp;
		
		++v;
	}

	LookUpPoint ** pp = pointPointers;
	for (LookUpCube * c = cubes; c != endCubes; ++c) {
		c->points = pp;
		pp += c->nPoints;
	}

	for (LookUpPoint * p = points; p != endPoints; ++p) {
		*(cubes[p->iCube].points++) = p;
	}
	for (LookUpCube * c = cubes; c != endCubes; ++c) {
		c->points -= c->nPoints;
	}

	foundPoints.resize(2);
	foundPoints[0].distance = -1.0;
	foundPoints[0].offsetToNext = 1;
	foundPoints[1].distance = 9999999999999999999999.0;
	foundPoints[1].offsetToNext = 0;

	/*
	memset(foundAt, 255, (sizeof(rmU32) * nPoints));
	FoundPoint * fp;
	LookUpPoint * p = pointsToProcess + 2;
	for (rmU32 i = 1; i < 6; ++i) {
		OUT_MSG("Level " << i << " search for vert 3")
		searchPoints(p);
		fp = &foundPoints[0];
		while (fp->offsetToNext != 0) {
			OUT_MSG("Vert " << (fp->point - points + 1) << ": " << fp->distance)
			fp += fp->offsetToNext;
		}
	}
	*/

	LookUpPoint * pit;
	LookUpPoint dummyPoint;
	dummyPoint.nLevelsSearched = 10;
	LookUpPoint * firstGroupPoint;
	rmU8 nLevelsSearched;
	for (LookUpPoint * p = pointsToProcess; p != endPoints; ++p) {
		if (p->next == 0) {
			memset(foundAt, 255, (sizeof(rmU32) * nPoints));
			foundPoints[0].offsetToNext = 1;
			foundPoints.resize(2);
			firstGroupPoint = p;
			p->next = &dummyPoint;
			foundAt[p - points] = 0; // So that point can't be found.
			nLevelsSearched = 0;

			while (true) {
				FoundPoint & fp = foundPoints[foundPoints[0].offsetToNext];
				if ((fp.distance > nLevelsSearched * searchLevelRadiusDelta) && (nLevelsSearched < 5)) {
					pit = firstGroupPoint;
					nLevelsSearched = firstGroupPoint->nLevelsSearched + 1;
					while (pit->nLevelsSearched < nLevelsSearched) {
						searchPoints(pit);
						pit = pit->next;
					}
				}
				else {
					pit = fp.point;
					if (pit->next == 0) {
						foundAt[pit - points] = 0; // This way that point can't be found another time (for the current p).
						foundPoints[0].offsetToNext += fp.offsetToNext;
						pit->next = firstGroupPoint;
						firstGroupPoint = pit;
						nLevelsSearched = 0;
					}
					else {
						LookUpPoint * cache;
						while (firstGroupPoint != &dummyPoint) {
							cache = firstGroupPoint;
							firstGroupPoint = firstGroupPoint->next;
							cache->next = pit->next;
						}
						break;
					}
				}
			}
		}
	}

	dest.resize(nVertsToMatch);
	rmU32 i = 0;
	for (LookUpPoint * p = pointsToProcess; p != endPoints; ++p) {
		dest[i++] = (p->next - points);
	}

	delete [] cubes;
	delete [] points;
	delete [] pointPointers;
	delete [] foundAt;
}

rmBool rmNearestNeighbor::isInRightOrder(const FoundPoint & p1, const FoundPoint & p2)
{
	return (p1.distance < p2.distance);
}