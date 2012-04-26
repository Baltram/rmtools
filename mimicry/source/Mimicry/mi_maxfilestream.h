#ifndef MI_MAXFILESTREAM_H_INCLUDED
#define MI_MAXFILESTREAM_H_INCLUDED

class mCVariant;

class mCMaxFileStream :
    public mCFileStream
{
public:
    mCMaxFileStream( void );
    mCMaxFileStream( mCString const & a_strFileName );
public:
    mEResult Open( mCString const & a_strFileName );
    mEResult ReadPersistentGlobal( mCString const & a_strName, mCVariant & a_vDest );
protected:
    virtual void DirectRead( MILPVoid a_pDest, MIUInt a_uPosition, MIUInt a_uSize );
    virtual void Init( MILPVoid a_pFile, mCString const & a_strFileName, mEFileOpenMode a_enuOpenMode );
private:
    void     Decrypt( MILPVoid a_pData, MIUInt a_uSize, MIU8 a_u8Type );
    mEResult ReadPersistentGlobalData( mCVariant & a_vDest );
private:
    mTArray< MIUInt > m_arrJunkBlocks;
    MIU32             m_u32ClassDataBlock;
    MIU8              m_u8Seed;
};

#endif