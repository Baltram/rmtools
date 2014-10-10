#include "mi_include_risen.h"

mCRisenDoc::mCRisenDoc( mCIOStreamBinary & a_streamIn, mCIOStreamBinary & a_streamOut, mCString const & m_strIndent ) :
    mCDoc( a_streamIn, a_streamOut, m_strIndent )
{
    mCRisenName::InitializeRisen3Strings();
}

mCString mCRisenDoc::FormatData( mCIOStreamBinary & m_streamIn, mCString const & a_strType, MIUInt uSize, mCString const & m_strIndent )
{
    mCMemoryStream streamText;
    mCRisenDoc Doc( m_streamIn, streamText, m_strIndent );
    Doc.FormatData( a_strType, uSize );
    mCString strResult;
    strResult.SetText( static_cast< MILPCChar >( streamText.GetBuffer() ), streamText.GetSize() );
    return strResult;
}

namespace
{
    enum ETypes { EBool, EFloat, EChar, ESignedChar, EUnsignedChar, EShort, EUnsignedShort, EInt, ELong, EUnsignedInt, EUnsignedLong, EInt64, EUnsignedInt64, EString, ETypes_Count };

    MILPCChar const       s_arrTypes[ ETypes_Count ] = { "bool", "float", "char", "signed char", "unsigned char", "short", "unsigned short", "int", "long", "unsigned int", "unsigned long", "__int64", "unsigned __int64", "class bCString" };
    mTStringMap< ETypes > s_mapTypes;
    MILPCChar const       s_strHex = "0123456789ABCDEF";
}

void mCRisenDoc::FormatData( mCString a_strType, MIUInt uSize )
{
    if ( s_mapTypes.GetCount() == 0 )
        for ( MIUInt u = ETypes_Count; u--; reinterpret_cast< MIUInt & >( s_mapTypes[ s_arrTypes[ u ] ] ) = u );
    ETypes enuType;
    if ( !s_mapTypes.GetAt( a_strType, enuType ) )
    {
        
        if ( a_strType.ReplaceLeft( "class bTObjArray<" ) && a_strType.ReplaceRight( ">" ) )
        {
            StartArray();
            for ( MIUInt u = 0, uCount = m_streamIn.ReadU32(); u != uCount; ++u )
                FormatData( a_strType, ( uSize - 4 ) / uCount /*In many cases incorrect*/ ), WriteLine( u + 1 < uCount ? "," : "" );
            EndArray( MIFalse );
        }
        else if ( a_strType.StartsWith( "class " ) && m_streamIn.ReadString( 4 ) == "GEC2" )
        {
            m_streamIn.Skip( -4 );
            DocumentRisen3Class();
        }
        else
        {
            if ( uSize == MI_DW_INVALID )
                return;
            mCBuffer bufferData( uSize );
            MILPByte pData = bufferData.AccessMemory();
            m_streamIn.Read( pData, uSize );
            mCString strBytes( ' ', uSize * 3 + 1 );
            for ( MIUInt u = 0, v = 1; u != uSize; ++u, ++v )
            {
                strBytes[ v++ ] = s_strHex[ pData[ u ] / 0x10 ];
                strBytes[ v++ ] = s_strHex[ pData[ u ] & 0x0F ];
            }
            strBytes[ 0 ] = '<', strBytes[ uSize * 3 ] = '>';
            Write( strBytes );
        }
    }
    mCString strResult;
    switch ( enuType )
    {
    case EBool:
        strResult = m_streamIn.ReadBool() ? "True" : "False";
        break;
    case EFloat:
        strResult.Format( "%f", m_streamIn.ReadFloat() );
        break;
    case EChar:
        strResult.Format( "'%c'", m_streamIn.ReadChar() );
        break;
    case ESignedChar:
        strResult.Format( "%hhi", m_streamIn.ReadI8() );
        break;
    case EUnsignedChar:
        strResult.Format( "%hhu", m_streamIn.ReadU8() );
        break;
    case EShort:
        strResult.Format( "%hi", m_streamIn.ReadI16() );
        break;
    case EUnsignedShort:
        strResult.Format( "%hu", m_streamIn.ReadU16() );
        break;
    case EInt:
    case ELong:
        strResult.Format( "%li", m_streamIn.ReadI32() );
        break;
    case EUnsignedInt:
    case EUnsignedLong:
        strResult.Format( "%lu", m_streamIn.ReadU32() );
        break;
    case EInt64:
        strResult.Format( "%lli", m_streamIn.ReadI64() );
        break;
    case EUnsignedInt64:
        strResult.Format( "%llu", m_streamIn.ReadU64() );
        break;
    case EString:
        strResult = "\"" + m_streamIn.ReadString( m_streamIn.ReadU16() ) + "\"";
        break;
    }
    Write( strResult );
}

MIBool mCRisenDoc::DocumentRisen3DlgData2( void )
{
    MIUInt uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GAR5" )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.ReadU32();
    StartBlock( "Gestures" );
    {
        DocumentRisen3Class();
		WriteLine();
    }
    EndBlock();
    StartBlock( "FacialAnimation" );
    {
        if ( m_streamIn.ReadU32() != 30 )
            return m_streamIn.Seek( uOffset ), MIFalse;
        MIUInt uFrameCount = m_streamIn.ReadU32();
        MIUInt uDataSize = m_streamIn.ReadU32();
        MIUInt uBlendShapeCount = uDataSize / uFrameCount;
        MIUInt uFramesOffset = m_streamIn.Tell();
        m_streamIn.Skip( uDataSize );
        StartArray( "BlendShapeNames" );
        for ( MIUInt u = 0, uBlendShapeCount = m_streamIn.ReadU32(); u != uBlendShapeCount; ++u )
        {
            FormatData( "class bCString" );
            WriteLine( u + 1 == uBlendShapeCount ? "" : "," );
        }
        EndArray( MIFalse ); WriteLine( ";" );
        m_streamIn.Seek( uFramesOffset );
        StartArray( "BlendShapeTimeLines" );
        for ( MIUInt u = 0; u != uBlendShapeCount; ++u )
        {
            FormatData( "", uFrameCount );
            WriteLine( u + 1 == uBlendShapeCount ? "" : "," );
        }
        EndArray( MIFalse ); WriteLine( ";" );
    }
    EndBlock();
    return MITrue;
}

MIUInt mCRisenDoc::DocumentRisen3Class( void )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GEC2" )
    {
        m_streamIn.Seek( uOffset );
        return 0;
    }
    StartBlock( ReadHash( "class" ) );
    {
        WriteLine( mCString().Format( "Version = %u;", m_streamIn.ReadU16() ) );
        m_streamIn.ReadU32();
        StartBlock( "Properties" );
        {
            for ( MIUInt u = m_streamIn.ReadU16(); u--; )
            {
                mCString strType, strName, strData;
                strType = ReadHash( "type" );
                strName = ReadHash( "name" );
                Write( strType + " " + strName + " = " );
                FormatData( strType, m_streamIn.ReadU32() );
                WriteLine( ";" );
            }
        }
        EndBlock();
        StartBlock( "ClassData" );
        {
            for ( MIUInt u = m_streamIn.ReadU32(); u--; )
            {
                mCString strClass = ReadHash( "class" );
                Write( mCString().Format( "[%s, Version %hu] = ", strClass.GetText(), m_streamIn.ReadU16() ) );
                FormatData( "", m_streamIn.ReadU32() );
                WriteLine( ";" );
            }
        }
        EndBlock();
    }
    EndBlock( MIFalse );
    return m_streamIn.Tell() - uOffset;
}

mCString mCRisenDoc::ReadHash( mCString const & a_strType )
{
    mCRisenName nameHash;
    m_streamIn >> nameHash;
    if ( nameHash.IsValid() )
        return nameHash.GetString();
    m_streamIn.Skip( -4 );
    return mCString().Format( "<unknown %s 0x%.8x>", a_strType.GetText(), m_streamIn.ReadU32() );
}