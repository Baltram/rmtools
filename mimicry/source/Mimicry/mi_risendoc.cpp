#include "mi_include_risen.h"

namespace
{
    enum ETypes { EBool, EFloat, EChar, ESignedChar, EUnsignedChar, EShort, EUnsignedShort, EInt, ELong, EUnsignedInt, EUnsignedLong, EInt64, EUnsignedInt64, EString, EScriptProxyScript, EScriptProxyAIFunction, EScriptProxyAIState, EGuiBitmapProxy2, EResourceProxy, EWeatherEnvironmentProxy, EEffectProxy, EFocusModeProxy2, EMovementSpeciesProxy, EParticleSystemProxy, EGuid, ETemplateEntityProxy, EEntityProxy, EBox, EEulerAngles, EFloatColor, EMatrix, EQuaternion, ERange1, EVector, EVector2, ELetterLocString, ETypes_Count };

    MILPCChar const       s_arrTypes[ ETypes_Count ] = { "bool", "float", "char", "signed char", "unsigned char", "short", "unsigned short", "int", "long", "unsigned int", "unsigned long", "__int64", "unsigned __int64", "class bCString", "class eCScriptProxyScript", "class gCScriptProxyAIFunction", "class gCScriptProxyAIState", "class eCGuiBitmapProxy2", "class eTResourceProxy", "class eCWeatherEnvironmentProxy", "class gCEffectProxy", "class gCFocusModeProxy2", "class gCMovementSpeciesProxy", "class eCParticleSystemProxy", "class bCGuid", "class eCTemplateEntityProxy", "class eCEntityProxy", "class bCBox", "class bCEulerAngles", "class bCFloatColor", "class bCMatrix", "class bCQuaternion", "class bCRange1", "class bCVector", "class bCVector2", "class gCLetterLocString" };
    mTStringMap< ETypes > s_mapTypes;
    MILPCChar const       s_strHex = "0123456789ABCDEF";
}

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

void mCRisenDoc::FormatData( mCString a_strType, MIUInt a_uSize )
{
    if ( s_mapTypes.GetCount() == 0 )
        for ( MIUInt u = ETypes_Count; u--; reinterpret_cast< MIUInt & >( s_mapTypes[ s_arrTypes[ u ] ] ) = u );
    ETypes enuType;
    if ( a_strType.StartsWith( "class eTResourceProxy" ) )
        a_strType = "class eTResourceProxy";
    if ( !s_mapTypes.GetAt( a_strType, enuType ) )
    {
        if ( a_strType.ReplaceLeft( "class bTObjArray<" ) && a_strType.ReplaceRight( ">" ) )
        {
            StartArray();
            for ( MIUInt u = 0, uCount = m_streamIn.ReadU32(); u != uCount; ++u )
                FormatData( a_strType, ( a_uSize - 4 ) / uCount /*In many cases incorrect*/ ), WriteLine( u + 1 < uCount ? "," : "" );
            EndArray( MIFalse );
        }
        else if ( a_strType.StartsWith( "class " ) && DocumentRisen3Class() != 0 )
        {
        }
        else if ( a_strType.StartsWith( "enum " ) && mCRisenName( a_strType ).GetRisenID() )
        {
            mCRisenName EnumMember;
            m_streamIn >> EnumMember;
            MIInt iValue = m_streamIn.ReadI32(), iValue2;
            if ( EnumMember.GetEnumValue( iValue2 ) && iValue2 == iValue )
                Write( EnumMember.GetString() );
            else
                Write( mCString().Format( "%i", iValue ) );
        }
        else
        {
            if ( a_uSize == MI_DW_INVALID )
                return;
            mCBuffer bufferData( a_uSize );
            MILPByte pData = bufferData.AccessMemory();
            m_streamIn.Read( pData, a_uSize );
            mCString strBytes( ' ', a_uSize * 3 + 1 );
            for ( MIUInt u = 0, v = 1; u != a_uSize; ++u, ++v )
            {
                strBytes[ v++ ] = s_strHex[ pData[ u ] / 0x10 ];
                strBytes[ v++ ] = s_strHex[ pData[ u ] & 0x0F ];
            }
            strBytes[ 0 ] = '<', strBytes[ a_uSize * 3 ] = '>';
            Write( strBytes );
        }
        return;
    }
    mCString strResult;
    MIUInt uFloatCount = 0;
    mCString strShortName;
    switch ( enuType )
    {
    case EBool:
        strResult = m_streamIn.ReadU32() != 0 ? "True" : "False";
        break;
    case EFloat:
        strResult.Format( "%f", m_streamIn.ReadFloat() );
        break;
    case EChar:
        strResult.Format( "'%c'", m_streamIn.ReadChar() );
        break;
    case ESignedChar:
        strResult.Format( "%i", m_streamIn.ReadI8() );
        break;
    case EUnsignedChar:
        strResult.Format( "%u", m_streamIn.ReadU8() );
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
    case EScriptProxyScript:
    case EScriptProxyAIFunction:
    case EScriptProxyAIState:
    case EGuiBitmapProxy2:
        m_streamIn.ReadU16();  // Version
    case EString:
    case ELetterLocString:
    case EResourceProxy:
    case EWeatherEnvironmentProxy:
    case EEffectProxy:
    case EFocusModeProxy2:
    case EMovementSpeciesProxy:
    case EParticleSystemProxy:
    {
        if ( enuType == ELetterLocString )
            m_streamIn.Skip( 4 );
        mCString strText = m_streamIn.ReadString( m_streamIn.ReadU16() );
        if ( strText.Contains( '\"' ) )
            strResult = "\"\"\"" + strText + "\"\"\"";
        else
            strResult = "\"" + strText + "\"";
        break;
    }
    case EGuid:
    case ETemplateEntityProxy:
    case EEntityProxy:
    {
        MIU32 u32Part1;
        MIU16 u16Part2, u16Part3;
        MIU8 arrPart4[ 8 ];
        m_streamIn >> u32Part1 >> u16Part2 >> u16Part3;
        for ( MIUInt u = 0; u != 8; ++u )
            m_streamIn >> arrPart4[ u ];
        strResult.Format( "{%.8lX-%.4hX-%.4hX-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X}", u32Part1, u16Part2, u16Part3, arrPart4[ 0 ], arrPart4[ 1 ], arrPart4[ 2 ], arrPart4[ 3 ], arrPart4[ 4 ], arrPart4[ 5 ], arrPart4[ 6 ], arrPart4[ 7 ] );
        break;
    }
    case EBox:
        if ( uFloatCount == 0 )
            uFloatCount = 6, strShortName = "box";
    case EEulerAngles:
        if ( uFloatCount == 0 )
            uFloatCount = 3, strShortName = "euler";
    case EFloatColor:
        if ( uFloatCount == 0 )
            uFloatCount = 3, strShortName = "color";
    case EMatrix:
        if ( uFloatCount == 0 )
            uFloatCount = 16, strShortName = "mat";
    case EQuaternion:
        if ( uFloatCount == 0 )
            uFloatCount = 4, strShortName = "quat";
    case ERange1:
        if ( uFloatCount == 0 )
            uFloatCount = 2, strShortName = "range";
    case EVector:
        if ( uFloatCount == 0 )
            uFloatCount = 3, strShortName = "vec";
    case EVector2:
        if ( uFloatCount == 0 )
            uFloatCount = 2, strShortName = "vec2";
        if ( uFloatCount != 0 )
        {
            strResult = "(" + strShortName;
            for ( MIUInt u = uFloatCount; u--; )
                strResult += mCString().Format( " %f", m_streamIn.ReadFloat() );
            strResult += ")";
        }
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
            FormatData( s_arrTypes[ EString ] );
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

MIBool mCRisenDoc::DocumentRisen3Sector( void )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GAR5" || m_streamIn.ReadU32() != 32 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    for ( MIUInt u = m_streamIn.ReadU16(); u--; )
    {
        StartBlock( "\"" + m_streamIn.ReadString( m_streamIn.ReadU16() ) + "\"" );
        if ( m_streamIn.Skip( 4 ), m_streamIn.ReadU32() != mCRisenName( "class gCEmbeddedLayer" ).GetRisenID() )
            return m_streamIn.Seek( uOffset ), MIFalse;
        m_streamIn.Skip( 88 );
        for ( MIUInt u = m_streamIn.ReadU32(); u--; WriteLine() )
            if ( !DocumentRisen3DynamicEntity() )
                return m_streamIn.Seek( uOffset ), MIFalse;
        EndBlock( MIFalse );
    }
    return MITrue;
}

MIBool mCRisenDoc::DocumentRisen3Template( void )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GAR5" || ( m_streamIn.ReadU32(), m_streamIn.ReadString( 4 ) != "GTP0" ) )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.ReadU64();  // Time stamp.
    return DocumentRisen3TemplateClass( MITrue );
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

MIBool mCRisenDoc::DocumentRisen3DynamicEntity( void )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GEC2" || m_streamIn.ReadU32() != mCRisenName( "class eCDynamicEntity" ).GetRisenID() || m_streamIn.ReadU16() != 6 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.ReadU32();
    for ( MIUInt u = m_streamIn.ReadU16(); u--; )
    {
        m_streamIn.Skip( 8 );
        m_streamIn.Skip( m_streamIn.ReadU32() );
    }
    m_streamIn.ReadU32();
    if ( m_streamIn.ReadU32() != mCRisenName( "class eCDynamicEntity" ).GetRisenID() || m_streamIn.ReadU16() != 6 || m_streamIn.ReadU32() != 186 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    MIUInt const uClassDataOffset1 = m_streamIn.Tell();
    m_streamIn.Skip( 186 );
    if ( m_streamIn.ReadU32() != mCRisenName( "class eCEntity" ).GetRisenID() || m_streamIn.ReadU16() != 3 || m_streamIn.ReadU32() == 0 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    MIUInt const uClassDataOffset2 = m_streamIn.Tell();
    m_streamIn.Skip( 16 );
    MIUInt const uNameLength = m_streamIn.ReadU16();
    StartBlock( "\"" + m_streamIn.ReadString( uNameLength ) + "\"" );
    m_streamIn.Seek( uClassDataOffset2 );
    FormatVariable( "GUID", s_arrTypes[ EGuid ] );
    m_streamIn.Seek( uClassDataOffset1 + 169 );
    FormatVariable( "Creator", s_arrTypes[ EGuid ] );
    m_streamIn.Seek( uClassDataOffset1 );
    FormatVariable( "MatrixLocal", s_arrTypes[ EMatrix ] );
    FormatVariable( "MatrixGlobal", s_arrTypes[ EMatrix ] );
    FormatVariable( "Extents", s_arrTypes[ EBox ] );
    FormatVariable( "Center", s_arrTypes[ EVector ] );
    FormatVariable( "Radius", s_arrTypes[ EFloat ] );
    m_streamIn.Seek( uClassDataOffset2 + 18 + uNameLength );
    FormatVariable( "TimeStamp", "blob", 8 );
    m_streamIn.Seek( uClassDataOffset1 + 185 );
    FormatVariable( "Unknown1", "blob", 1 );
    m_streamIn.Seek( uClassDataOffset2 + 26 + uNameLength );
    FormatVariable( "Unknown2", "blob", 6 );
    for ( MIUInt u = m_streamIn.ReadU8(); u--; WriteLine() )
        DocumentRisen3Class();
    for ( MIUInt u = m_streamIn.ReadU32(); u--; WriteLine() )
        if ( !DocumentRisen3DynamicEntity() )
            return m_streamIn.Seek( uOffset ), MIFalse;
    EndBlock( MIFalse );
    return MITrue;
}

MIBool mCRisenDoc::DocumentRisen3TemplateClass( MIBool a_bMasterEntity )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GEC2" || m_streamIn.ReadU32() != mCRisenName( "class eCTemplateEntity" ).GetRisenID() || m_streamIn.ReadU16() != 3 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.ReadU32();
    for ( MIUInt u = m_streamIn.ReadU16(); u--; )
    {
        m_streamIn.Skip( 8 );
        m_streamIn.Skip( m_streamIn.ReadU32() );
    }
    m_streamIn.ReadU32();
    MIUInt const uClassDataOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadU32() != mCRisenName( "class eCTemplateEntity" ).GetRisenID() || m_streamIn.ReadU16() != 3 || m_streamIn.ReadU32() != 52 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.Skip( 52 );
    if ( m_streamIn.ReadU32() != mCRisenName( "class eCEntity" ).GetRisenID() || m_streamIn.ReadU16() != 3 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.Skip( 20 );
    mCString strName = m_streamIn.ReadString( m_streamIn.ReadU16() );
    MIUInt uOffset2 = m_streamIn.Tell();
    if ( a_bMasterEntity )
        strName = "<master>";
    else
        strName = "\"" + strName + "\"";
    StartBlock( strName );
    m_streamIn.Seek( uClassDataOffset + 72 );
    FormatVariable( "GUID", s_arrTypes[ EGuid ] );
    m_streamIn.Seek( uClassDataOffset + 10 );
    FormatVariable( "RefTemplateGUID", s_arrTypes[ EGuid ] );
    m_streamIn.Skip( 1 );
    FormatVariable( "Position", s_arrTypes[ EVector ] );
    FormatVariable( "Rotation", s_arrTypes[ EQuaternion ] );
    m_streamIn.Skip( 5 );
    FormatVariable( "Unknown1", "blob", 2 );
    m_streamIn.Seek( uOffset2 + 8 );
    FormatVariable( "Unknown2", "blob", 6 );
    for ( MIUInt v = m_streamIn.ReadU8(); v--; WriteLine() )
        if ( 0 == DocumentRisen3Class() )
            return m_streamIn.Seek( uOffset ), MIFalse;
    for ( MIUInt u = m_streamIn.ReadU32(); u--; )
        if ( !DocumentRisen3TemplateClass( MIFalse ) )
            return m_streamIn.Seek( uOffset ), MIFalse;
    EndBlock();
    return MITrue;
}

void mCRisenDoc::FormatVariable( mCString const & a_strName, mCString const & a_strType, MIUInt a_uSize )
{
    Write( a_strName + " = " );
    FormatData( a_strType, a_uSize );
    WriteLine( ";" );
}