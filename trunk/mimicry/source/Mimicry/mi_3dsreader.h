#ifndef MI_3DSREADER_H_INCLUDED
#define MI_3DSREADER_H_INCLUDED

class mC3dsReader
{
public:
    static mEResult Read3dsFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource );
};

#endif
