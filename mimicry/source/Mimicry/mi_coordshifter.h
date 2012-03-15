#ifndef MI_COORDSHIFTER_H_INCLUDED
#define MI_COORDSHIFTER_H_INCLUDED

class mCCoordShifter
{
public:
    virtual mCFace &    ShiftFaceCoords( mCFace & a_faceDest ) const = 0;
    virtual mCMaxFace & ShiftFaceCoords( mCMaxFace & a_faceDest ) const = 0;
    virtual mCMatrix4 & ShiftMatrixCoords( mCMatrix4 & a_matDest ) const = 0;
    virtual mCVec3 &    ShiftVectorCoords( mCVec3 & a_vecDest ) const = 0;
    virtual mCVec3 &    ShiftTexCoords( mCVec3 & a_vecDest ) const = 0;
public:
    mCMesh & ShiftMeshCoords( mCMesh & a_meshDest ) const;
protected:
    virtual ~mCCoordShifter( void ) = 0;
protected:
    mCMatrix4 m_matTransform;
};

class mCMaxRisenCoordShifter :
    public mCCoordShifter
{
public:
    static mCMaxRisenCoordShifter const & GetInstance( void );
public:
    virtual mCFace &    ShiftFaceCoords( mCFace & a_faceDest ) const;
    virtual mCMaxFace & ShiftFaceCoords( mCMaxFace & a_faceDest ) const;
    virtual mCMatrix4 & ShiftMatrixCoords( mCMatrix4 & a_matDest ) const;
    virtual mCVec3 &    ShiftVectorCoords( mCVec3 & a_vecDest ) const;
    virtual mCVec3 &    ShiftTexCoords( mCVec3 & a_vecDest ) const;
protected:
    virtual ~mCMaxRisenCoordShifter( void );
protected:
    mCMaxRisenCoordShifter( void );
};

class mCMaxToObjCoordShifter :
    public mCCoordShifter
{
public:
    static mCMaxToObjCoordShifter const & GetInstance( void );
public:
    virtual mCFace &    ShiftFaceCoords( mCFace & a_faceDest ) const;
    virtual mCMaxFace & ShiftFaceCoords( mCMaxFace & a_faceDest ) const;
    virtual mCMatrix4 & ShiftMatrixCoords( mCMatrix4 & a_matDest ) const;
    virtual mCVec3 &    ShiftVectorCoords( mCVec3 & a_vecDest ) const;
    virtual mCVec3 &    ShiftTexCoords( mCVec3 & a_vecDest ) const;
protected:
    virtual ~mCMaxToObjCoordShifter( void );
protected:
    mCMaxToObjCoordShifter( void );
};

class mCObjToMaxCoordShifter :
    public mCCoordShifter
{
public:
    static mCObjToMaxCoordShifter const & GetInstance( void );
public:
    virtual mCFace &    ShiftFaceCoords( mCFace & a_faceDest ) const;
    virtual mCMaxFace & ShiftFaceCoords( mCMaxFace & a_faceDest ) const;
    virtual mCMatrix4 & ShiftMatrixCoords( mCMatrix4 & a_matDest ) const;
    virtual mCVec3 &    ShiftVectorCoords( mCVec3 & a_vecDest ) const;
    virtual mCVec3 &    ShiftTexCoords( mCVec3 & a_vecDest ) const;
protected:
    virtual ~mCObjToMaxCoordShifter( void );
protected:
    mCObjToMaxCoordShifter( void );
};

#endif