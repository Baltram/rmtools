#include "mi_include_converters.h"

MIInt ( * mCGenomeVolume::s_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );

mCGenomeVolume::mCGenomeVolume( mCString const & a_strFilePath )
{
    Open( a_strFilePath );
}

mCGenomeVolume::mCGenomeVolume( void )
{
}

void mCGenomeVolume::RegisterZlibUncompressFunction( MIInt ( * a_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 ) )
{
    s_pfuncUncompress = a_pfuncUncompress;
}

void mCGenomeVolume::Close( void )
{
    m_mapFiles.Clear();
    m_streamArchive.Close();
}

MIBool mCGenomeVolume::FindFile( mCString a_strFileBaseName, mCString & a_strFilePathDest, mCMemoryStream & a_streamFileDataDest )
{
    SFile File;
    mCByteArray arrFileData;
    if ( !m_mapFiles.GetAt( a_strFileBaseName.ToLower(), File ) || ( File.m_bCompressed && !s_pfuncUncompress ) )
        return MIFalse;
    a_strFilePathDest = File.m_strFilePath;
    a_streamFileDataDest.Clear();
    arrFileData.Resize( File.m_uDataSize );
    m_streamArchive.Seek( File.m_uDataOffset );
    m_streamArchive.Read( arrFileData.AccessBuffer(), File.m_uDataSize );
    if ( File.m_bCompressed )
    {
        MIU32 u32FileSize = File.m_uFileSize;
        mCByteArray arrUncompressedFileData;
        arrUncompressedFileData.Resize( u32FileSize );
        if ( ( *s_pfuncUncompress )( arrUncompressedFileData.AccessBuffer(), &u32FileSize, arrFileData.GetBuffer(), arrFileData.GetCount() ) != 0 )
            return MIFalse;
        arrUncompressedFileData.Swap( arrFileData );
    }
    a_streamFileDataDest.Write( arrFileData.GetBuffer(), arrFileData.GetCount() );
    return MITrue;
}

MIBool mCGenomeVolume::IsOpen( void )
{
    return m_streamArchive.IsOpen();
}

mEResult mCGenomeVolume::Open( mCString const & a_strFilePath )
{
    m_streamArchive.Open( a_strFilePath, mEFileOpenMode_Read );
    if ( !m_streamArchive.IsOpen() || m_streamArchive.GetSize() < 48 )
        return mEResult_False;
    MIUInt const uVersion = m_streamArchive.ReadU32();
    mCString const strId = m_streamArchive.ReadString( 4 );
    m_streamArchive.Skip( 4 );
    MIUInt const uEncryption = m_streamArchive.ReadU32();
    MIUInt const uCompression = m_streamArchive.ReadU32();
    if ( uVersion > 1 || strId != "G3V0" || uEncryption || uCompression > 2 )
        return mEResult_False;
    m_streamArchive.Skip( 12 );
    MIUInt const uRootOffset = static_cast< MIUInt >( m_streamArchive.ReadU64() );
    m_streamArchive.Seek( uRootOffset );
    mTArray< SFile > arrFiles;
    if ( uVersion == 0 )
        ReadOldVersionedEntry( m_streamArchive, arrFiles, "" );
    else
        ReadNewVersionedDirectory( m_streamArchive, arrFiles, "" );
    m_mapFiles.Clear();
    m_mapFiles.Reserve( arrFiles.GetCount() );
    for ( MIUInt u = 0, ue = arrFiles.GetCount(); u != ue; ++u )
        m_mapFiles[ g_GetFileNameNoExt( arrFiles[ u ].m_strFilePath ).ToLower() ] = arrFiles[ u ];
    return mEResult_Ok;
}

namespace
{
    mCString ReadString( mCIOStreamBinary & a_streamSource )
    {
        MIUInt uLength = a_streamSource.ReadU32();
        if ( !uLength )
            return "";
        mCString strResult = a_streamSource.ReadString( uLength );
        a_streamSource.Skip( 1 );
        return strResult;
    }
}

void mCGenomeVolume::ReadNewVersionedDirectory( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath )
{
    a_strPath += ReadString( a_streamSource ) + "\\";
    a_streamSource.Skip( 28 );
    for ( MIUInt u = a_streamSource.ReadU32(); u--; )
    {
        if ( a_streamSource.ReadU32() & 0x10 )
            ReadNewVersionedDirectory( a_streamSource, a_arrFilesDest, a_strPath );
        else
            ReadNewVersionedFile( a_streamSource, a_arrFilesDest, a_strPath );
    }
}

void mCGenomeVolume::ReadNewVersionedFile( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath )
{
    SFile & File = a_arrFilesDest.AddNew();
    File.m_strFilePath = a_strPath + ReadString( a_streamSource );
    File.m_uDataOffset = static_cast< MIUInt >( a_streamSource.ReadU64() );
    a_streamSource.Skip( 24 );
    MIU32 u32Attributes = a_streamSource.ReadU32();
    a_streamSource.Skip( 4 );
    File.m_bCompressed = ( ( a_streamSource.ReadU32() & 2 ) != 0 );
    File.m_uDataSize = a_streamSource.ReadU32();
    File.m_uFileSize = a_streamSource.ReadU32();
    if ( u32Attributes & 0xC000 )
        a_arrFilesDest.RemoveAt( a_arrFilesDest.GetCount() - 1 );
}

void mCGenomeVolume::ReadOldVersionedEntry( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath )
{
    a_streamSource.Skip( 32 );
    MIU32 u32Attributes = a_streamSource.ReadU32();
    if ( u32Attributes & 0x10 )
    {
        a_strPath = ReadString( a_streamSource );
        for ( MIUInt u = a_streamSource.ReadU32(); u--; ReadOldVersionedEntry( a_streamSource, a_arrFilesDest, a_strPath ) );
        for ( MIUInt u = a_streamSource.ReadU32(); u--; ReadOldVersionedEntry( a_streamSource, a_arrFilesDest, a_strPath ) );
        return;
    }
    SFile & File = a_arrFilesDest.AddNew();
    File.m_uDataOffset = static_cast< MIUInt >( a_streamSource.ReadU64() );
    File.m_uDataSize = static_cast< MIUInt >( a_streamSource.ReadU64() );
    File.m_uFileSize = static_cast< MIUInt >( a_streamSource.ReadU64() );
    a_streamSource.Skip( 4 );
    File.m_bCompressed = ( ( a_streamSource.ReadU32() & 2 ) != 0 );
    File.m_strFilePath = ReadString( a_streamSource );
    ReadString( a_streamSource );
    if ( u32Attributes & 0xC000 )
        a_arrFilesDest.RemoveAt( a_arrFilesDest.GetCount() - 1 );
}
