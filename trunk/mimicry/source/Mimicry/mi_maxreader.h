#ifndef MI_MAXREADER_H_INCLUDED
#define MI_MAXREADER_H_INCLUDED

class mCMaxReader
{
public:
    static mEResult ReadInMaxFileData( mCScene & a_sceneDest, mCMaxFileStream & a_streamSource );
};

#endif