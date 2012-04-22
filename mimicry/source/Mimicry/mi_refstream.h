#ifndef MI_REFSTREAM_H_INCLUDED
#define MI_REFSTREAM_H_INCLUDED

class mCRefStream :
    public mCIStreamBinary,
    public mCOStreamBinary
{
public:
    virtual ~mCRefStream( void );
public:
    mCRefStream( MILPVoid a_pTarget, MIUInt a_uSize );
    mCRefStream( mCBuffer & a_bufTarget );
public:
    virtual mEResult Read( MILPVoid a_pDest, MIUInt a_uSize );
    virtual mEResult Read( mCString & a_strDest, MIUInt a_uSize );
    virtual mEResult Read( mCString & a_strDest );
    virtual mEResult Write( MILPCVoid a_pSource, MIUInt a_uSize );
    virtual mEResult Write( mCString const & a_strSource );
public:
    MIUInt   GetRemainingSize( void ) const;
    mEResult Skip( MIUInt a_uCount );
protected:
    mCRefStream( mCRefStream const & );
    mCRefStream( void );
protected:
    mCRefStream & operator = ( mCRefStream const & );
protected:
    MIByte *   m_pMemory;
    MIUInt     m_uSize;
};

#endif