#ifndef MI_MAXFACE_H_INCLUDED
#define MI_MAXFACE_H_INCLUDED

class mCMaxFace :
    public mCFace
{
public:
    mCMaxFace( MIUInt a_uA, MIUInt a_uB, MIUInt a_uC, MIUInt a_uMatID = 0, MIU32 a_u32SGs = 0 );
    mCMaxFace( mCMaxFace const & a_faceSource );
    mCMaxFace( void );
   ~mCMaxFace( void );
public:
    MIBool      operator == ( mCMaxFace const & a_faceOther ) const;
    MIBool      operator != ( mCMaxFace const & a_faceOther ) const;
    mCMaxFace & operator =  ( mCMaxFace const & a_faceSource );
public:
    MIUInt & AccessMatID( void );
    MIU32 &  AccessSGs( void );
    MIUInt   GetMatID( void ) const;
    MIU32    GetSGs( void ) const;
    void     Swap( mCMaxFace & a_faceOther );
private:
    MIUInt m_uMatID;
    MIU32  m_u32SGs;
};

#endif