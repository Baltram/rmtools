#ifndef MI_XLMSHREADER_H_INCLUDED
#define MI_XLMSHREADER_H_INCLUDED

class mCXlmshReader
{
public:
    static mEResult ReadXlmshFileData( mTArray< mCString > & a_arrMeshNamesDest, mCIOStreamBinary & a_streamSource );
};

#endif