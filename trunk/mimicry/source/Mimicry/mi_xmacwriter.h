#ifndef MI_XMACWRITER_H_INCLUDED
#define MI_XMACWRITER_H_INCLUDED

class mCXmacWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    public:
        SOptions( void );
    public:
        mTArray< mCString > * m_pReplacedMeshesArray;
        MIBool                m_bReplaceOnlyVertices;
        MIBool                m_bIndirectVertexMatching;
        mCIOStreamBinary *    m_pBaseXmacStream;
    };
public:
    static mEResult WriteXmacFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options );
};

#endif
