#ifndef MI_RISEN2NAME_H_INCLUDED
#define MI_RISEN2NAME_H_INCLUDED

class mCRisen2Name :
    public mCName
{
public:
    explicit mCRisen2Name( mCString const & a_strString = "" );
             mCRisen2Name( mCName const & a_nameSource );
public:
    mCIOStreamBinary & operator << ( mCIOStreamBinary & a_streamSource );
    mCIOStreamBinary & operator >> ( mCIOStreamBinary & a_streamDest ) const;
public:
    MIU32 GetRisen2ID( void ) const;
    void  RegisterRisen2NameStrings( MILPCChar const * a_pStrings, MIUInt a_uCount );
private:
    void Initialize( void );
private:
    enum { ENumNames = 4924 };
private:
    static MIBool                 s_bInitialized;
    static mTNameMap< MIU32 >     s_mapRisen2Hashes;
    static mTMap< MIU32, mCName > s_mapRisen2Names;
    static MILPCChar const        s_arrRisen2Strings[  ];
};

mCIOStreamBinary & operator >> ( mCIOStreamBinary & a_streamSource, mCRisen2Name & a_nameDest );
mCIOStreamBinary & operator << ( mCIOStreamBinary & a_streamDest, mCRisen2Name const & a_nameSource );

#endif