#include "mi_include_map.h"

mTNameMap< mCName::StringRecord > mCName::s_mapNames;

mCName::mCName( mCString const & a_strString ) :
    m_u32ID( g_djb2( a_strString.GetText() ) | ( ( MIU32 ) 1 << 31 ) )
{
    StringRecord * pRecord = &( s_mapNames[ *this ] );
    while ( ( pRecord->m_strString.GetLength() ) && ( pRecord->m_strString != a_strString ) )
    {
        if ( !pRecord->m_nameNext.m_u32ID )
            pRecord->m_nameNext.m_u32ID = m_u32ID + 4000747;
        m_u32ID = pRecord->m_nameNext.m_u32ID;
        pRecord = &( s_mapNames[ *this ] );
    }
    if ( pRecord->m_strString == "" )
        pRecord->m_strString = a_strString;
}

mCName::mCName( mCName const & a_nameSource ) :
    m_u32ID( a_nameSource.m_u32ID )
{
}

mCName::~mCName( void )
{
}

MIBool mCName::operator == ( mCName const & a_nameOther ) const
{
    return m_u32ID == a_nameOther.m_u32ID;
}

MIBool mCName::operator != ( mCName const & a_nameOther ) const
{
    return m_u32ID != a_nameOther.m_u32ID;
}

mCName & mCName::operator = ( mCName const & a_nameSource )
{
    m_u32ID = a_nameSource.m_u32ID;
    return *this;
}

MIU32 mCName::GetID( void )
{
    return m_u32ID;
}

mCString const & mCName::GetString( void )
{
    return s_mapNames[ *this ].m_strString;
}

mCName::mCName( void ) :
    m_u32ID( 0 )
{
}