#ifndef MI_RISENNAME_H_INCLUDED
#define MI_RISENNAME_H_INCLUDED

class mCRisenName :
    public mCName
{
public:
    explicit mCRisenName( mCString const & a_strString );
             mCRisenName( mCName const & a_nameSource );
public:
    static void InitializeRisen2Strings( void );
    static void InitializeRisen3Strings( void );
    static void RegisterNameStrings( MILPCChar const * a_pStrings, MIUInt a_uCount );
public:
    mCIStreamBinary & operator << ( mCIStreamBinary & a_streamSource );
    mCOStreamBinary & operator >> ( mCOStreamBinary & a_streamDest ) const;
public:
    MIU32 GetRisenID( void ) const;
protected:
    mCRisenName( void );
private:
    static mTNameMap< MIU32 >     s_mapRisenHashes;
    static mTMap< MIU32, mCName > s_mapRisenNames;
};

mCIStreamBinary & operator >> ( mCIStreamBinary & a_streamSource, mCRisenName & a_nameDest );
mCOStreamBinary & operator << ( mCOStreamBinary & a_streamDest, mCRisenName const & a_nameSource );

#endif