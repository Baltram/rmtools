#ifndef MI_3DBREADER_H_INCLUDED
#define MI_3DBREADER_H_INCLUDED

class mC3dbReader
{
public:
    static mEResult Read3dbFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource );
};

#endif