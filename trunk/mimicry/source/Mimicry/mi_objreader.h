#ifndef MI_OBJREADER_H_INCLUDED
#define MI_OBJREADER_H_INCLUDED

class mCObjReader
{
public:
    static mEResult ReadObjFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, MILPCChar a_pcFolderPath = "" );
};

#endif