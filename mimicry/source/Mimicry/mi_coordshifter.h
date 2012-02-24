#ifndef MI_COORDSHIFTER_H_INCLUDED
#define MI_COORDSHIFTER_H_INCLUDED

class mCCoordShifter
{
public:
    virtual ~mCCoordShifter( void ) = 0;
public:
    virtual mCFace & ShiftFaceCoords( mCFace & a_faceDest ) = 0;
    virtual mCMaxFace & ShiftFaceCoords( mCMaxFace & a_faceDest ) = 0;
    virtual mCVec3 & ShiftVectorCoords( mCVec3 & a_vecDest ) = 0;
    virtual mCVec3 & ShiftTexCoords( mCVec3 & a_vecDest ) = 0;
public:
    mCMesh & ShiftMeshCoords( mCMesh & a_meshDest );
};

class mCMaxRisenCoordShifter :
    public mCCoordShifter
{
public:
    virtual ~mCMaxRisenCoordShifter( void );
public:
    virtual mCFace & ShiftFaceCoords( mCFace & a_faceDest );
    virtual mCMaxFace & ShiftFaceCoords( mCMaxFace & a_faceDest );
    virtual mCVec3 & ShiftVectorCoords( mCVec3 & a_vecDest );
    virtual mCVec3 & ShiftTexCoords( mCVec3 & a_vecDest );
};

#endif