#include "ddshandler.h"
#include <QVariant>
#include <QDataStream>
#include <QGLContext>

DdsHandler::DdsHandler( void )
{
}

DdsHandler::~DdsHandler( void )
{
}

bool DdsHandler::canRead( QIODevice * a_pDevice )
{
    return preProcess( a_pDevice );
}

bool DdsHandler::canRead( void ) const
{
    return canRead( device() );
}

QByteArray DdsHandler::name( void ) const
{
    return "dds";
}

QVariant DdsHandler::option( ImageOption a_Option ) const
{
    SImageInfo Info;
    if ( a_Option == Size && preProcess( device(), &Info ) )
        return Info.m_sizeSize;
    return QVariant();
}

bool DdsHandler::read( QImage * a_pDestImage )
{
    SImageInfo Info;
    if ( !preProcess( device(), &Info ) )
        return false;
    QDataStream steamIn( device() );
    steamIn.setByteOrder( QDataStream::LittleEndian );
    steamIn.skipRawData( Info.m_uOffsetInFile );
    if ( Info.m_bIsCompressed && !readCompressedImage( Info, steamIn.device(), a_pDestImage ) )
        return false;
    if ( !Info.m_bIsCompressed && !readUncompressedImage( Info, steamIn.device(), a_pDestImage ) )
        return false;
    return true;
}

void DdsHandler::setOption( ImageOption, const QVariant & )
{
}

bool DdsHandler::supportsOption( ImageOption a_Option ) const
{
    return a_Option == Size;
}

DdsHandler::ES3TCCompression DdsHandler::getCompression( QString const & a_strFourCC )
{
    if ( a_strFourCC == "DXT1" )
        return ES3TCCompression_DXT1;
    if ( a_strFourCC == "DXT3" )
        return ES3TCCompression_DXT3;
    if ( a_strFourCC == "DXT5" )
        return ES3TCCompression_DXT5;
    return ES3TCCompression_Invalid;
}

int DdsHandler::getCompressedSize( QSize const & a_sizeSize, ES3TCCompression a_enuCompression )
{
    return ( ( a_sizeSize.width() + 3 ) / 4 ) * ( ( a_sizeSize.height() + 3 ) / 4 ) * ( a_enuCompression == ES3TCCompression_DXT1 ? 8 : 16 );
}

class ContextHelper
{
public:
    ContextHelper()
    {
        m_bActive = QGLContext::currentContext() ? false : true;
        if ( !m_bActive )
            return;
        if ( !s_pContext )
        {
            s_pContext = new QGLContext( QGLFormat(), new QGLWidget );
            s_pContext->create();
        }
        s_pContext->makeCurrent();
    }
   ~ContextHelper()
    {
        if ( m_bActive )
            s_pContext->doneCurrent();
    }
private:
    bool m_bActive;
    static QGLContext * s_pContext;
};

QGLContext * ContextHelper::s_pContext = 0;

bool DdsHandler::readCompressedImage( SImageInfo const & a_ImageInfo, QIODevice * a_pDevice, QImage * a_pDestImage )
{
    typedef void ( APIENTRY * FP_glCompressedTexImage2DARB )( GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, GLvoid const * );
    static FP_glCompressedTexImage2DARB s_fpCompressedTexImage2DARB = static_cast< FP_glCompressedTexImage2DARB >( 0 );
    ContextHelper contextHelper;  // this ensures that there is a current QGLContext
    if ( !s_fpCompressedTexImage2DARB )
        if ( QGLContext::currentContext() )
            s_fpCompressedTexImage2DARB = static_cast< FP_glCompressedTexImage2DARB >( QGLContext::currentContext()->getProcAddress( "glCompressedTexImage2DARB" ) );
    static GLenum s_arrCompressionConstants[ ES3TCCompression_Count ] = { 0x83F1, 0x83F2, 0x83F3 };
    int const iPixelCount = a_ImageInfo.m_sizeSize.width() * a_ImageInfo.m_sizeSize.height(), iSize = getCompressedSize( a_ImageInfo.m_sizeSize, a_ImageInfo.m_enuCompression );
    if ( !s_fpCompressedTexImage2DARB || a_ImageInfo.m_sizeSize.width() % 4 || a_ImageInfo.m_sizeSize.height() % 4 || !a_pDevice || a_pDevice->bytesAvailable() < iSize || a_ImageInfo.m_enuCompression >= ES3TCCompression_Invalid )
        return false;
    QByteArray arrCompressed = a_pDevice->peek( iSize );
    QImage imgResult( a_ImageInfo.m_sizeSize, QImage::Format_ARGB32 );
    ( *s_fpCompressedTexImage2DARB )( GL_TEXTURE_2D, 0, s_arrCompressionConstants[ a_ImageInfo.m_enuCompression ], a_ImageInfo.m_sizeSize.width(), a_ImageInfo.m_sizeSize.height(), 0, iSize, arrCompressed.constData() );
    glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgResult.bits() );
    if ( QSysInfo::ByteOrder == QSysInfo::BigEndian )
        for ( quint32 * pPixel = reinterpret_cast< quint32 * >( imgResult.bits() ), * pEnd = pPixel + iPixelCount; pPixel != pEnd ; ++pPixel )
            *pPixel = ( *pPixel << 24 ) | ( *pPixel >> 8 );
    else
        for ( quint32 * pPixel = reinterpret_cast< quint32 * >( imgResult.bits() ), * pEnd = pPixel + iPixelCount; pPixel != pEnd ; ++pPixel )
            *pPixel = ( ( *pPixel << 16 ) & 0xff0000 ) | ( ( *pPixel >> 16 ) & 0xff ) | ( *pPixel & 0xff00ff00 );
    *a_pDestImage = imgResult;
    return true;
}

namespace
{
    void processMask( quint32 & a_u32Mask, int & a_iBitOffset, uint & a_uBitCount )
    {
        if ( !a_u32Mask )
        {
            a_iBitOffset = 0, a_uBitCount = 8;
            return;
        }
        uint u, v = 0, w = 0;
        for ( u = 1; ( u & a_u32Mask ) == 0; u <<= 1, ++v );
        for ( w = v; ( u & a_u32Mask ) != 0; u <<= 1, ++w );
        a_uBitCount = w - v;
        a_iBitOffset = w - 8;
        if ( a_uBitCount > 8 )
            a_uBitCount = 0, a_u32Mask &= 0xFF << a_iBitOffset;
    }

    inline
    quint8 getColorComponent( quint32 a_u32Source, quint32 a_u32Mask, int a_iBitOffset, uint a_uBitCount )
    {
        quint32 u32Result = a_iBitOffset < 0 ? ( a_u32Source & a_u32Mask ) << 0 - a_iBitOffset : ( a_u32Source & a_u32Mask ) >> a_iBitOffset;
        for ( int i = 8 - a_uBitCount; i > 0; i -= a_uBitCount, a_uBitCount *= 2 )
            u32Result |= u32Result >> a_uBitCount;
        return static_cast< quint8 >( u32Result );
    }
}

bool DdsHandler::readUncompressedImage( SImageInfo const & a_ImageInfo, QIODevice * a_pDevice, QImage * a_pDestImage )
{
    uint uPixelSize = a_ImageInfo.m_uPixelSize, uSize = uPixelSize * a_ImageInfo.m_sizeSize.height() * a_ImageInfo.m_sizeSize.width();
    if ( !a_pDevice || a_pDevice->bytesAvailable() < uSize )
        return false;
    quint32 u32RMask = a_ImageInfo.m_u32RMask, u32GMask = a_ImageInfo.m_u32GMask, u32BMask = a_ImageInfo.m_u32BMask, u32AMask = a_ImageInfo.m_u32AMask;
    int iRBitOffset, iGBitOffset, iBBitOffset, iABitOffset;
    uint uRBitCount, uGBitCount, uBBitCount, uABitCount;
    processMask( u32RMask, iRBitOffset, uRBitCount );
    processMask( u32GMask, iGBitOffset, uGBitCount );
    processMask( u32BMask, iBBitOffset, uBBitCount );
    processMask( u32AMask, iABitOffset, uABitCount );
    QByteArray arrPixels = a_pDevice->peek( uSize );
    QImage imgResult( a_ImageInfo.m_sizeSize, QImage::Format_ARGB32 );
    arrPixels.append( "000" );
    char * pPixel = arrPixels.data();
    for ( uint u = 0, ue = a_ImageInfo.m_sizeSize.height(); u != ue; ++u )
    {
        QRgb * pLine = reinterpret_cast< QRgb * >( imgResult.scanLine( u ) );
        for ( uint v = 0, ve = a_ImageInfo.m_sizeSize.width(); v != ve; ++v, pPixel += uPixelSize )
        {
            quint32 u32Pixel = *reinterpret_cast< quint32 * >( pPixel );
            pLine[ v ] = qRgba( getColorComponent( u32Pixel, u32RMask, iRBitOffset, uRBitCount ),
                                getColorComponent( u32Pixel, u32GMask, iGBitOffset, uGBitCount ),
                                getColorComponent( u32Pixel, u32BMask, iBBitOffset, uBBitCount ),
                                u32AMask ? getColorComponent( u32Pixel, u32AMask, iABitOffset, uABitCount ) : 255 );
        }
    }
    *a_pDestImage = imgResult;
    return true;
}

bool DdsHandler::preProcess( QIODevice * a_pDevice, SImageInfo * a_pDest )
{
    SImageInfo infoResult;
    QByteArray arrFirstBytes = a_pDevice->peek( 128 );
    QDataStream steamIn( arrFirstBytes );
    steamIn.setByteOrder( QDataStream::LittleEndian );
    quint32 u32Width, u32Height, u32PFlags, u32PixelBitCount;
    if ( steamIn.device()->read( 4 ) != "DDS " )
        return false;
    steamIn.skipRawData( 8 );
    steamIn >> u32Height >> u32Width;
    steamIn.skipRawData( 60 );
    steamIn >> u32PFlags;
    infoResult.m_enuCompression = getCompression( steamIn.device()->read( 4 ) );
    steamIn >> u32PixelBitCount >> infoResult.m_u32RMask >> infoResult.m_u32GMask >> infoResult.m_u32BMask >> infoResult.m_u32AMask;
    infoResult.m_bIsCompressed = ( u32PFlags & 0x4 ) != 0;
    if ( ( infoResult.m_bIsCompressed && ( u32Width % 4 || u32Height % 4 ) ) ||
         ( infoResult.m_bIsCompressed && infoResult.m_enuCompression == ES3TCCompression_Invalid ) ||
         ( !infoResult.m_bIsCompressed && ( u32PFlags & 0x40 ) == 0 ) ||
         ( !infoResult.m_bIsCompressed && ( u32PixelBitCount % 8 ) != 0 ) )
        return false;
    infoResult.m_sizeSize = QSize( u32Width, u32Height );
    infoResult.m_u32AMask = ( u32PFlags & 1 ) ? infoResult.m_u32AMask : 0;
    infoResult.m_uOffsetInFile = 128;
    infoResult.m_uPixelSize = u32PixelBitCount / 8;
    if ( a_pDest )
        *a_pDest = infoResult;
    return true;
}
