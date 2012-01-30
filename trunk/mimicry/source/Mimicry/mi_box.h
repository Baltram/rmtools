#ifndef MI_BOX_H_INCLUDED
#define MI_BOX_H_INCLUDED

class mCBox
{
public:
    mCBox( mCVec3 a_vecMin, mCVec3 a_vecMax );
    mCBox( mCBox const & a_boxSource );
    mCBox( void );
   ~mCBox( void );
public:
    MIBool  operator == ( mCBox const & a_boxOther ) const;
    MIBool  operator != ( mCBox const & a_boxOther ) const;
    mCBox   operator *  ( MIFloat a_fFactor ) const;
    mCBox   operator -  ( mCVec3 const & a_vecVector ) const;
    mCBox   operator +  ( mCVec3 const & a_vecVector ) const;
    mCBox   operator /  ( MIFloat a_fFactor ) const;
    mCBox   operator &  ( mCBox const & a_boxBox ) const;
    mCBox   operator |  ( mCBox const & a_boxBox ) const;
    mCBox   operator |  ( mCVec3 const & a_vecVector ) const;
    mCBox & operator *= ( MIFloat a_fFactor );
    mCBox & operator += ( mCVec3 const & a_vecVector );
    mCBox & operator -= ( mCVec3 const & a_vecVector );
    mCBox & operator /= ( MIFloat a_fFactor );
    mCBox & operator &= ( mCBox const & a_boxBox );
    mCBox & operator |= ( mCBox const & a_boxBox );
    mCBox & operator |= ( mCVec3 const & a_vecVector );
    mCBox & operator =  ( mCBox const & a_boxSource );
public:
    mCVec3 &       AccessMax( void );
    mCVec3 &       AccessMin( void );
    MIBool         Contains( mCBox const & a_boxBox ) const;
    MIBool         Contains( mCVec3 const & a_vecVector ) const;
    mCVec3         GetCenter( void ) const;
    MIFloat        GetHeight( void ) const;
    MIFloat        GetLength( void ) const;
    mCVec3 const & GetMax( void ) const;
    mCVec3 const & GetMin( void ) const;
    MIFloat        GetWidth( void ) const;
    MIBool         Intersects( mCBox const & a_boxBox ) const;
    void           Invalidate( void );
    MIBool         IsValid( void ) const;
    void           SetTo( mCVec3 const & a_vecMin, mCVec3 const & a_vecMax );
private:
    static mCVec3 const s_vecPosInfinity;
    static mCVec3 const s_vecNegInfinity;
private:
    mCVec3 m_vecMin;
    mCVec3 m_vecMax;
};

#endif