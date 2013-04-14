#ifndef MI_XLMSHWRITER_H_INCLUDED
#define MI_XLMSHWRITER_H_INCLUDED

class mCXlmshWriter
{
public:
    static mEResult WriteXlmshFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest );
};

#endif
