#ifndef MI_VEC3_H_INCLUDED
#define MI_VEC3_H_INCLUDED

MI_UNNAMED_STRUCTS_NO_WARNINGS_BEGIN

class mCVec3
{
public:
    mCVec3( MIFloat a_fX, MIFloat a_fY, MIFloat a_fZ );
    mCVec3( mCVec3 const & a_vecSource );
    mCVec3( void );
   ~mCVec3( void );
public:
    MIBool          operator == ( mCVec3 const & a_vecOther ) const;
    MIBool          operator != ( mCVec3 const & a_vecOther ) const;
    MIFloat &       operator [] ( MIUInt a_uCoordIndex );
    MIFloat const & operator [] ( MIUInt a_uCoordIndex ) const;
    mCVec3          operator *  ( MIFloat a_fFactor ) const;
    mCVec3          operator /  ( MIFloat a_fFactor ) const;
    mCVec3          operator -  ( mCVec3 const & a_vecVector ) const;
    mCVec3          operator +  ( mCVec3 const & a_vecVector ) const;
    mCVec3 &        operator *= ( MIFloat a_fFactor );
    mCVec3 &        operator /= ( MIFloat a_fFactor );
    mCVec3 &        operator += ( mCVec3 const & a_vecVector );
    mCVec3 &        operator -= ( mCVec3 const & a_vecVector );
    mCVec3 &        operator =  ( mCVec3 const & a_vecSource );
public:
    static mCVec3  CrossProduct( mCVec3 const & a_vecFirst, mCVec3 const & a_vecSecond );
    static MIFloat DotProduct( mCVec3 const & a_vecFirst, mCVec3 const & a_vecSecond );
public:
    MIFloat &      AccessX( void );
    MIFloat &      AccessY( void );
    MIFloat &      AccessZ( void );
    MIFloat        CalcAngleDeg( mCVec3 const & a_vecOther, MIBool a_bAlreadyNormalized = MIFalse ) const;
    MIFloat        CalcAngleRad( mCVec3 const & a_vecOther, MIBool a_bAlreadyNormalized = MIFalse ) const;
    mCVec3         CalcCrossProduct( mCVec3 const & a_vecSecond ) const;
    MIFloat        CalcDotProduct( mCVec3 const & a_vecSecond ) const;
    MIFloat        CalcMagnitude( void ) const;
    MIFloat        CalcMagnitudeSqr( void ) const;
    mCVec3         CalcNormalized( void ) const;
    void           Clear( void );
    MIFloat        GetX( void ) const;
    MIFloat        GetY( void ) const;
    MIFloat        GetZ( void ) const;
    MIBool         IsSimilar( mCVec3 const & a_vecOther, MIFloat a_fCoordTolerance, MIBool a_bIgnoreZ = MIFalse ) const;
    MIBool         IsZero( void ) const;
    mCVec3 const & Normalize( void );
    void           Swap( mCVec3 & a_vecOther );
private:
    union
    {
        MIFloat m_arrCoordinates[ 3 ];
        struct
        {
            MIFloat m_fX;
            MIFloat m_fY;
            MIFloat m_fZ;
        };
    };
};

MI_UNNAMED_STRUCTS_NO_WARNINGS_END

mCIStreamBinary & operator >> ( mCIStreamBinary & a_streamSource, mCVec3 & a_vecDest );
mCIStreamBinary & operator << ( mCVec3 & a_vecDest, mCIStreamBinary & a_streamSource );
mCOStreamBinary & operator << ( mCOStreamBinary & a_streamDest, mCVec3 const & a_vecSource );
mCOStreamBinary & operator >> ( mCVec3 const & a_vecSource, mCOStreamBinary & a_streamDest );

#endif
