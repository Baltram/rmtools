#include "mi_include_risen.h"
#include "mi_include_standard.h"

namespace
{
    enum ETypes { EBool, EFloat, EChar, ESignedChar, EUnsignedChar, EShort, EUnsignedShort, EInt, ELong, EUnsignedInt, EUnsignedLong, EInt64, EUnsignedInt64, EString, EScriptProxyScript, EScriptProxyAIFunction, EScriptProxyAIState, EUIDialogCloseScriptProxy, EUICanExecuteScriptProxy, EUIExecuteScriptProxy, EUICancelEventScriptProxy, EUIEventScriptProxy, EUIMessageScriptProxy, EUIDialogClosingScriptProxy, EGuiBitmapProxy2, EGuiFontProxy2, EResourceProxy, EWeatherEnvironmentProxy, EEffectProxy, EImageEffectProxy, EWeatherStatesProxy, EFocusModeProxy2, EMovementSpeciesProxy, EParticleSystemProxy, EUIBindingProxy, EUISoundThemeProxy, EVegetationDefinitionProxy, EGuid, ETemplateEntityProxy, EEntityProxy, EBox, EEulerAngles, EFloatColor, EMatrix, EQuaternion, ERange1, EVector, EVector2, ELocString, ELetterLocString, EBookLocString, ECutsceneLocString, EInfoLocString, EInfoDescLocString, EQuestLocString, EQuestDescLocString, EQuestProxy, EDeliveryEntity, EEntityStringProxy, EDateTime, ETypes_Count };

    MILPCChar const       s_arrTypes[ ETypes_Count ] = { "bool", "float", "char", "signed char", "unsigned char", "short", "unsigned short", "int", "long", "unsigned int", "unsigned long", "__int64", "unsigned __int64", "class bCString", "class eCScriptProxyScript", "class gCScriptProxyAIFunction", "class gCScriptProxyAIState", "class gCUIDialogCloseScriptProxy", "class gCUICanExecuteScriptProxy", "class gCUIExecuteScriptProxy", "class gCUICancelEventScriptProxy", "class gCUIEventScriptProxy", "class gCUIMessageScriptProxy", "class gCUIDialogClosingScriptProxy", "class eCGuiBitmapProxy2", "class eCGuiFontProxy2", "class eTResourceProxy", "class eCWeatherEnvironmentProxy", "class gCEffectProxy", "class eCImageEffectProxy", "class eCWeatherStatesProxy", "class gCFocusModeProxy2", "class gCMovementSpeciesProxy", "class eCParticleSystemProxy", "class gCUIBindingProxy", "class gCUISoundThemeProxy", "class eCVegetationDefinitionProxy", "class bCGuid", "class eCTemplateEntityProxy", "class eCEntityProxy", "class bCBox", "class bCEulerAngles", "class bCFloatColor", "class bCMatrix", "class bCQuaternion", "class bCRange1", "class bCVector", "class bCVector2", "class eCLocString", "class gCLetterLocString", "class gCBookLocString", "class gCCutsceneLocString", "class gCInfoLocString", "class gCInfoDescLocString", "class gCQuestLocString", "class gCQuestDescLocString", "class gCQuestProxy", "struct gCQuest::gSDeliveryEntity", "class eCEntityStringProxy", "class bCDateTime" };
    mTStringMap< ETypes > s_mapTypes;
    MILPCChar const       s_strHex = "0123456789ABCDEF";
}

mCRisenDoc::mCRisenDoc( mCIOStreamBinary & a_streamIn, mCIOStreamBinary & a_streamOut, mCString const & m_strIndent ) :
    mCDoc( a_streamIn, a_streamOut, m_strIndent )
{
    mCRisenName::InitializeGameStrings();
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
        if ( a_strType.ReplaceLeft( "class bTValArray<" ) && a_strType.ReplaceRight( ">" ) || 
             a_strType.ReplaceLeft( "class bTObjArray<" ) && a_strType.ReplaceRight( ">" ) || 
             a_strType.ReplaceLeft( "class bTSceneObjArray<" ) && a_strType.ReplaceRight( ">" ) || 
             a_strType.ReplaceLeft( "class bTRefPtrArray<" ) && a_strType.ReplaceRight( ">" ) ||
             a_strType.ReplaceLeft( "class bTRefPropertyArray<" ) && a_strType.ReplaceRight( ">" ) || 
             a_strType.ReplaceLeft( "class bTSceneRefPtrArray<" ) && a_strType.ReplaceRight( ">" ) || 
             a_strType.ReplaceLeft( "class bTSceneRefPropertyArray<" ) && a_strType.ReplaceRight( ">" ) )
        {
            StartArray();


            MIUInt uCount = m_streamIn.ReadU32();
            if ( uCount > 0 )
            {
                MIUInt uAddToLastChunk = ( a_uSize - 4 ) % uCount;
                MIUInt uChunkSize = ( a_uSize - uAddToLastChunk - 4 ) / uCount;

                for ( MIUInt u = 0; u != uCount; ++u )
                    FormatData( a_strType, uChunkSize + ( u + 1 < uCount ? 0 : uAddToLastChunk ) ), WriteLine( u + 1 < uCount ? "," : "" );
            }

            EndArray( MIFalse );
        }
        else if ( a_strType.EndsWith( "*" ) && ( m_streamIn.ReadString( 4 ) == "NULL" || ( m_streamIn.Skip( -4 ), MIFalse ) ) )
        {
            Write( "NULL" );
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
            
            if ( a_uSize == 0 )
            {
                Write("< >");
                return;
            }
            
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
        strResult.Format( "%.9g", m_streamIn.ReadFloat() );
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
    case EDateTime:
    {
        MIU64 u64Time = m_streamIn.ReadU32();
        u64Time = ( u64Time << 32 ) + m_streamIn.ReadU32();
        time_t time = static_cast< time_t >( g_filetimeToCtime( u64Time ) );
        MI_CRT_NO_WARNINGS( tm * pTimeInfo = gmtime( &time ); )
        MI_CRT_NO_WARNINGS( mCString strTime( asctime( pTimeInfo ) ); )
        strResult.Format( "(time %llu \"%s\")", u64Time, strTime.TrimRight("\n").GetText() );
        break;
    }
    case EScriptProxyScript:
    case EScriptProxyAIFunction:
    case EScriptProxyAIState:
    case EUIDialogCloseScriptProxy:
    case EUICanExecuteScriptProxy:
    case EUIExecuteScriptProxy:
    case EUICancelEventScriptProxy:
    case EUIEventScriptProxy:
    case EUIMessageScriptProxy:
    case EUIDialogClosingScriptProxy:
    case EGuiBitmapProxy2:
    case EGuiFontProxy2:
    case EEntityStringProxy:
        m_streamIn.ReadU16();  // Version
    case EString:
    case ELocString:
    case ELetterLocString:
    case EBookLocString:
    case ECutsceneLocString:
    case EInfoLocString:
    case EInfoDescLocString:
    case EQuestLocString:
    case EQuestDescLocString:
    case EResourceProxy:
    case EWeatherEnvironmentProxy:
    case EEffectProxy:
    case EImageEffectProxy:
    case EWeatherStatesProxy:
    case EFocusModeProxy2:
    case EMovementSpeciesProxy:
    case EParticleSystemProxy:
    case EUIBindingProxy:
    case EUISoundThemeProxy:
    case EVegetationDefinitionProxy:
    case EQuestProxy:
    {
        if ( a_strType.EndsWith( "LocString" ) )
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
                strResult += mCString().Format( " %.9g", m_streamIn.ReadFloat() );
            strResult += ")";
        }
        break;
    case EDeliveryEntity:
        StartBlock( a_strType );
        FormatVariable( "Amount", s_arrTypes[ EUnsignedInt ] );
        FormatVariable( "Counter", s_arrTypes[ EUnsignedInt ] );
        FormatVariable( "Entity", s_arrTypes[ EString ] );
        EndBlock( MIFalse );
    }
    Write( strResult );
}

MIBool mCRisenDoc::DocumentElexWorld( void )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GAR5" || m_streamIn.ReadU32() != 32 || 0 == DocumentRisen3Class() )
        return m_streamIn.Seek( uOffset ), MIFalse;
    WriteLine();
    Write( "Sectors = " );
    StartArray();
    for ( MIUInt u = 0, ue = m_streamIn.ReadU16(); u != ue; ++u, WriteLine( u < ue ? "," : "" ) )
        FormatData( s_arrTypes[ EString ] );
    EndArray( MIFalse );
    return MITrue;
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
                MIUInt uVersion = m_streamIn.ReadU16();
                DocumentRisen3ClassData( strClass, uVersion );
            }
        }
        EndBlock();
    }
    EndBlock( MIFalse );
    return m_streamIn.Tell() - uOffset;
}

void mCRisenDoc::DocumentRisen3ClassData( mCString a_strName, MIUInt uVersion )
{
    MIUInt const uSize = m_streamIn.ReadU32();
    MIUInt const uOffset = m_streamIn.Tell();
    Write( mCString().Format( "[%s, Version %u]", a_strName.GetText(), uVersion ) );
    MIUInt const uOffsetOut = m_streamOut.Tell();
    StartBlock();
    if ( a_strName == "class gCInventory_PS" && uVersion == 5 )
    {
        FormatVariable( "InventoryStacks", "class bTObjArray<class gCInventoryStack>" );
    }
    else if ( a_strName == "class gCNavigation_PS" && uVersion == 12 )
    {
        FormatVariable( "Routines", "class bTObjArray<class gCRoutine>" );
    }
    else if ( a_strName == "class gCRoutine" && uVersion == 1 )
    {
        FormatVariable( "RoutinePoints", "class bTObjArray<class gCRoutinePoint>" );
    }
    else if ( a_strName == "class eCAnimation3Controller_PS" && uVersion == 13 )
    {
        for ( MIUInt u = 0; u != 6; ++u )
        {
            StartBlock( mCString().Format( "BodyPart%u", u ) );
            FormatVariable( "MeshFile1", s_arrTypes[ EString ] );
            FormatVariable( "MeshFile2", s_arrTypes[ EString ] );
            FormatVariable( "Unknown1", "", 2 );
            FormatVariable( "Unknown2", "", 2 );
            EndBlock();
        }
    }
    else if ( a_strName == "class gCInfo" && uVersion == 1 )
    {
        MIUInt const uInfoCommandCount = m_streamIn.ReadU16();
        MIUInt const uInfoConditionCount = m_streamIn.ReadU16();
        StartBlock( "InfoCommands" );
        {
            for ( MIUInt u = 0; u != uInfoCommandCount; ++u, WriteLine() )
                DocumentRisen3Class();
        }
        EndBlock();
        StartBlock( "InfoConditions" );
        {
            for ( MIUInt u = 0; u != uInfoConditionCount; ++u, WriteLine() )
                DocumentRisen3Class();
        }
        EndBlock();
    }
    else
    {
        // Very hacky unfortunately
        EndBlock();
        Write( "" );
        m_streamOut.Seek( uOffsetOut );
        Write( " = " );
        FormatData( "", uSize );
        WriteLine( ";" );
        return;
    }
    MIUInt const uRemainingSize = uSize - ( m_streamIn.Tell() - uOffset );
    if ( uRemainingSize )
        FormatVariable( "Unknown", "", uRemainingSize );
    EndBlock();
}

MIBool mCRisenDoc::DocumentRisen3Sector( MIBool a_bElex )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GAR5" || m_streamIn.ReadU32() != 32 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    for ( MIUInt u = m_streamIn.ReadU16(); u--; )
    {
        StartBlock( "\"" + m_streamIn.ReadString( m_streamIn.ReadU16() ) + "\"" );
        if ( m_streamIn.Skip( 4 ), m_streamIn.ReadU32() != mCRisenName( "class gCEmbeddedLayer" ).GetRisenID() )
            return m_streamIn.Seek( uOffset ), MIFalse;
        m_streamIn.Skip( 92 );
        if ( !DocumentRisen3DynamicEntity( a_bElex ) )
            return m_streamIn.Seek( uOffset ), MIFalse;
        WriteLine();
        EndBlock( MIFalse );
    }
    return MITrue;
}

MIBool mCRisenDoc::DocumentRisen3Template( MIBool a_bElex )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GAR5" || ( m_streamIn.ReadU32(), m_streamIn.ReadString( 4 ) != "GTP0" ) )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.ReadU64();  // Time stamp.
    return DocumentRisen3TemplateClass( MITrue, a_bElex );
}

MIBool mCRisenDoc::DocumentRisen3Hdr( void )
{
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "DT01" || m_streamIn.ReadU32() != 0 || m_streamIn.ReadU32() != 1 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    MIUInt uSectionCount = m_streamIn.ReadU32();
    MIUInt uOffsetSections = m_streamIn.Tell();
    for ( MIUInt s = 0; s < uSectionCount; s++ ) {
        m_streamIn.Seek( uOffsetSections + s * 4 );
        m_streamIn.Seek( m_streamIn.ReadU32() + 8 );  // displacement of 8 cause GAR5 header takes 8 byte
        StartBlock( ReadHash( "class" ) );
        for ( MIUInt u = m_streamIn.ReadU32(); u--; )
        {
            mCString strName = m_streamIn.ReadString( m_streamIn.ReadU32() );
            StartBlock( "\"" + strName + "\"" );
            FormatVariable( "Timestamp", s_arrTypes[ EDateTime ] );
            m_streamIn.ReadU32();  // skip class size
            if ( !DocumentRisen3Class() )
                return m_streamIn.Seek( uOffset ), MIFalse;
            WriteLine();
            EndBlock();
        }
        EndBlock();
    }
    return MITrue;
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

MIBool mCRisenDoc::DocumentRisen3DynamicEntity( MIBool a_bElex )
{
    MIU16 const u16DynEntityVersion = a_bElex ? 9 : 6;
    MIU16 const u16EntityVersion = a_bElex ? 4 : 3;
    MIUInt const uFFSize = a_bElex ? 0 : 1;
    MIUInt const uRangeAndUnknown0Size = a_bElex ? 12 : 0;
    MIUInt const uTimestampSize = a_bElex ? 0 : 8;
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GEC2" || m_streamIn.ReadU32() != mCRisenName( "class eCDynamicEntity" ).GetRisenID() || m_streamIn.ReadU16() != u16DynEntityVersion )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.ReadU32();
    for ( MIUInt u = m_streamIn.ReadU16(); u--; )
    {
        m_streamIn.Skip( 8 );
        m_streamIn.Skip( m_streamIn.ReadU32() );
    }
    m_streamIn.ReadU32();
    if ( m_streamIn.ReadU32() != mCRisenName( "class eCDynamicEntity" ).GetRisenID() || m_streamIn.ReadU16() != u16DynEntityVersion || m_streamIn.ReadU32() != 185 + uFFSize + uRangeAndUnknown0Size )
        return m_streamIn.Seek( uOffset ), MIFalse;
    MIUInt const uClassDataOffset1 = m_streamIn.Tell();
    m_streamIn.Skip( 185 + uFFSize + uRangeAndUnknown0Size );
    if ( m_streamIn.ReadU32() != mCRisenName( "class eCEntity" ).GetRisenID() || m_streamIn.ReadU16() != u16EntityVersion || m_streamIn.ReadU32() == 0 )
        return m_streamIn.Seek( uOffset ), MIFalse;
    MIUInt const uClassDataOffset2 = m_streamIn.Tell();
    m_streamIn.Skip( 16 );
    MIUInt const uNameLength = m_streamIn.ReadU16();
    StartBlock( "\"" + m_streamIn.ReadString( uNameLength ) + "\"" );
    m_streamIn.Seek( uClassDataOffset2 );
    FormatVariable( "GUID", s_arrTypes[ EGuid ] );
    m_streamIn.Seek( uClassDataOffset1 + 168 + uFFSize );
    FormatVariable( "Creator", s_arrTypes[ EGuid ] );
    m_streamIn.Seek( uClassDataOffset1 );
    FormatVariable( "MatrixLocal", s_arrTypes[ EMatrix ] );
    FormatVariable( "MatrixGlobal", s_arrTypes[ EMatrix ] );
    FormatVariable( "Extents", s_arrTypes[ EBox ] );
    FormatVariable( "Center", s_arrTypes[ EVector ] );
    FormatVariable( "Radius", s_arrTypes[ EFloat ] );
    if ( a_bElex )
    {
        m_streamIn.Skip( 16 + 1 );
        FormatVariable( "Range", s_arrTypes[ EFloat ] );
        FormatVariable( "Unknown0", "blob", 8 );
    }
    m_streamIn.Seek( uClassDataOffset2 + 18 + uNameLength );
    if ( !a_bElex )
        FormatVariable( "TimeStamp", "blob", 8 );
    m_streamIn.Seek( uClassDataOffset1 + 184 + uFFSize );
    FormatVariable( "Unknown1", "blob", 1 );
    m_streamIn.Seek( uClassDataOffset2 + 18 + uNameLength + uTimestampSize );
    FormatVariable( "Unknown2", "blob", 6 );
    for ( MIUInt u = m_streamIn.ReadU8(); u--; WriteLine() )
        DocumentRisen3Class();
    for ( MIUInt u = m_streamIn.ReadU32(); u--; WriteLine() )
        if ( !DocumentRisen3DynamicEntity( a_bElex ) )
            return m_streamIn.Seek( uOffset ), MIFalse;
    EndBlock( MIFalse );
    return MITrue;
}

MIBool mCRisenDoc::DocumentRisen3TemplateClass( MIBool a_bMasterEntity, MIBool a_bElex )
{
    MIU16 const u16TemplateEntityVersion = a_bElex ? 6 : 3;
    MIU16 const u16EntityVersion = a_bElex ? 4 : 3;
    MIUInt const uFiletimeSize = a_bElex ? 0 : 8;
    MIUInt const uUnknown1Size = a_bElex ? 4 : 2;
    MIUInt const uOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadString( 4 ) != "GEC2" || m_streamIn.ReadU32() != mCRisenName( "class eCTemplateEntity" ).GetRisenID() || m_streamIn.ReadU16() != u16TemplateEntityVersion )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.ReadU32();
    for ( MIUInt u = m_streamIn.ReadU16(); u--; )
    {
        m_streamIn.Skip( 8 );
        m_streamIn.Skip( m_streamIn.ReadU32() );
    }
    m_streamIn.ReadU32();
    MIUInt const uClassDataOffset = m_streamIn.Tell();
    if ( m_streamIn.ReadU32() != mCRisenName( "class eCTemplateEntity" ).GetRisenID() || m_streamIn.ReadU16() != u16TemplateEntityVersion || m_streamIn.ReadU32() != 50 + uUnknown1Size )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.Skip( 50 + uUnknown1Size );
    if ( m_streamIn.ReadU32() != mCRisenName( "class eCEntity" ).GetRisenID() || m_streamIn.ReadU16() != u16EntityVersion )
        return m_streamIn.Seek( uOffset ), MIFalse;
    m_streamIn.Skip( 20 );
    mCString strName = m_streamIn.ReadString( m_streamIn.ReadU16() );
    MIUInt uOffset2 = m_streamIn.Tell();
    if ( a_bMasterEntity )
        strName = "<master>";
    else
        strName = "\"" + strName + "\"";
    StartBlock( strName );
    m_streamIn.Seek( uClassDataOffset + 70 + uUnknown1Size );
    FormatVariable( "GUID", s_arrTypes[ EGuid ] );
    m_streamIn.Seek( uClassDataOffset + 10 );
    FormatVariable( "RefTemplateGUID", s_arrTypes[ EGuid ] );
    m_streamIn.Skip( 1 );
    FormatVariable( "Position", s_arrTypes[ EVector ] );
    FormatVariable( "Rotation", s_arrTypes[ EQuaternion ] );
    m_streamIn.Skip( 5 );
    if ( a_bElex )
    {
        m_streamIn.Skip( -1 );
        FormatVariable( "Unknown0", "blob", 1 );
        FormatVariable( "Unknown1", s_arrTypes[ EFloat ] );
    }
    else
        FormatVariable( "Unknown1", "blob", 2 );
    m_streamIn.Seek( uOffset2 + uFiletimeSize );
    FormatVariable( "Unknown2", "blob", 6 );
    for ( MIUInt v = m_streamIn.ReadU8(); v--; WriteLine() )
        if ( 0 == DocumentRisen3Class() )
            return m_streamIn.Seek( uOffset ), MIFalse;
    for ( MIUInt u = m_streamIn.ReadU32(); u--; )
        if ( !DocumentRisen3TemplateClass( MIFalse, a_bElex ) )
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