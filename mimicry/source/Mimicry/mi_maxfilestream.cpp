#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_standard.h"
#include "mi_include_variant.h"
#include "mi_include_3d.h"
#include "mi_include_streams.h"

namespace
{
    enum
    {
        mEBlockSize = 512
    };

    MIUInt GetNextJunkBlockPhysical( MIUInt a_uPhysicalOffset, MIUInt a_uVirtualSize, mTArray< MIUInt > & a_arrJunkBlocks )
    {
        MIUInt const uNumJunkBlocks = a_arrJunkBlocks.GetCount();
        MIUInt const uPhysicalSize = a_uVirtualSize + ( a_arrJunkBlocks.GetCount() - 1 ) * mEBlockSize;
        if ( a_uPhysicalOffset >= uPhysicalSize )
            return a_arrJunkBlocks.GetCount();
        MIU64 u64NearJunkBlock = a_uPhysicalOffset;
        u64NearJunkBlock *= uNumJunkBlocks;
        u64NearJunkBlock /= uPhysicalSize;

        MIUInt u = static_cast< MIUInt >( u64NearJunkBlock + 1 );
        while ( a_arrJunkBlocks[ u - 1 ] + ( u - 1 ) * mEBlockSize > a_uPhysicalOffset )
            if ( !--u ) break;
        while ( a_arrJunkBlocks[ u ] + u * mEBlockSize <= a_uPhysicalOffset )
            ++u;
        return u; 
    }

    MIUInt GetNextJunkBlockVirtual( MIUInt a_uVirtualOffset, MIUInt a_uVirtualSize, mTArray< MIUInt > & a_arrJunkBlocks )
    {
        MIUInt const uNumJunkBlocks = a_arrJunkBlocks.GetCount();
        if ( a_uVirtualOffset >= a_uVirtualSize )
            return a_arrJunkBlocks.GetCount();
        MIU64 u64NearJunkBlock = a_uVirtualOffset;
        u64NearJunkBlock *= uNumJunkBlocks;
        u64NearJunkBlock /= a_uVirtualSize;

        MIUInt u = static_cast< MIUInt >( u64NearJunkBlock + 1 );
        while ( a_arrJunkBlocks[ u - 1 ] > a_uVirtualOffset )
            if ( !--u ) break;
        while ( a_arrJunkBlocks[ u ] <= a_uVirtualOffset )
            ++u;
        return u;
    }

    MIUInt GetVirtualOffset( MIUInt a_uPhysicalOffset, MIUInt a_uVirtualSize, mTArray< MIUInt > & a_arrJunkBlocks )
    // a_uPhysicalOffset must not be within a junk block.
    {
        return a_uPhysicalOffset - GetNextJunkBlockPhysical( a_uPhysicalOffset, a_uVirtualSize, a_arrJunkBlocks ) * mEBlockSize;
    }
}

mCMaxFileStream::mCMaxFileStream( void )
{
}

mCMaxFileStream::mCMaxFileStream( mCString a_strFileName ) :
    mCFileStream()
{
    Open( a_strFileName );
}

mEResult mCMaxFileStream::Open( mCString a_strFileName )
{
    return mCFileStream::Open( a_strFileName, mEFileOpenMode_Read );
}

mEResult mCMaxFileStream::ReadPersistentGlobal( mCString const a_strName, mCVariant & a_vDest )
{
    mCError const * const pLastError = mCError::GetLastError< mCError >();
    a_vDest.Clear();
    if ( !IsOpen() )
        return mEResult_False;

    Seek( GetVirtualOffset( ( m_u32ClassDataBlock + 1 ) * 512 + 244, GetSize(), m_arrJunkBlocks ) );
    Seek( GetVirtualOffset( ( ReadU32() + 1 ) * 512, GetSize(), m_arrJunkBlocks ) );
    if ( pLastError != mCError::GetLastError< mCError >() )
        return mEResult_False;

    for ( ; ; )
    {
        MIUInt const uPosition = Tell();
        MIU32 u32Length = ReadU32();
        MIU16 u16Magic = ReadU16();
        MIU8 * pLengthBytes = reinterpret_cast< MIU8 * >( &u32Length );
        if ( u32Length == 0 )
            u32Length = 4;
        else if ( ( pLengthBytes[ 2 ] ^ pLengthBytes[ 3 ] ) == 0x80 )
            u32Length = 6;
        else
            for ( MIUInt u = 0; u != 4; pLengthBytes[ u++ ] ^= pLengthBytes[ 3 ] );
        if ( u32Length == 8 )
            if ( u16Magic == 0x8521 )
                m_u8Seed = ReadU8();
        Seek( uPosition + u32Length );
        if ( pLastError != mCError::GetLastError< mCError >() )
            return mEResult_False;
        if ( u32Length == 6 )
            if ( u16Magic == 0x8551 )
                break;
    }

    ReadU32();  // MIU32 u32PersistentsLength. Xor each byte with last byte and with 0x80.
    MIU16 u16Magic = ReadU16();
    if ( u16Magic != 0x8571 )
        return mEResult_False;

    MIUInt uEndPersistents = GetSize();
    MIUInt uCurrentOffset;
    MIU32 u32CurrentLength;
    while ( Tell() < uEndPersistents )
    {
        uCurrentOffset = Tell();
        u32CurrentLength = ReadU32();
        MIU8 u8Type = ReadU8();
        if ( ReadU8() != 0xA4 )
            return mEResult_False;
        Decrypt( &u32CurrentLength, sizeof( MIU32 ), u8Type );
        u32CurrentLength &= ~( 1 << 31 );
        if ( u8Type == 0xF7 )
        {
            mCString strName;
            Read( strName, ReadU32() );
            if ( strName == a_strName )
            {
                Seek( uCurrentOffset + u32CurrentLength );
                return ReadPersistentGlobalData( a_vDest );
            }
        }
        else if ( u8Type == 0xC1 )
        {
            uEndPersistents = uCurrentOffset + u32CurrentLength;
            continue;
        }
        Seek( uCurrentOffset + u32CurrentLength );
    }
    return mEResult_False;
}

void mCMaxFileStream::DirectRead( MILPVoid a_pDest, MIUInt a_uPosition, MIUInt a_uSize )
{
    MIUInt u = GetNextJunkBlockVirtual( a_uPosition, GetSize(), m_arrJunkBlocks );
    FILE * pFile = static_cast< FILE * >( m_pFile );
    MIByte * pDest = static_cast< MIByte * >( a_pDest );
    while ( a_uSize )
    {
        MIUInt uSize = g_min( a_uSize, ( m_arrJunkBlocks[ u ] - a_uPosition ) );
        fseek( pFile, ( a_uPosition + u * mEBlockSize ), SEEK_SET );
        fread( pDest, sizeof( MIByte ), uSize, pFile );
        a_uSize -= uSize;
        pDest += uSize;
        a_uPosition += uSize;
        ++u;
    }
}

void mCMaxFileStream::Init( MILPVoid a_pFile, mCString a_strFileName, mEFileOpenMode a_enuOpenMode )
{
    if ( a_enuOpenMode != mEFileOpenMode_Read )
    {
        fclose( static_cast< FILE * >( a_pFile ) );
        Open( a_strFileName );
        return;
    }
    MIUInt uFileSize = 0;
    MIUInt uJunkBlockCount = 0;
    mCFileStream PreFileStream( a_strFileName, mEFileOpenMode_Read );

    for ( ; ; )
    {
        if ( PreFileStream.GetSize() < 2000 )
            break;
        PreFileStream.Seek( 0 );
        if ( PreFileStream.ReadU32() != 0xE011CFD0 )
            break;
        PreFileStream.Seek( 44 );
        PreFileStream >> g_32( uJunkBlockCount );
        if ( uJunkBlockCount > ( PreFileStream.GetSize() / ( 512 / 4 ) / 512 + 2 ) )
            break;
        uJunkBlockCount += ( ( uJunkBlockCount + 17 ) / 127 );
        m_arrJunkBlocks.Resize( uJunkBlockCount + 1 );
        uFileSize = PreFileStream.GetSize() - ( uJunkBlockCount * mEBlockSize );

        MIUInt u = 0;
        MIUInt uNumBlocksRead = 0;

        PreFileStream.Seek( 76 );
        for ( uNumBlocksRead = g_min< MIUInt >( uJunkBlockCount, uNumBlocksRead + 109 ); u != uNumBlocksRead; ++u )
            PreFileStream >> g_32( m_arrJunkBlocks[ u ] );
        if ( uNumBlocksRead != uJunkBlockCount )
        {
            PreFileStream.Seek( 68 );
            ++uNumBlocksRead;
            PreFileStream >> g_32( m_arrJunkBlocks[ u++ ] );
        }
        while ( uNumBlocksRead != uJunkBlockCount )
        {
            PreFileStream.Seek( m_arrJunkBlocks[ u - 1 ] * mEBlockSize + mEBlockSize );
            m_arrJunkBlocks[ u ] = m_arrJunkBlocks[ u - 1 ];
            m_arrJunkBlocks[ u - 1 ] = PreFileStream.ReadU32();
            ++u;
            for ( uNumBlocksRead = g_min< MIUInt >( uJunkBlockCount, uNumBlocksRead + 128 ); u != uNumBlocksRead; ++u )
                PreFileStream >> g_32( m_arrJunkBlocks[ u ] );
        }
        for ( u = 0; u != uJunkBlockCount; ++u )
            m_arrJunkBlocks[ u ] = ( m_arrJunkBlocks[ u ] * mEBlockSize + mEBlockSize ) - ( u * mEBlockSize );
        m_arrJunkBlocks[ uJunkBlockCount ] = PreFileStream.GetSize() - ( uJunkBlockCount * mEBlockSize );
        PreFileStream.Seek( 516 );
        m_u32ClassDataBlock = PreFileStream.ReadU32();
        PreFileStream.Close();

        Clear();
        m_pFile = a_pFile;
        m_uRecentFileSize = uFileSize;
        m_enuOpenMode = a_enuOpenMode;
        m_strFileName = a_strFileName;
        Buffer( 0 );
        return;
    }
    fclose( static_cast< FILE * >( a_pFile ) );
    Clear();
    MI_ERROR( mCStreamError, EBadFormat, "File is not a valid .gmax or .max file." );
}

void mCMaxFileStream::Decrypt( MILPVoid a_pData, MIUInt a_uSize, MIU8 a_u8Type )
{
    MIU16 const u16TypeId = a_u8Type ^ 0xF1;
    MIByte * pData = static_cast< MIByte * >( a_pData );
    MIU32 u32Key = ( 7 * u16TypeId + ( m_u8Seed | 0xD3 ) ) | m_u8Seed;
    for ( MIUInt u = 0; u != a_uSize; ++u )
    {
        u32Key = 205333 * u32Key + 2331415;
        pData[ u ] ^= u32Key >> 16;
    }
}

mEResult mCMaxFileStream::ReadPersistentGlobalData( mCVariant & a_vDest )
{
    MI_STATIC_ASSERT( sizeof( MIFloat ) == 4 )
    MIU32 u32Size = ReadU32();
    MIU8 u8Type = ReadU8();
    if ( ReadU8() != 0xA4 )
        return mEResult_False;
    Decrypt( &u32Size, sizeof( MIU32 ), u8Type );
    u32Size &= ~( 1 << 31 );
    switch ( u8Type )
    {
    case 0xF0:
        {
            a_vDest.SetData< MIBool >( ReadU32() != 0 );
            return mEResult_Ok;
        }
    case 0xF2:
        {
            MII32 i32Value = ReadI32();
            Decrypt( &i32Value, sizeof( MII32 ), u8Type );
            a_vDest.SetData< MIInt >( static_cast< MIInt >( i32Value ) );
            return mEResult_Ok;
        }
    case 0xF4:
        {

            MIU32 u32Length = ReadU32();
            Decrypt( &u32Length, sizeof( MIU32 ), u8Type );
            mCCharArray arrTemp( u32Length );
            Read( arrTemp.AccessBuffer(), u32Length );
            Decrypt( arrTemp.AccessBuffer(), u32Length, u8Type );
            a_vDest.SetData< mCString >( arrTemp.GetBuffer() );
            return mEResult_Ok;
        }
    case 0xF5:
        {
            a_vDest.SetData< MIFloat >( ReadFloat() );
            return mEResult_Ok;
        }
    case 0xF6:
        {
            Skip( 6 );
            MIU32 u32NumElements = ReadU32();
            mTArray< mCVariant > arrValues( u32NumElements );
            for ( MIUInt u = 0; u != u32NumElements; ++u )
            {
                ReadPersistentGlobalData( arrValues.AddNew() );
            }
            mTArray< mCVariant > arrTemp;
            a_vDest.SetData( arrTemp );
            a_vDest.SwapData( arrValues );
            return mEResult_Ok;
        }
    case 0xF7:
        {
            mCString strText;
            Read( strText, ReadU32() );
            a_vDest.SetData< mCString >( strText );
            return mEResult_Ok;
        }
    case 0xF9:
        {
            MIFloat arrCoords[ 3 ];
            Read( arrCoords, 3 * sizeof( MIFloat ) );
            Decrypt( arrCoords, 3 * sizeof( MIFloat ), u8Type );
            a_vDest.SetData< mCVec3 >( mCVec3( arrCoords[ 0 ], arrCoords[ 1 ], arrCoords[ 2 ] ) );
            return mEResult_Ok;
        }
    case 0xFC:
        {
            MIFloat arrComponents[ 4 ][ 3 ];
            Read( *arrComponents, 12 * sizeof( MIFloat ) );
            Skip( 4 );
            Decrypt( *arrComponents, 12 * sizeof( MIFloat ), u8Type );
            mCMatrix4 matMatrix;
            matMatrix.SetToIdentity();
            for ( MIUInt u = 4; u--; )
                g_memcpy( matMatrix.AccessRow( u ), arrComponents[ u ], 3 * sizeof( MIFloat ) );
            a_vDest.SetData< mCMatrix4 >( matMatrix );
            return mEResult_Ok;
        }
    case 0xFE:
        {
            MIFloat arrComponents[ 4 ];
            Read( arrComponents, 4 * sizeof( MIFloat ) );
            a_vDest.SetData< mCColor >( mCColor( static_cast< MIU8 >( arrComponents[ 0 ] * 255 ), 
                                                 static_cast< MIU8 >( arrComponents[ 1 ] * 255 ), 
                                                 static_cast< MIU8 >( arrComponents[ 2 ] * 255 ), 
                                                 static_cast< MIU8 >( arrComponents[ 3 ] * 255 ) ) );
            return mEResult_Ok;
        }
    default:
        {
            a_vDest.Clear();
            Skip( u32Size - 6 );
            return mEResult_Ok;
        }
    }
}

#endif