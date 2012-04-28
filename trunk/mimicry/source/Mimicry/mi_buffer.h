#ifndef MI_BUFFER_H_INCLUDED
#define MI_BUFFER_H_INCLUDED

class mCBuffer
{
public:
    mCBuffer( MILPCVoid a_pSource, MIUInt a_uSize );
    mCBuffer( MIUInt a_uSize, MIByte a_byteValue = 0 );
    mCBuffer( mCBuffer const & a_bufSource );
    mCBuffer( void );
   ~mCBuffer( void );
public:
    mCBuffer &     operator =  ( mCBuffer const & a_bufSource );
    MIByte const & operator [] ( MIUInt a_uOffset ) const;
    MIByte &       operator [] ( MIUInt a_uOffset );
    MIBool         operator == ( mCBuffer const & a_bufOther ) const;
    MIBool         operator != ( mCBuffer const & a_bufOther ) const;
public:
    MILPByte  AccessMemory( void );
    MILPCByte GetMemory( void ) const;
    MIUInt    GetSize( void ) const;
    MILPByte  Resize( MIUInt a_uNewSize );
    void      SetAt( MIUInt a_uOffset, MILPVoid a_pSource, MIUInt a_uSize );
    void      SetAt( MIUInt a_uOffset, MIByte a_byteValue, MIUInt a_uSize );
    void      Swap( mCBuffer & a_bufOther );
protected:
    MILPByte m_pData;
    MIUInt   m_uSize;
};

#endif