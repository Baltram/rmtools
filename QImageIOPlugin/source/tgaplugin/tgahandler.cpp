#include "tgahandler.h"
#include <QVariant>
#include <QDataStream>

#ifdef _MSC_VER
#define TGAPLUGININLINE __forceinline
#else
#define TGAPLUGININLINE inline
#endif

namespace
{
    TGAPLUGININLINE
    quint16 AsShort( char const & a_Source )
    {
        quint16 u16Result = *reinterpret_cast< quint16 const * >( &a_Source );
        if ( QSysInfo::ByteOrder == QSysInfo::BigEndian )
            qSwap( reinterpret_cast< char * >( &u16Result )[ 0 ], reinterpret_cast< char * >( &u16Result )[ 1 ] );
        return u16Result;
    }

    TGAPLUGININLINE
    void ReadGreyScale( char const *& a_pSource, QRgb & a_Dest )
    {
        a_Dest = qRgb( *a_pSource, *a_pSource, *a_pSource );
        ++a_pSource;
    }

    TGAPLUGININLINE
    void ReadTarga16( char const *& a_pSource, QRgb & a_Dest )
    {
        int i8A = ( a_pSource[ 1 ] & 0x80 ) ? 0xFF : 0x00;
        int i8R = ( a_pSource[ 1 ] & 0x7C ) << 1;
        int i8G = ( a_pSource[ 1 ] & 0x03 ) << 6 | ( a_pSource[ 0 ] & 0xE0 ) >> 2;
        int i8B = ( a_pSource[ 0 ] & 0x1F ) << 3;
        i8R |= i8R >> 5;
        i8G |= i8G >> 5;
        i8B |= i8B >> 5;
        a_Dest = qRgba( i8R, i8G, i8B, i8A );
        a_pSource += 2;
    }

    TGAPLUGININLINE
    void ReadTarga24( char const *& a_pSource, QRgb & a_Dest )
    {
        a_Dest = qRgb( a_pSource[ 2 ], a_pSource[ 1 ], a_pSource[ 0 ] );
        a_pSource += 3;
    }

    TGAPLUGININLINE
    void ReadTarga32( char const *& a_pSource, QRgb & a_Dest )
    {
        a_Dest = qRgba( a_pSource[ 2 ], a_pSource[ 1 ], a_pSource[ 0 ], a_pSource[ 3 ] );
        a_pSource += 4;
    }

    TGAPLUGININLINE
    void ReadColor( char const *& a_pSource, QRgb & a_Dest, uint const a_uByteCount )
    {
        if ( a_uByteCount == 4 )
            ReadTarga32( a_pSource, a_Dest );
        else if ( a_uByteCount == 3 )
            ReadTarga24( a_pSource, a_Dest );
        else if ( a_uByteCount == 2 )
            ReadTarga16( a_pSource, a_Dest );
        else if ( a_uByteCount == 1 )
            ReadGreyScale( a_pSource, a_Dest );
    }

    TGAPLUGININLINE
    void ReadIndexedColor8( char const *& a_pSource, QRgb const * a_pPalette, QRgb & a_Dest )
    {
        a_Dest = a_pPalette[ *reinterpret_cast< quint8 const * >( a_pSource ) ];
        ++a_pSource;
    }

    TGAPLUGININLINE
    void ReadIndexedColor16( char const *& a_pSource, QRgb const * a_pPalette, QRgb & a_Dest )
    {
        a_Dest = a_pPalette[ *reinterpret_cast< quint16 const * >( a_pSource ) ];
        a_pSource += 2;
    }

    TGAPLUGININLINE
    void ReadIndexedColor( char const *& a_pSource, QRgb const * a_pPalette, QRgb & a_Dest, uint const a_uByteCount )
    {
        if ( a_uByteCount == 2 )
            ReadIndexedColor16( a_pSource, a_pPalette, a_Dest );
        else if ( a_uByteCount == 1 )
            ReadIndexedColor8( a_pSource, a_pPalette, a_Dest );
    }

    TGAPLUGININLINE
    void ReadRLEEncoded( char const *& a_pSource, QRgb const * a_pPalette, QRgb *& pDest, uint const a_uByteCount )
    {
        quint8 const u8Header = *a_pSource++;
        uint const uCount = ( u8Header & 0x7F ) + 1;
        if ( u8Header & 0x80 )
        {
            QRgb Color;
            if ( a_pPalette )
                ReadIndexedColor( a_pSource, a_pPalette, Color, a_uByteCount );
            else
                ReadColor( a_pSource, Color, a_uByteCount );
            for ( uint u = 0; u != uCount; ++u )
                *pDest-- = Color;
        }
        else
        {
            if ( a_pPalette )
                for ( uint u = 0; u != uCount; ++u )
                    ReadIndexedColor( a_pSource, a_pPalette, *pDest--, a_uByteCount );
            else
                for ( uint u = 0; u != uCount; ++u )
                    ReadColor( a_pSource, *pDest--, a_uByteCount );
        }
    }

    TGAPLUGININLINE
    void ReadUnEncoded( char const *& a_pSource, QRgb const * a_pPalette, QRgb *& pDest, uint const a_uByteCount )
    {
        if ( a_pPalette )
            ReadIndexedColor( a_pSource, a_pPalette, *pDest--, a_uByteCount );
        else
            ReadColor( a_pSource, *pDest--, a_uByteCount );
    }
}

TgaHandler::TgaHandler( void )
{
}

TgaHandler::~TgaHandler( void )
{
}

bool TgaHandler::canRead( QIODevice * a_pDevice )
{
    return preProcess( a_pDevice );
}

bool TgaHandler::canRead( void ) const
{
    return canRead( device() );
}

QByteArray TgaHandler::name( void ) const
{
    return "tga";
}

QVariant TgaHandler::option( ImageOption a_Option ) const
{
    SHeader Header;
    if ( a_Option == Size && preProcess( device(), &Header ) )
        return QSize( AsShort( Header.m_Width1 ), AsShort( Header.m_Height1 ) );
    return QVariant();
}

bool TgaHandler::read( QImage * a_pDestImage )
{
    SHeader Header;
    if ( !preProcess( device(), &Header ) )
        return false;
    QDataStream steamIn( device() );
    steamIn.setByteOrder( QDataStream::LittleEndian );
    steamIn.skipRawData( sizeof( SHeader ) + Header.m_IdLength );
    bool const bRLE = ( Header.m_DataTypeCode == 9 ) || ( Header.m_DataTypeCode == 10 ) || ( Header.m_DataTypeCode == 11 );
    bool const bIndexed = ( Header.m_DataTypeCode == 1 ) || ( Header.m_DataTypeCode == 9 );
    uint const uByteCountPerPixel = ( Header.m_BitCountPerPixel + 1 ) / 8;  // 15 bit -> 2 bytes.
    uint uMaxLength = ( Header.m_ColorMapType ? AsShort( Header.m_ColorMapLength1 ) * Header.m_ColorMapDepth / 8 : 0 ) +
                      ( ( bRLE ? 1 : 0 ) + Header.m_BitCountPerPixel / 8 ) * AsShort( Header.m_Height1 ) * AsShort( Header.m_Width1 );
    QByteArray arrData = steamIn.device()->peek( uMaxLength );
    arrData.resize( uMaxLength );
    char const * pData = arrData.data();
    QRgb * pColorMap = 0;
    if ( Header.m_ColorMapType )
    {
        pColorMap = new QRgb [ AsShort( Header.m_ColorMapLength1 ) * 2 ];
        for ( int i = 0, ie = AsShort( Header.m_ColorMapLength1 ); i != ie; ++i )
            ReadColor( pData, pColorMap[ i ], ( Header.m_ColorMapDepth + 1 ) / 8 );
        qMemCopy( pColorMap + AsShort( Header.m_ColorMapLength1 ), pColorMap, AsShort( Header.m_ColorMapLength1 ) * sizeof ( QRgb ) );
    }
    QImage imgResult( QSize( AsShort( Header.m_Width1 ), AsShort( Header.m_Height1 ) ), QImage::Format_ARGB32 );
    QRgb * pEnd = reinterpret_cast< QRgb * >( imgResult.bits() ) - 1;
    QRgb * pPixel = pEnd + AsShort( Header.m_Width1 ) * AsShort( Header.m_Height1 );
    if ( bRLE )
        while ( pPixel > pEnd )
            ReadRLEEncoded( pData, pColorMap, pPixel, uByteCountPerPixel );
    else
        while ( pPixel > pEnd )
            ReadUnEncoded( pData, pColorMap, pPixel, uByteCountPerPixel );
    delete [] pColorMap;
    if ( pData - arrData.data() > steamIn.device()->bytesAvailable() )
        return false;
    *a_pDestImage = imgResult.mirrored( !( Header.m_ImageDescriptorBit & 0x10 ), Header.m_ImageDescriptorBit & 0x20 );
    return true;
}

void TgaHandler::setOption( ImageOption, const QVariant & )
{
}

bool TgaHandler::supportsOption( ImageOption a_Option ) const
{
    return a_Option == Size;
}

bool TgaHandler::preProcess( QIODevice * a_pDevice, SHeader * a_pDest )
{
    QByteArray arrFirstBytes = a_pDevice->peek( sizeof( SHeader ) );
    if ( arrFirstBytes.length() != sizeof( SHeader ) )
        return false;
    SHeader headerResult = *reinterpret_cast< SHeader * >( arrFirstBytes.data() );
    switch ( headerResult.m_DataTypeCode )
    {
    case 1: case 2: case 3: case 9: case 10: case 11:
        break;
    default:
        return false;
    }
    if ( ( ( headerResult.m_ColorMapDepth != 8 ) && ( headerResult.m_ColorMapDepth != 15 ) && ( headerResult.m_ColorMapDepth != 16 ) && ( headerResult.m_ColorMapDepth != 24 ) && ( headerResult.m_ColorMapDepth != 32 ) && headerResult.m_ColorMapType ) ||
         ( ( headerResult.m_BitCountPerPixel != 8 ) && ( headerResult.m_BitCountPerPixel != 15 ) && ( headerResult.m_BitCountPerPixel != 16 ) && ( headerResult.m_BitCountPerPixel != 24 ) && ( headerResult.m_BitCountPerPixel != 32 ) ) ||
         ( ( ( headerResult.m_DataTypeCode == 1 ) || ( headerResult.m_DataTypeCode == 9 ) ) && !headerResult.m_ColorMapType ) ||
         ( headerResult.m_ColorMapOrigin1 || headerResult.m_ColorMapOrigin2 ) )
        return false;
    if ( a_pDest )
        *a_pDest = headerResult;
    return true;
}
