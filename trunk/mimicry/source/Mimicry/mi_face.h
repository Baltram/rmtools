#ifndef MI_FACE_H_INCLUDED
#define MI_FACE_H_INCLUDED

MI_UNNAMED_STRUCTS_NO_WARNINGS_BEGIN

class mCFace
{
public:
    mCFace( MIUInt a_uA, MIUInt a_uB, MIUInt a_uC );
    mCFace( mCFace const & a_faceSource );
    mCFace( void );
   ~mCFace( void );
public:
    MIBool         operator == ( mCFace const & a_faceOther ) const;
    MIBool         operator != ( mCFace const & a_faceOther ) const;
    MIUInt &       operator [] ( MIUInt a_uCoordIndex );
    MIUInt const & operator [] ( MIUInt a_uCoordIndex ) const;
    mCFace &       operator =  ( mCFace const & a_faceSource );
public:
    MIUInt & AccessA( void );
    MIUInt & AccessB( void );
    MIUInt & AccessC( void );
    mCVec3   CalcNormal( mCVec3 const * a_pVertices, MIUInt a_uVertexCount, MIFloat /*OUT*/ a_arrAngles[ 3 ] = 0 ) const;
    MIUInt   GetA( void ) const;
    MIUInt   GetB( void ) const;
    MIUInt   GetC( void ) const;
    void     Invert( void );
    void     Swap( mCFace & a_faceOther );
protected:
union
    {
        MIUInt m_arrIndices[ 3 ];
        struct
        {
            MIUInt m_uA;
            MIUInt m_uB;
            MIUInt m_uC;
        };
    };
};

MI_UNNAMED_STRUCTS_NO_WARNINGS_END

mCIOStreamBinary & operator >> ( mCIOStreamBinary & a_streamSource, mCFace & a_faceDest );
mCIOStreamBinary & operator << ( mCFace & a_faceDest, mCIOStreamBinary & a_streamSource );
mCIOStreamBinary & operator << ( mCIOStreamBinary & a_streamDest, mCFace const & a_faceSource );
mCIOStreamBinary & operator >> ( mCFace const & a_faceSource, mCIOStreamBinary & a_streamDest );

#endif