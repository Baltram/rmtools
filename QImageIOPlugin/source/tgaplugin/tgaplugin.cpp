#include <qimageiohandler.h>
#include "tgahandler.h"

class TgaPlugin :
    public QImageIOPlugin
{
public:
    TgaPlugin( void );
   ~TgaPlugin( void );
public:
    Capabilities      capabilities( QIODevice * a_pDevice, QByteArray const & a_Format ) const;
    QImageIOHandler * create( QIODevice * a_pDevice, QByteArray const & a_Format = QByteArray() ) const;
    QStringList       keys( void ) const;
};

TgaPlugin::TgaPlugin( void )
{
}

TgaPlugin::~TgaPlugin( void )
{
}

QStringList TgaPlugin::keys( void ) const
{
    return QStringList() << "tga";
}

QImageIOPlugin::Capabilities TgaPlugin::capabilities( QIODevice * a_pDevice, QByteArray const & a_Format ) const
{
    if ( a_pDevice->isOpen() && a_pDevice->isReadable() )
    {
        if ( TgaHandler::canRead( a_pDevice ) )
            return Capabilities( CanRead );
    }
    else
    {
        if ( a_Format == "tga" )
            return Capabilities( CanRead );
    }
    return 0;
}

QImageIOHandler * TgaPlugin::create( QIODevice * a_pDevice, QByteArray const & a_Format ) const
{
    QImageIOHandler * pHandler = new TgaHandler;
    pHandler->setDevice( a_pDevice );
    pHandler->setFormat( a_Format );
    return pHandler;
}

Q_EXPORT_PLUGIN( TgaPlugin )
