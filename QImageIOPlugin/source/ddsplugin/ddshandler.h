#ifndef DDSHANDLER_H_INCLUDED
#define DDSHANDLER_H_INCLUDED

#include <QImageIOHandler>
#include <QImage>

class DdsHandler :
    public QImageIOHandler
{
public:
    DdsHandler( void );
   ~DdsHandler( void );
public:
    static bool canRead( QIODevice * a_pDevice );
public:
    bool       canRead( void ) const;
    QByteArray name( void ) const;
    QVariant   option( ImageOption a_Option ) const;
    bool       read( QImage * a_pDestImage );
    void       setOption( ImageOption, QVariant const & );
    bool       supportsOption( ImageOption a_Option ) const;
protected:
    enum ES3TCCompression
    {
        ES3TCCompression_DXT1,
        ES3TCCompression_DXT3,
        ES3TCCompression_DXT5,
        ES3TCCompression_Count,
        ES3TCCompression_Invalid
    };
    struct SImageInfo
    {
        QSize            m_sizeSize;
        uint             m_uOffsetInFile;
        bool             m_bIsCompressed;
        ES3TCCompression m_enuCompression;
        uint             m_uPixelSize;
        quint32          m_u32RMask;
        quint32          m_u32GMask;
        quint32          m_u32BMask;
        quint32          m_u32AMask;
    };
protected:
    static ES3TCCompression getCompression( QString const & a_strFourCC );
    static int              getCompressedSize( QSize const & a_sizeSize, ES3TCCompression a_enuCompression );
    static bool             readCompressedImage( SImageInfo const & a_ImageInfo, QIODevice * a_pDevice, QImage * a_pDestImage );
    static bool             readUncompressedImage( SImageInfo const & a_ImageInfo, QIODevice * a_pDevice, QImage * a_pDestImage );
    static bool             preProcess( QIODevice * a_pDevice, SImageInfo * a_pDest = 0 );
};

#endif // DDSHANDLER_H_INCLUDED
