#ifndef MI_GENOMEFILE_H_INCLUDED
#define MI_GENOMEFILE_H_INCLUDED

class mCGenomeFile
{
public:
    explicit mCGenomeFile( mCIOStreamBinary * a_pStream );
public:
    mCIOStreamBinary const & operator () ( void ) const;
    mCIOStreamBinary       & operator () ( void );
    mCGenomeFile & operator >> ( mCString & a_strDest );
    mCGenomeFile & operator << ( mCString const & a_strSource );
public:
    void     EndWrite( void );
    MIBool   IsArchiveFile( void );
    void     Read( mCString & a_strSource );
    mCString ReadString( void );
    void     StartRead( void );
    void     StartWrite( void );
    void     Write( mCString const & a_strSource );
private:
    mCGenomeFile( void );
private:
    MIBool CanRead( void );
    MIBool CanWrite( void );
private:
    mTStringMap< MIUInt > m_mapStringIndices;
    mTArray< mCString >   m_arrStrings;
    mCIOStreamBinary *    m_pStream;
    MIUInt                m_uStartOffset;
    MIBool                m_bIsArchiveFile;
    MIBool                m_bIsReadMode;
    MIBool                m_bIsOpen;
};

mCGenomeFile & operator << ( mCString & a_strDest, mCGenomeFile & a_Source );
mCGenomeFile & operator >> ( mCString & a_strSource, mCGenomeFile & a_Dest );

#endif
