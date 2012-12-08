#ifndef MI_XMACREADER_H_INCLUDED
#define MI_XMACREADER_H_INCLUDED

class mCXmacReader
{
public:
    typedef mCXactReader::SOptions SOptions;
public:
    static mEResult ReadXmacFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options = SOptions() );
};

#endif
