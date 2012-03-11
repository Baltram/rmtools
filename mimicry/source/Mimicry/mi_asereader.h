#ifndef MI_ASEREADER_H_INCLUDED
#define MI_ASEREADER_H_INCLUDED

class mCAseReader
{
public:
    static mEResult ReadAseFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource );
};

#endif