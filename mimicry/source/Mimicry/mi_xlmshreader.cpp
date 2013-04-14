#include "mi_include_converters.h"

mEResult mCXlmshReader::ReadXlmshFileData( mTArray< mCString > & a_arrMeshNamesDest, mCIOStreamBinary & a_streamSource )
{
    a_arrMeshNamesDest.Clear();
    mCGenomeFile File( &a_streamSource );
    File.StartRead();
    if ( ( File().GetSize() < 11 ) ||
         ( File().ReadU16() != 1 ) ||
         ( File().ReadU8() != 1 ) ||
         ( File().ReadU16() != 1 ) ||
         ( File().ReadU8() != 1 ) ||
         ( File.ReadString() != "eCResourceMeshLoD_PS" ) ||
         ( File().ReadU16() != 1 ) ||
         ( File().ReadU8() != 0 ) )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid .xlmsh file." ), mEResult_False;
    MIUInt uVersion = File().ReadU16();
    File().Skip( 6 );
    if ( uVersion < 81 )
        File.ReadString();
    if ( uVersion < 82 )
        File().Skip( 20 );
    File().ReadU16();
    for ( MIUInt u = File().ReadU32(); u--; )
        File.ReadString(), File.ReadString(), File().ReadU16(), File().Skip( File().ReadU32() );
    uVersion = File().ReadU16();
    MIUInt const uPSVersion = File().ReadU16();
    if ( uPSVersion > 22 )
        File().ReadU32();
    if ( uPSVersion < 30 )
        if ( File().ReadU16() > 1 )
            File().ReadBool();
    if ( uVersion < 23 )
        File().ReadU32();
    for ( MIUInt u = File().ReadU32(); u--; a_arrMeshNamesDest.Add( File.ReadString() ) );
    return mEResult_Ok;
}
