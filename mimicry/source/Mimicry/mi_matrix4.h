#ifndef MI_MATRIX4_H_INCLUDED
#define MI_MATRIX4_H_INCLUDED

class mCQuaternion;

class mCMatrix4
{
public:
    enum EElement
    {
        EElement_11,
        EElement_12,
        EElement_13,
        EElement_14,
        EElement_21,
        EElement_22,
        EElement_23,
        EElement_24,
        EElement_31,
        EElement_32,
        EElement_33,
        EElement_34,
        EElement_41,
        EElement_42,
        EElement_43,
        EElement_44,
        EElement_Count,
        EElement_ForceDWORD = MI_FORCE_DWORD
    };
    enum ERow
    {
        ERow_XAxis,
        ERow_YAxis,
        ERow_ZAxis,
        ERow_Translation,
        ERow_Count,
        ERow_ForceDWORD = MI_FORCE_DWORD
    };
    enum EColumn
    {
        EColumn_X,
        EColumn_Y,
        EColumn_Z,
        EColumn_Reserved,
        EColumn_Count,
        EColumn_ForceDWORD = MI_FORCE_DWORD
    };
public:
    mCMatrix4( mCQuaternion const & a_quatSource );
    mCMatrix4( mCMatrix4 const & a_matSource );
    mCMatrix4( void );
   ~mCMatrix4( void );
public:
    mCMatrix4 & operator =  ( mCMatrix4 const & a_matSource );
    MIBool      operator == ( mCMatrix4 const & a_matOther ) const;
    MIBool      operator != ( mCMatrix4 const & a_matOther ) const;
    mCMatrix4   operator *  ( mCMatrix4 const & a_matRight ) const;
    mCMatrix4   operator *  ( mCQuaternion const & a_quatRotation ) const;
    mCMatrix4 & operator *= ( mCMatrix4 const & a_matRight );
    mCMatrix4 & operator *= ( mCQuaternion const & a_quatRotation );
public:
    MIFloat &       AccessElement( MIUInt a_uElementIndex );
    MIFloat &       AccessElement( MIUInt a_uRowIndex, MIUInt a_uColumnIndex );
    MIFloat *       AccessElements( void );
    MIFloat *       AccessRow( MIUInt a_uRowIndex );
    mCVec3 &        AccessTranslation( void );
    MIFloat         GetElement( MIUInt a_uElementIndex ) const;
    MIFloat         GetElement( MIUInt a_uRowIndex, MIUInt a_uColumnIndex ) const;
    MIFloat const * GetElements( void ) const;
    MIFloat const * GetRow( MIUInt a_uRowIndex ) const;
    mCVec3 const &  GetTranslation( void ) const;
    void            ModifyRotation( mCQuaternion const & a_quatSource );
    void            SetToIdentity( void );
    void            SetToRotation( mCQuaternion const & a_quatRotation );
    void            SetToProduct( mCMatrix4 const & a_matLeft, mCMatrix4 const & a_matRight );
    void            SetToZero( void );
    void            Swap( mCMatrix4 & a_matOther );
private:
    union
    {
        MIFloat m_arrElements[ EElement_Count ];
        MIFloat m_arrRowColumn[ ERow_Count ][ EColumn_Count ];
    };
};

#endif