#ifndef MI_QUATERNION_H_INCLUDED
#define MI_QUATERNION_H_INCLUDED

MI_UNNAMED_STRUCTS_NO_WARNINGS_BEGIN

class mCQuaternion
{
public:
    mCQuaternion( mCMatrix4 const & a_matSource );
    mCQuaternion( mCQuaternion const & a_quatSource );
    mCQuaternion( void );
   ~mCQuaternion( void );
public:
    mCQuaternion &  operator =  ( mCQuaternion const & a_quatSource );
    MIFloat &       operator [] ( MIUInt a_uCoordIndex );
    MIFloat const & operator [] ( MIUInt a_uCoordIndex ) const;
public:
    MIFloat &      AccessX( void );
    MIFloat &      AccessY( void );
    MIFloat &      AccessZ( void );
    MIFloat &      AccessW( void );
    void           Clear( void );
    MIFloat        GetX( void ) const;
    MIFloat        GetY( void ) const;
    MIFloat        GetZ( void ) const;
    MIFloat        GetW( void ) const;
    mCQuaternion & Normalize( void );
    mCQuaternion & Inverse( void );
    void           Swap( mCQuaternion & a_quatOther );
private:
    union
    {
        MIFloat m_arrCoordinates[ 4 ];
        struct
        {
            MIFloat m_fX;
            MIFloat m_fY;
            MIFloat m_fZ;
            MIFloat m_fW;
        };
    };
};

MI_UNNAMED_STRUCTS_NO_WARNINGS_END

mCIStreamBinary & operator >> ( mCIStreamBinary & a_streamSource, mCQuaternion & a_quatDest );
mCIStreamBinary & operator << ( mCQuaternion & a_quatDest, mCIStreamBinary & a_streamSource );
mCOStreamBinary & operator << ( mCOStreamBinary & a_streamDest, mCQuaternion const & a_quatSource );
mCOStreamBinary & operator >> ( mCQuaternion const & a_quatSource, mCOStreamBinary & a_streamDest );

#endif
