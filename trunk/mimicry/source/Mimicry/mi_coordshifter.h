#ifndef MI_COORDSHIFTER_H_INCLUDED
#define MI_COORDSHIFTER_H_INCLUDED

class mCCoordShifter
{
public:
    virtual ~mCCoordShifter( void ) = 0;
public:
    virtual void ShiftFaceCoords( mCFace & a_faceDest ) = 0;
    virtual void ShiftFaceCoords( mCMaxFace & a_faceDest ) = 0;
    virtual void ShiftVectorCoords( mCVec3 & a_vecDest ) = 0;
    virtual void ShiftTexCoords( mCVec3 & a_vecDest ) = 0;
public:
    void ShiftMeshCoords( mCMesh & a_meshDest );
};

class mCMaxRisenCoordShifter :
    public mCCoordShifter
{
public:
    virtual ~mCMaxRisenCoordShifter( void );
public:
    virtual void ShiftFaceCoords( mCFace & a_faceDest );
    virtual void ShiftFaceCoords( mCMaxFace & a_faceDest );
    virtual void ShiftVectorCoords( mCVec3 & a_vecDest );
    virtual void ShiftTexCoords( mCVec3 & a_vecDest );
};

#endif