#include "mi_include_converters.h"

namespace
{
    enum EChunkID
    {
        MAIN3DS = 0x4D4D
    };
}

mEResult mC3dsReader::Read3dsFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
{
    a_sceneDest.Clear();
    mCString strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileNameNoExt( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) : "";
    if ( a_streamSource.ReadString() != "3db" )
    {
        MI_ERROR( mCConverterError, EBadFormat, "Invalid .3db file." );
        return mEResult_False;
    }
    a_sceneDest.SetName( strSceneName );
    return mEResult_Ok;
}
