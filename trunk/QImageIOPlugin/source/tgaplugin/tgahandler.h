#ifndef TGAHANDLER_H_INCLUDED
#define TGAHANDLER_H_INCLUDED

#include <QImageIOHandler>
#include <QImage>

class TgaHandler :
    public QImageIOHandler
{
public:
    TgaHandler( void );
   ~TgaHandler( void );
public:
    static bool canRead( QIODevice * a_pDevice );
public:
    bool       canRead( void ) const;
    QByteArray name( void ) const;
    QVariant   option( ImageOption a_Option ) const;
    bool       read( QImage * a_pDestImage );
    void       setOption( ImageOption, QVariant const & );
    bool       supportsOption( ImageOption a_Option ) const;
    bool       write( QImage const & a_Image );
protected:
    struct SHeader
    {
        char m_IdLength;
        char m_ColorMapType;
        char m_DataTypeCode;
        char m_ColorMapOrigin1;
        char m_ColorMapOrigin2;
        char m_ColorMapLength1;
        char m_ColorMapLength2;
        char m_ColorMapDepth;
        char m_OriginX1;
        char m_OriginX2;
        char m_OriginY1;
        char m_OriginY2;
        char m_Width1;
        char m_Width2;
        char m_Height1;
        char m_Height2;
        char m_BitCountPerPixel;
        char m_ImageDescriptorBit;
    };
private:
    static bool preProcess( QIODevice * a_pDevice, SHeader * a_pDest = 0 );
};

#endif // TGAHANDLER_H_INCLUDED
