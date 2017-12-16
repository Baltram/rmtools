#ifndef MI_RISENNAME_H_INCLUDED
#define MI_RISENNAME_H_INCLUDED

enum mEGame
{
    mEGame_Unknown,
    mEGame_Risen2,
    mEGame_Risen3,
    mEGame_Elex
};

class mCRisenName :
    public mCName
{
public:
    explicit mCRisenName( mCString const & a_strString );
    explicit mCRisenName( MIU32 a_u32Hash );
             mCRisenName( mCName const & a_nameSource );
             mCRisenName( void );
public:
    static mEGame GetGame( void );
    static void   SetGame( mEGame a_enuGame );
    static void   InitializeGameStrings( void );
public:
    mCIStreamBinary & operator << ( mCIStreamBinary & a_streamSource );
    mCOStreamBinary & operator >> ( mCOStreamBinary & a_streamDest ) const;
public:
    MIBool GetEnumValue( MIInt & a_iDest ) const;
    MIU32  GetRisenID( void ) const;
private:
    static void InitializeRisen2Strings( void );
    static void InitializeRisen3Strings( void );
    static void InitializeElexStrings( void );
    static void RegisterNameStrings( MILPCChar const * a_pStrings, MIUInt a_uCount );
private:
    static mEGame                 s_enuGame;
    static mTNameMap< MIU32 >     s_mapRisenHashes;
    static mTMap< MIU32, mCName > s_mapRisenNames;
    static mTNameMap< MIInt >     s_mapRisen3EnumValues;
    static mTNameMap< MIInt >     s_mapElexEnumValues;
};

mCIStreamBinary & operator >> ( mCIStreamBinary & a_streamSource, mCRisenName & a_nameDest );
mCOStreamBinary & operator << ( mCOStreamBinary & a_streamDest, mCRisenName const & a_nameSource );

#endif