#include "mi_include_variant.h"

mTArray< mCVariant::SPool * > mCVariant::SPool::s_arrPools;

mTArray< MIUInt > mCVariant::SPool::s_arrCollapsedPoolIndices;

mTArray< mCVariant::SPool::Map * > mCVariant::SPool::s_arrMaps;

mTArray< MIUInt > mCVariant::SPool::s_arrVacantMapIndices;

MIUInt mCVariant::SPool::s_uCollapsablePoolCount = 0;

mCVariant::mCVariant( void )
{
    m_idElem.Invalidate();
}

mCVariant::mCVariant( mCVariant const & a_vSource )
{
    if ( a_vSource.IsEmpty() )
        m_idElem.Invalidate();
    else
        m_idElem = SPool::Clone( a_vSource.m_idElem );
}

mCVariant::~mCVariant( void )
{
    Clear();
}

mCVariant & mCVariant::operator = ( mCVariant const & a_vSource )
{
    if ( this == &a_vSource )
        return *this;
    mCVariant Temp( a_vSource );
    Swap( Temp );
    return *this;
}

MIBool mCVariant::operator == ( mCVariant const & a_vOther ) const
{
    return SPool::Compare( m_idElem, a_vOther.m_idElem );
}

MIBool mCVariant::operator != ( mCVariant const & a_vOther ) const
{
    return !( *this == a_vOther );
}

void mCVariant::Clear( void )
{
    if ( m_idElem.IsValid() )
        SPool::Free( m_idElem );
}

void mCVariant::Swap( mCVariant & a_vOther )
{
    if ( this == &a_vOther )
        return;
    g_swap( m_idElem, a_vOther.m_idElem );
}
