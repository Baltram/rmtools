#include "mi_include_risen.h"
#include "mi_include_3d.h"
#include "mi_include_standard.h"

namespace
{
    enum ETypes { EBool, EFloat, EChar, ESignedChar, EUnsignedChar, EShort, EUnsignedShort, EInt, ELong, EUnsignedInt, EUnsignedLong, EInt64, EUnsignedInt64, EString, EScriptProxyScript, EScriptProxyAIFunction, EScriptProxyAIState, EGuiBitmapProxy2, EResourceProxy, EWeatherEnvironmentProxy, EEffectProxy, EFocusModeProxy2, EMovementSpeciesProxy, EParticleSystemProxy, EGuid, ETemplateEntityProxy, EEntityProxy, EBox, EEulerAngles, EFloatColor, EMatrix, EQuaternion, ERange1, EVector, EVector2, ELetterLocString, ETypes_Count };

    MILPCChar const       s_arrTypes[ ETypes_Count ] = { "bool", "float", "char", "signed char", "unsigned char", "short", "unsigned short", "int", "long", "unsigned int", "unsigned long", "__int64", "unsigned __int64", "class bCString", "class eCScriptProxyScript", "class gCScriptProxyAIFunction", "class gCScriptProxyAIState", "class eCGuiBitmapProxy2", "class eTResourceProxy", "class eCWeatherEnvironmentProxy", "class gCEffectProxy", "class gCFocusModeProxy2", "class gCMovementSpeciesProxy", "class eCParticleSystemProxy", "class bCGuid", "class eCTemplateEntityProxy", "class eCEntityProxy", "class bCBox", "class bCEulerAngles", "class bCFloatColor", "class bCMatrix", "class bCQuaternion", "class bCRange1", "class bCVector", "class bCVector2", "class gCLetterLocString" };
    mTStringMap< ETypes > s_mapTypes;

    MIBool ReadValueString( mCStringStream & a_streamIn, mCString & a_strDest )
    {
        mCError::CProbe Probe;
        mCError const * pLastError = mCError::GetLastError< mCError >();
        static mTArray< MIBool > s_arrChars( MIFalse, 256 );
        if ( !s_arrChars[ '_' ] )
        {
            s_arrChars[ '_' ] = s_arrChars[ '{' ] = s_arrChars[ '}' ] = s_arrChars[ '-' ] = MITrue;
            for ( MIChar c = 'a'; c <= 'z'; ++c )
                s_arrChars[ c ] = MITrue;
            for ( MIChar c = 'A'; c <= 'Z'; ++c )
                s_arrChars[ c ] = MITrue;
            for ( MIChar c = '0'; c <= '9'; ++c )
                s_arrChars[ c ] = MITrue;
        }
        mCString strText = a_streamIn.ReadString();
        MIUInt u = 0, uLength = strText.GetLength();
        for ( MILPCChar pcIt = strText.GetText(); u != uLength && s_arrChars[ *pcIt ]; ++u, ++pcIt );
        a_strDest = strText.Left( u );
        a_streamIn.Skip( -static_cast< MIInt >( uLength - u ) );
        if ( pLastError != mCError::GetLastError< mCError >() )
            return MIFalse;
        return MITrue;
    }
}

mCRisenDocParser::mCRisenDocParser( mCStringStream & a_streamIn, mCIOStreamBinary & a_streamOut ) :
    mCDocParser( a_streamIn, a_streamOut )
{
    mCRisenName::InitializeRisen3Strings();
}

MIBool mCRisenDocParser::ParseRisen3Class( MIBool a_bSetLastErrorLine )
{
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    mCString strClassName;
    if ( !EnterBlock( &strClassName, a_bSetLastErrorLine ) )
        return MIFalse;
    MIU32 u32ClassHash = 0;
    if ( 1 != strClassName.Lower().Scan( "< unknown class %i >", &u32ClassHash ) )
        u32ClassHash = mCRisenName( strClassName ).GetRisenID();
    if ( !u32ClassHash )
        return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut << "GEC2" << u32ClassHash;
    MIU16 u16Version;
    if ( !MatchImmediate( "Version", a_bSetLastErrorLine ) || !MatchImmediate( "=", a_bSetLastErrorLine ) || !ParseVersion( u16Version, a_bSetLastErrorLine ) || !MatchImmediate( ";", a_bSetLastErrorLine, MITrue, MIFalse, MIFalse ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut << u16Version << ( MIU32 ) 0;
    MIUInt uOffset1 = m_streamOut.Tell();
    if ( !EnterBlock( "Properties", a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    MIU16 u16PropertyCount = 0;
    m_streamOut << u16PropertyCount;
    while ( !LeaveBlock( MIFalse ) )
    {
        if ( !ParseProperty( a_bSetLastErrorLine ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
        ++u16PropertyCount;
    }
    if ( !EnterBlock( "ClassData", a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    MIUInt uOffset2 = m_streamOut.Tell(), uClassDataCount = 0;
    m_streamOut << ( MIU32 ) 0;
    while ( !LeaveBlock( MIFalse ) )
    {
        ++uClassDataCount;
        mCStringArray arrTokens;
        MIU16 u16Version;
        if ( !MatchImmediate( "[", a_bSetLastErrorLine ) || !Match( "Version", arrTokens, a_bSetLastErrorLine ) || !ParseVersion( u16Version, a_bSetLastErrorLine ) || !MatchImmediate( "] =", a_bSetLastErrorLine ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
        mCString strClassName = arrTokens.GetCount() ? arrTokens[ 0 ] : "";
        MIU32 u32ClassHash = 0;
        for ( MIUInt u = 1, ue = arrTokens.GetCount(); u != ue; ++u )
            strClassName += " " + arrTokens[ u ];
        strClassName.TrimRight( " ," );
        if ( 1 != strClassName.Lower().Scan( "< unknown class %i >", &u32ClassHash ) )
            u32ClassHash = mCRisenName( strClassName ).GetRisenID();
        if ( !u32ClassHash )
            return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
        m_streamOut << u32ClassHash << u16Version;
        if ( !ParseData( "", MITrue, a_bSetLastErrorLine ) || !MatchImmediate( ";", a_bSetLastErrorLine, MITrue, MIFalse, MIFalse ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    }
    if ( !LeaveBlock( a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    MIUInt uOffsetCurrent = m_streamOut.Tell();
    m_streamOut.Seek( uOffset2 );
    m_streamOut << g_32( uClassDataCount );
    m_streamOut.Seek( uOffset1 - 4 );
    m_streamOut << g_32( uOffsetCurrent - uOffset1 ) << u16PropertyCount;
    m_streamOut.Seek( uOffsetCurrent );
    return MITrue;
}

MIBool mCRisenDocParser::ParseProperty( MIBool a_bSetLastErrorLine )
{
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    mCStringArray arrTokens, arrTokensNew;
    if ( !Match( "=", arrTokens, a_bSetLastErrorLine, MITrue ) )
        return m_streamIn.Seek( uOffset ), MIFalse;
    MIU32 u32TypeHash = 0, u32NameHash = 0;
    mCString strType;
    arrTokens.Resize( arrTokens.GetCount() + 4 );
    for ( MIUInt u = 0, ue = arrTokens.GetCount() - 4; u < ue; ++u )
    {
        if ( mCString( arrTokens[ u ] ).ToLower() == "<unknown" || arrTokens[ u ] == "<" && arrTokens[ u + 1 ] == "unknown" && ++u )
        {
            if ( arrTokens[ u + 1 ] == "type" )
            {
                if ( arrTokens[ u + 2 ].Scan( "0x%x", &u32TypeHash ) != 1 )
                    return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), MIFalse;
            }
            else if ( arrTokens[ u + 1 ] == "name" )
            {
                if ( arrTokens[ u + 2 ].Scan( "0x%x", &u32NameHash ) != 1 )
                    return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), MIFalse;
            }
            else
                return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), MIFalse;
            if ( !arrTokens[ u + 2 ].EndsWith( ">" ) && arrTokens[ ++u + 2 ] != ">" )
                return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), MIFalse; 
            u += 2;
            arrTokensNew.Add( "_reserved_mimicry" );
        }
        else
            arrTokensNew.Add( arrTokens[ u ] );
    }
    if ( arrTokensNew.GetCount() < 2 )
        return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), MIFalse;
    if ( !u32NameHash )
    {
        u32NameHash = mCRisenName( arrTokensNew.Back() ).GetRisenID();
        if ( !u32NameHash )
            return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), MIFalse;
    }
    if ( !u32TypeHash )
    {
        strType = arrTokensNew[ 0 ];
        for ( MIUInt u = 1, ue = arrTokensNew.GetCount() - 1; u != ue; ++u )
            strType += " " + arrTokensNew[ u ];
        u32TypeHash = mCRisenName( strType ).GetRisenID();
        if ( !u32TypeHash )
            return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, MIFalse ), m_streamIn.Seek( uOffset ), MIFalse;
    }
    m_streamOut << u32TypeHash << u32NameHash;
    if ( !ParseData( strType, MITrue, a_bSetLastErrorLine ) || !MatchImmediate( ";", a_bSetLastErrorLine, MITrue, MIFalse, MIFalse ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    return MITrue;
}

MIBool mCRisenDocParser::ParseRisen3DlgData2( MIBool a_bSetLastErrorLine )
{
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    mCMemoryStream streamBlendsShapeNames, streamFrameData;
    mCRisenDocParser ParserNames( m_streamIn, streamBlendsShapeNames ), ParserData( m_streamIn, streamFrameData );
    m_streamOut << "GAR5" << ( MIU32 ) 32;
    if ( !EnterBlock( "Gestures", a_bSetLastErrorLine ) ||
         !ParseRisen3Class( a_bSetLastErrorLine ) ||
         !LeaveBlock( a_bSetLastErrorLine ) ||
         !EnterBlock( "FacialAnimation", a_bSetLastErrorLine ) ||
         !MatchImmediate( "BlendShapeNames =", a_bSetLastErrorLine ) ||
         ( !ParserNames.ParseData( "class bTObjArray<class bCString>", MIFalse, a_bSetLastErrorLine ) ) && ( m_uLastErrorLine = ParserNames.m_uLastErrorLine ) != MI_DW_INVALID ||
         !MatchImmediate( ";", a_bSetLastErrorLine ) ||
         !MatchImmediate( "BlendShapeTimeLines =", a_bSetLastErrorLine ) ||
         ( !ParserData.ParseData( "class bTObjArray<blob>", MIFalse, a_bSetLastErrorLine ) ) && ( m_uLastErrorLine = ParserData.m_uLastErrorLine ) != MI_DW_INVALID ||
         !MatchImmediate( ";", a_bSetLastErrorLine ) ||
         !LeaveBlock( a_bSetLastErrorLine ) )
        return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    streamFrameData.Seek( 0 );
    MIUInt const uBlendShapeCount = streamFrameData.ReadU32();
    MIUInt const uFrameCount = ( streamFrameData.GetSize() - 4 ) / uBlendShapeCount;
    m_streamOut << ( MIU32 ) 30 << g_32( uFrameCount ) << g_32( uFrameCount * uBlendShapeCount );
    m_streamOut.Write( static_cast< MILPCByte >( streamFrameData.GetBuffer() ) + 4, uFrameCount * uBlendShapeCount );
    m_streamOut << streamBlendsShapeNames;
    return MITrue;
}

MIBool mCRisenDocParser::ParseRisen3Sector( MIBool a_bSetLastErrorLine )
{
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    mCError::CProbe Probe;
    m_streamOut << "GAR5" << ( MIU32 ) 32 << ( MIU16 ) 0;
    mCString strLayerName;
    MIUInt uLayerCount = 0;
    while ( mEResult_Ok == m_streamIn.ReadStringInQuotes( strLayerName ) )
    {
        ++uLayerCount;
        m_streamOut << ( MIU16 )( strLayerName.GetLength() ) << strLayerName;
        MIUInt uOffsetLayer = m_streamOut.Tell(), uEntityCount = 0;
        m_streamOut << "GEC2" << mCRisenName( "class gCEmbeddedLayer" ) << ( MIU16 ) 1 << ( MIU32 ) 0 << ( MIU16 ) 0;
        m_streamOut << ( MIU32 ) 1 << mCRisenName( "class gCEmbeddedLayer" ) << ( MIU16 ) 1 << ( MIU32 ) 0 << mCRisenName( "gEEntityType_Game" ) << ( MIU32 ) 0;
        m_streamOut << "GEC2" << mCRisenName( "class eCScene" ) << ( MIU16 ) 1 << ( MIU32 ) 0;
        m_streamOut << ( MIU16 ) 1 << mCRisenName( "class bCGuid" ) << mCRisenName( "ID" ) << ( MIU32 ) 16 << ( MIU64 ) 0 << ( MIU64 ) 0;
        m_streamOut << ( MIU32 ) 1 << mCRisenName( "class eCScene" ) << ( MIU16 ) 1 << ( MIU32 ) 0 << ( MIU32 ) 0;
        if ( !EnterBlock( "", a_bSetLastErrorLine ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
        while ( MatchImmediate( "\"", MIFalse ) )
            if ( MIFalse == ( m_streamIn.Skip( -1 ), ++uEntityCount, ParseRisen3DynamicEntity( a_bSetLastErrorLine ) ) )
                return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
        if ( !LeaveBlock( a_bSetLastErrorLine ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
        MIUInt uOffsetLayerEnd = m_streamOut.Tell();
        m_streamOut.Seek( uOffsetLayer + 10 );
        m_streamOut << g_32( uOffsetLayerEnd - uOffsetLayer - 10 - 4 );
        m_streamOut.Seek( uOffsetLayer + 26 );
        m_streamOut << g_32( uOffsetLayerEnd - uOffsetLayer - 26 - 4 );
        m_streamOut.Seek( uOffsetLayer + 48 );
        m_streamOut << g_32( uOffsetLayerEnd - uOffsetLayer - 48 - 4 );
        m_streamOut.Seek( uOffsetLayer + 92 );
        m_streamOut << g_32( uOffsetLayerEnd - uOffsetLayer - 92 - 4 );
        m_streamOut << g_32( uEntityCount );
        m_streamOut.Seek( uOffsetLayerEnd );
    }
    MIUInt const uOffsetEnd = m_streamOut.Tell();
    m_streamOut.Seek( uOffsetOut + 4 + 4 );
    m_streamOut << ( MIU16 )( uLayerCount );
    m_streamOut.Seek( uOffsetEnd );
    return MITrue;
}

MIBool mCRisenDocParser::ParseRisen3Template( mCString const a_strName, MIBool a_bSetLastErrorLine )
{
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    MIU64 u64Time = g_time();
    u64Time = ( u64Time << 32 ) | ( u64Time >> 32 );
    m_streamOut << "GAR5" << ( MIU32 ) 32 << "GTP0" << u64Time;
    if ( !ParseRisen3TemplateClass( a_strName, u64Time, a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    return MITrue;
}

MIBool mCRisenDocParser::ParseData( mCString a_strType, MIBool a_bWriteSize, MIBool a_bSetLastErrorLine )
{
    if ( s_mapTypes.GetCount() == 0 )
        for ( MIUInt u = ETypes_Count; u--; reinterpret_cast< MIUInt & >( s_mapTypes[ s_arrTypes[ u ] ] ) = u );
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    if ( a_bWriteSize )
        m_streamOut << ( MIU32 ) 0;
    ETypes enuType;
    if ( a_strType.StartsWith( "class eTResourceProxy" ) )
        a_strType = "class eTResourceProxy";
    if ( !s_mapTypes.GetAt( a_strType, enuType ) )
    {
        if ( a_strType.ReplaceLeft( "class bTObjArray<" ) && a_strType.ReplaceRight( ">" ) )
        {
            if ( !EnterArray( a_bSetLastErrorLine ) )
                return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
            m_streamOut << ( MIU32 ) 0;
            if ( !LeaveArray( MIFalse ) )
            {
                MIUInt uCount = 0;
                do
                {
                    if ( !ParseData( a_strType, MIFalse, a_bSetLastErrorLine ) )
                        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
                    ++uCount;
                } while ( MatchImmediate( ",", MIFalse, MITrue ) );
                if ( !LeaveArray( a_bSetLastErrorLine ) )
                    return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
                MIUInt uOffsetTemp = m_streamOut.Tell();
                m_streamOut.Seek( a_bWriteSize ? uOffsetOut + 4 : uOffsetOut );
                m_streamOut << g_32( uCount );
                m_streamOut.Seek( uOffsetTemp );
            }
        }
        else if ( MatchImmediate( "<", MIFalse, MITrue ) )
        {
            mCByteArray arrData;
            arrData.Reserve( 100 );
            while ( !m_streamIn.IsAtEnd() )
            {
                mCString strToken = m_streamIn.ReadString();
                MIUInt uValidCharCount = 0;
                MIUInt uValue;
                if ( strToken.Scan( "%2x%n", &uValue, &uValidCharCount ) == 1 )
                    arrData.Add( static_cast< MIByte >( uValue ) );
                if ( uValidCharCount != strToken.GetLength() )
                {
                    m_streamIn.Skip( -static_cast< MIInt >( strToken.GetLength() - uValidCharCount ) );
                    break;
                }
            }
            if ( !MatchImmediate( ">", a_bSetLastErrorLine, MITrue, MIFalse, MIFalse ) )
                return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
            m_streamOut.Write( arrData.GetBuffer(), arrData.GetCount() );
        }
        else if ( a_strType.StartsWith( "class " ) )
        {
            if ( !ParseRisen3Class( a_bSetLastErrorLine ) )
                return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
        }
        else if ( a_strType.StartsWith( "enum " ) )
        {
            mCString strEnum;
            if ( !ReadValueString( m_streamIn, strEnum ) )
                return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
            mCRisenName EnumMember( strEnum );
            MIInt iEnumValue = 0;
            if ( EnumMember.GetEnumValue( iEnumValue ) )
                m_streamOut << EnumMember.GetRisenID() << g_32( iEnumValue );
            else if ( 1 == strEnum.Scan( "%i", &iEnumValue ) )
                m_streamOut << ( MIU32 ) 0 << g_32( iEnumValue );
            else
                return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
        }
        else
            return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    }
    else
    {
        MIBool bResult = MITrue;
        mCError::CProbe Probe;
        MIUInt uFloatCount = 0;
        mCString strShortName;
        switch ( enuType )
        {
        case EBool:
            if ( MatchImmediate( "true", MIFalse, MIFalse ) )
                m_streamOut << ( MIU32 ) 1;
            else if ( MatchImmediate( "false", MIFalse, MIFalse ) )
                m_streamOut << ( MIU32 ) 0;
            else
                bResult = MIFalse;
            break;
        case EFloat:
            m_streamOut << m_streamIn.ReadFloat();
            break;
        case EChar:
            bResult &= MatchImmediate( "'", MIFalse, MITrue );
            m_streamOut << m_streamIn.ReadChar();
            bResult &= MatchImmediate( "'", MIFalse, MITrue );
            break;
        case ESignedChar:
            m_streamOut << m_streamIn.ReadI8();
            break;
        case EUnsignedChar:
            m_streamOut << m_streamIn.ReadU8();
            break;
        case EShort:
            m_streamOut << m_streamIn.ReadI16();
            break;
        case EUnsignedShort:
            m_streamOut << m_streamIn.ReadU16();
            break;
        case EInt:
        case ELong:
            m_streamOut << m_streamIn.ReadI32();
            break;
        case EUnsignedInt:
        case EUnsignedLong:
            m_streamOut << m_streamIn.ReadU32();
            break;
        case EInt64:
            m_streamOut << m_streamIn.ReadI64();
            break;
        case EUnsignedInt64:
            m_streamOut << m_streamIn.ReadU64();
            break;
        case EScriptProxyScript:
        case EScriptProxyAIFunction:
        case EScriptProxyAIState:
        case EGuiBitmapProxy2:
            m_streamOut << ( MIU16 ) 1;
        case EString:
        case ELetterLocString:
        case EResourceProxy:
        case EWeatherEnvironmentProxy:
        case EEffectProxy:
        case EFocusModeProxy2:
        case EMovementSpeciesProxy:
        case EParticleSystemProxy:
        {
            mCString strText, strPart;
            if ( MatchImmediate( "\"\"\"", MIFalse, MITrue ) )
            {
                bResult = MIFalse;
                while ( m_streamIn.Skip( -1 ), mEResult_Ok == m_streamIn.ReadStringInQuotes( strPart ) )
                {
                    strText += strPart;
                    if ( MatchImmediate( "\"\"", MIFalse, MITrue ) )
                    {
                        bResult = MITrue;
                        while ( MatchImmediate( "\"", MIFalse, MITrue ) )
                            strText += "\"";
                        break;
                    }
                    else
                        strText  += "\"";
                }
            }
            else
                bResult &= ( mEResult_Ok == m_streamIn.ReadStringInQuotes( strText ) );
            if ( enuType == ELetterLocString )
                m_streamOut << g_djb2( strText.Lower().GetText() );
            m_streamOut << ( MIU16 )( strText.GetLength() ) << strText;
            break;
        }
        case EGuid:
        case ETemplateEntityProxy:
        case EEntityProxy:
        {
            MIU32 u32Part1;
            MIU16 u16Part2, u16Part3;
            MIU8 arrPart4[ 8 + 1 ];
            mCString strGuid;
            bResult &= ReadValueString( m_streamIn, strGuid );
            // hh specifier not supported by Visual Studio
            bResult &= ( 11 == strGuid.Scan( "{%8lX-%4hX-%4hX-%2hX%2hX-%2hX%2hX%2hX%2hX%2hX%2hX}", &u32Part1, &u16Part2, &u16Part3, &arrPart4[ 0 ], &arrPart4[ 1 ], &arrPart4[ 2 ], &arrPart4[ 3 ], &arrPart4[ 4 ], &arrPart4[ 5 ], &arrPart4[ 6 ], &arrPart4[ 7 ] ) );
            m_streamOut << u32Part1 << u16Part2 << u16Part3;
            m_streamOut.Write( arrPart4, 8 );
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
                bResult &= MatchImmediate( "( " + strShortName, MIFalse );
                for ( MIUInt u = uFloatCount; u--; )
                    m_streamOut << m_streamIn.ReadFloat();
                bResult &= MatchImmediate( ")", MIFalse );
            }
        }
        if ( !bResult )
            return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    }
    if ( !a_bWriteSize )
        return MITrue;
    MIUInt uOffsetEnd = m_streamOut.Tell();
    m_streamOut.Seek( uOffsetOut );
    m_streamOut << g_32( uOffsetEnd - uOffsetOut - 4 );
    m_streamOut.Seek( uOffsetEnd );
    return MITrue;
}

namespace
{
    MIBool bNewPosition = MIFalse;
    MIBool bNewRotation = MIFalse;
    mCVec3 vecRotation;
    mCVec3 vecPosition;
}

void mCRisenDocParser::CheckNewPosRot( void )
{
    mCMemoryStream streamTemp;
    mCRisenDocParser Parser( m_streamIn, streamTemp );
    if ( Parser.ParseVariable( "NewPos", s_arrTypes[ EVector ], MIFalse ) )
        bNewPosition = MITrue, g_memcpy( &vecPosition, static_cast< MILPCByte >( streamTemp.GetBuffer() ) + streamTemp.Tell() - sizeof( vecPosition ), sizeof( vecPosition ) );
    if ( Parser.ParseVariable( "NewRot", s_arrTypes[ EEulerAngles ], MIFalse ) )
        bNewRotation = MITrue, g_memcpy( &vecRotation, static_cast< MILPCByte >( streamTemp.GetBuffer() ) + streamTemp.Tell() - sizeof( vecRotation ), sizeof( vecRotation ) );
    if ( Parser.ParseVariable( "NewPos", s_arrTypes[ EVector ], MIFalse ) )
        bNewPosition = MITrue, g_memcpy( &vecPosition, static_cast< MILPCByte >( streamTemp.GetBuffer() ) + streamTemp.Tell() - sizeof( vecPosition ), sizeof( vecPosition ) );
}

MIBool mCRisenDocParser::ParseVariable( mCString const a_strName, mCString const a_strType, MIBool a_bSetLastErrorLine, MIBool a_bCheckNewPosRot )
{
    if ( a_bCheckNewPosRot )
        CheckNewPosRot();
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    if ( !MatchImmediate( a_strName + " =", a_bSetLastErrorLine ) ||
         !ParseData( a_strType, MIFalse, a_bSetLastErrorLine ) ||
         !MatchImmediate( ";", a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    return MITrue;
}

MIBool mCRisenDocParser::ParseVersion( MIU16 & a_u16Version, MIBool a_bSetLastErrorLine )
{
    mCError::CProbe Probe;
    mCError const * pError = mCError::GetLastError< mCError >();
    m_streamIn >> a_u16Version;
    if ( pError != mCError::GetLastError< mCError >() )
        return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), MIFalse;
    return MITrue;
}

MIBool mCRisenDocParser::ParseRisen3DynamicEntity( MIBool a_bSetLastErrorLine, MILPCVoid a_pParentMat )
{
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    mCError::CProbe Probe;
    mCString strName;
    if ( mEResult_False == m_streamIn.ReadStringInQuotes( strName ) || 
         !EnterBlock( "", a_bSetLastErrorLine ) )
        return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut << "GEC2" << mCRisenName( "class eCDynamicEntity" ) << ( MIU16 ) 6;
    m_streamOut << ( MIU32 ) 0 << ( MIU16 ) 0 << ( MIU32 ) 2;
    m_streamOut << mCRisenName( "class eCDynamicEntity" ) << ( MIU16 ) 6 << ( MIU32 ) 186;
    MIUInt const uClassDataOffset1 = m_streamOut.Tell();
    m_streamOut.Write( mCString::AccessStaticBuffer(), 186 );
    m_streamOut << mCRisenName( "class eCEntity" ) << ( MIU16 ) 3 << ( MIU32 ) 0;
    MIUInt const uClassDataOffset2 = m_streamOut.Tell();
    bNewPosition = bNewRotation = MIFalse;
    if ( !ParseVariable( "GUID", s_arrTypes[ EGuid ], a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut.Seek( uClassDataOffset1 + 169 );
    if ( !ParseVariable( "Creator", s_arrTypes[ EGuid ], a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut.Seek( uClassDataOffset1 );
    mCMemoryStream stream3D;
    mCRisenDocParser Parser( m_streamIn, stream3D );
    if ( !Parser.ParseVariable( "MatrixLocal", s_arrTypes[ EMatrix ], a_bSetLastErrorLine, MITrue ) ||
         !Parser.ParseVariable( "MatrixGlobal", s_arrTypes[ EMatrix ], a_bSetLastErrorLine, MITrue ) ||
         !Parser.ParseVariable( "Extents", s_arrTypes[ EBox ], a_bSetLastErrorLine, MITrue ) ||
         !Parser.ParseVariable( "Center", s_arrTypes[ EVector ], a_bSetLastErrorLine, MITrue ) ||
         !Parser.ParseVariable( "Radius", s_arrTypes[ EFloat ], a_bSetLastErrorLine, MITrue ) )
        return m_uLastErrorLine = Parser.m_uLastErrorLine, m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut << stream3D;
    m_streamOut << ( MIU8 ) 0xFF;
    m_streamOut.Seek( uClassDataOffset2 + 16 );
    m_streamOut << ( MIU16 )( strName.GetLength() ) << strName;
    if ( !ParseVariable( "TimeStamp", "blob", a_bSetLastErrorLine, MITrue ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    mCMatrix4 matGlobal( static_cast< mCMatrix4 const * >( stream3D.GetBuffer() )[ 1 ] );
    if ( bNewPosition || bNewRotation )
    {
        // Convert euler angles to quaternion
        mCVec3 vecCos( cos( vecRotation.GetY() / 2 ), cos( vecRotation.GetX() / 2 ), cos( vecRotation.GetZ() / 2 ) );
        mCVec3 vecSin( sin( vecRotation.GetY() / 2 ), sin( vecRotation.GetX() / 2 ), sin( vecRotation.GetZ() / 2 ) );
        mCQuaternion quatRot( vecSin.GetX() * vecCos.GetY() * vecCos.GetZ() - vecCos.GetX() * vecSin.GetY() * vecSin.GetZ(),
                              vecCos.GetX() * vecSin.GetY() * vecCos.GetZ() + vecSin.GetX() * vecCos.GetY() * vecSin.GetZ(),
                              vecCos.GetX() * vecCos.GetY() * vecSin.GetZ() - vecSin.GetX() * vecSin.GetY() * vecCos.GetZ(),
                              vecCos.GetX() * vecCos.GetY() * vecCos.GetZ() + vecSin.GetX() * vecSin.GetY() * vecSin.GetZ() );
        if ( bNewRotation )
            matGlobal = mCMatrix4( quatRot );
        stream3D.Seek( 4 * ( 16 + 16 + 6 ) );
        mCVec3 vecCenter;
        stream3D >> vecCenter;
        MIFloat fRadius = stream3D.ReadFloat();
        if ( bNewPosition )
        {
            vecCenter += vecPosition - matGlobal.GetTranslation();
            matGlobal.AccessTranslation() = vecPosition;
        }
        mCMatrix4 matParent;
        if ( a_pParentMat )
            matParent = *static_cast< mCMatrix4 const * >( a_pParentMat );
        mCMatrix4 matLocal = matGlobal * matParent.GetInverted();
        stream3D.Seek( 0 );
        stream3D.Write( &matLocal, sizeof( matLocal ) );
        stream3D.Write( &matGlobal, sizeof( matGlobal ) );
        // ToDo: Better bounding box approximation
        stream3D << ( vecCenter - mCVec3( fRadius, fRadius, fRadius ) ) << ( vecCenter + mCVec3( fRadius, fRadius, fRadius ) );
        stream3D << vecCenter;
    }
    m_streamOut.Seek( uClassDataOffset1 );
    m_streamOut << stream3D;
    m_streamOut.Seek( uClassDataOffset1 + 185 );
    if ( !ParseVariable( "Unknown1", "blob", a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut.Seek( uClassDataOffset2 + 26 + strName.GetLength() );
    if ( !ParseVariable( "Unknown2", "blob", a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    MIUInt uOffsetPS = m_streamOut.Tell(), uPSCount = 0;
    m_streamOut << ( MIU8 ) 0;
    for ( ; !MatchImmediate( "\"", MIFalse, MITrue ) && !MatchImmediate( "}", MIFalse, MITrue ); ++uPSCount )
        if ( !ParseRisen3Class( a_bSetLastErrorLine ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamIn.Skip( -1 );
    MIUInt uOffsetChildren = m_streamOut.Tell(), uChildCount = 0;
    m_streamOut.Seek( uOffsetPS );
    m_streamOut << ( MIU8 )( uPSCount );
    m_streamOut.Seek( uOffsetChildren );
    m_streamOut << ( MIU32 ) 0;
    for ( ; MatchImmediate( "\"", MIFalse, MITrue ); ++uChildCount )
        if ( m_streamIn.Skip( -1 ), !ParseRisen3DynamicEntity( a_bSetLastErrorLine, &matGlobal ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    MIUInt uOffsetEnd = m_streamOut.Tell();
    m_streamOut.Seek( uOffsetChildren );
    m_streamOut << g_32( uChildCount );
    m_streamOut.Seek( uOffsetOut + 10 );
    m_streamOut << g_32( uOffsetEnd - uOffsetOut - 10 - 4 );
    m_streamOut.Seek( uOffsetOut + 222 );
    m_streamOut << g_32( uOffsetEnd - uOffsetOut - 222 - 4 );
    m_streamOut.Seek( uOffsetEnd );
    if ( !LeaveBlock( a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    return MITrue;
}

MIBool mCRisenDocParser::ParseRisen3TemplateClass( mCString a_strName, MIU64 a_u4TimeSwapped, MIBool a_bSetLastErrorLine )
{
    MIUInt const uOffset = m_streamIn.Tell(), uOffsetOut = m_streamOut.Tell();
    if ( !MatchImmediate( "<master>", MIFalse ) )
        if ( mEResult_False == m_streamIn.ReadStringInQuotes( a_strName ) )
            return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    if ( !EnterBlock( "", a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut << "GEC2" << mCRisenName( "class eCTemplateEntity" ) << ( MIU16 ) 3;
    m_streamOut << ( MIU32 ) 0 << ( MIU16 ) 0 << ( MIU32 ) 2;
    m_streamOut << mCRisenName( "class eCTemplateEntity" ) << ( MIU16 ) 3 << ( MIU32 ) 52;
    m_streamOut.Write( mCString::AccessStaticBuffer(), 52 );
    m_streamOut << mCRisenName( "class eCEntity" ) << ( MIU16 ) 3 << ( MIU32 ) 0;
    if ( !ParseVariable( "GUID", s_arrTypes[ EGuid ], a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut << ( MIU16 )( a_strName.GetLength() ) << a_strName << a_u4TimeSwapped;
    m_streamOut.Seek( uOffsetOut + 30 );
    if ( !ParseVariable( "RefTemplateGUID", s_arrTypes[ EGuid ], a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut << ( MIU8 ) 0;
    if ( !ParseVariable( "Position", s_arrTypes[ EVector ], a_bSetLastErrorLine ) ||
         !ParseVariable( "Rotation", s_arrTypes[ EQuaternion ], a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut << ( MIU32 ) 0xFEEDFACE << ( MIU8 ) 0xFF;
    if ( !ParseVariable( "Unknown1", "blob", a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamOut.Seek( uOffsetOut + 118 + a_strName.GetLength() );
    if ( !ParseVariable( "Unknown2", "blob", a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    MIUInt uOffsetPS = m_streamOut.Tell(), uPSCount = 0;
    m_streamOut << ( MIU8 ) 0;
    for ( ; !MatchImmediate( "\"", MIFalse, MITrue ) && !MatchImmediate( "}", MIFalse, MITrue ); ++uPSCount )
        if ( !ParseRisen3Class( a_bSetLastErrorLine ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    m_streamIn.Skip( -1 );
    MIUInt uOffsetChildren = m_streamOut.Tell(), uChildCount = 0;
    m_streamOut.Seek( uOffsetPS );
    m_streamOut << ( MIU8 )( uPSCount );
    m_streamOut.Seek( uOffsetChildren );
    m_streamOut << ( MIU32 ) 0;
    for ( ; MatchImmediate( "\"", MIFalse, MITrue ); ++uChildCount )
        if ( m_streamIn.Skip( -1 ), !ParseRisen3TemplateClass( "", a_u4TimeSwapped, a_bSetLastErrorLine ) )
            return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    MIUInt uOffsetEnd = m_streamOut.Tell();
    m_streamOut.Seek( uOffsetChildren );
    m_streamOut << g_32( uChildCount );
    m_streamOut.Seek( uOffsetOut + 10 );
    m_streamOut << g_32( uOffsetEnd - uOffsetOut - 14 );
    m_streamOut.Seek( uOffsetOut + 88 );
    m_streamOut << g_32( uOffsetEnd - uOffsetOut - 92 );
    m_streamOut.Seek( uOffsetEnd );
    if ( !LeaveBlock( a_bSetLastErrorLine ) )
        return m_streamIn.Seek( uOffset ), m_streamOut.Seek( uOffsetOut ), MIFalse;
    return MITrue;
}