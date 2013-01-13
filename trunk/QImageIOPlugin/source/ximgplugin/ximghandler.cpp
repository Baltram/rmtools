#include "ximghandler.h"
#include <QVariant>
#include <QDataStream>

XimgHandler::XimgHandler( void )
{
}

XimgHandler::~XimgHandler( void )
{
}

bool XimgHandler::canRead( QIODevice * a_pDevice )
{
    return preProcess( a_pDevice );
}

bool XimgHandler::canRead( void ) const
{
    return canRead( device() );
}

QByteArray XimgHandler::name( void ) const
{
    return "ximg";
}

QVariant XimgHandler::option( ImageOption a_Option ) const
{
    SImageInfo Info;
    if ( a_Option == Size && preProcess( device(), &Info ) )
        return Info.m_sizeSize;
    return QVariant();
}

bool XimgHandler::read( QImage * a_pDestImage )
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

void XimgHandler::setOption( ImageOption, const QVariant & )
{
}

bool XimgHandler::supportsOption( ImageOption a_Option ) const
{
    return a_Option == Size;
}

bool XimgHandler::preProcess( QIODevice * a_pDevice, SImageInfo * a_pDest )
{
    SImageInfo infoResult;
    QByteArray arrFirstBytes = a_pDevice->peek( 265 + 128 );
    QDataStream steamIn( arrFirstBytes );
    steamIn.setByteOrder( QDataStream::LittleEndian );
    qint32 iOffset1, iOffset2 = 0;
    if ( ( steamIn.device()->read( 8 ) == "GR01IM04" ) || ( iOffset2 = 8, steamIn.device()->read( 8 ) == "GR01IM04" ) )  // Risen 1/2 ._ximg
    {
        steamIn.skipRawData( 8 );
        steamIn >> iOffset1;
        steamIn.skipRawData( iOffset1 + iOffset2 - 20 );
        bool bResult = DdsHandler::preProcess( steamIn.device(), a_pDest );
        if ( a_pDest )
            a_pDest->m_uOffsetInFile += iOffset1 + 2 * iOffset2;
        return bResult;

    }
    // Gothic 3 .ximg
    quint32 u32Width, u32Height, u32MipMapCount;
    steamIn.skipRawData( 22 );
    if ( steamIn.device()->read( 5 ) != "G3IMG" )
        return false;
    steamIn.skipRawData( 4 );
    steamIn >> u32Width >> u32Height;
    steamIn.skipRawData( 8 );
    steamIn >> u32MipMapCount;
    steamIn.skipRawData( 4 );
    infoResult.m_enuCompression = getCompression( steamIn.device()->read( 4 ) );
    if ( ( infoResult.m_enuCompression == ES3TCCompression_Invalid ) ||
         ( ( u32Width % 4 ) || ( u32Height % 4 ) ) )
        return false;
    infoResult.m_bIsCompressed = true;
    infoResult.m_sizeSize = QSize( u32Width, u32Height );
    infoResult.m_uOffsetInFile = 87;
    QSize sizeMipMap = infoResult.m_sizeSize;
    for ( int i = u32MipMapCount - 1; i--; infoResult.m_uOffsetInFile += getCompressedSize( sizeMipMap, infoResult.m_enuCompression ) )
        sizeMipMap.rwidth() /= 2, sizeMipMap.rheight() /= 2;
    if ( a_pDest )
        *a_pDest = infoResult;
    return true;
}
