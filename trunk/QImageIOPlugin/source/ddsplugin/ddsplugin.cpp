#include <qimageiohandler.h>
#include "ddshandler.h"

class DdsPlugin :
    public QImageIOPlugin
{
public:
    DdsPlugin( void );
   ~DdsPlugin( void );
public:
    Capabilities      capabilities( QIODevice * a_pDevice, QByteArray const & a_Format ) const;
    QImageIOHandler * create( QIODevice * a_pDevice, QByteArray const & a_Format = QByteArray() ) const;
    QStringList       keys( void ) const;
};

DdsPlugin::DdsPlugin( void )
{
}

DdsPlugin::~DdsPlugin( void )
{
}

QStringList DdsPlugin::keys( void ) const
{
    return QStringList() << "dds";
}

QImageIOPlugin::Capabilities DdsPlugin::capabilities( QIODevice * a_pDevice, QByteArray const & a_Format ) const
{
    if ( a_pDevice->isOpen() && a_pDevice->isReadable() )
    {
        if ( DdsHandler::canRead( a_pDevice ) )
            return Capabilities( CanRead );
    }
    else
    {
        if ( a_Format == "dds" )
            return Capabilities( CanRead );
    }
    return 0;
}

QImageIOHandler * DdsPlugin::create( QIODevice * a_pDevice, QByteArray const & a_Format ) const
{
    QImageIOHandler * pHandler = new DdsHandler;
    pHandler->setDevice( a_pDevice );
    pHandler->setFormat( a_Format );
    return pHandler;
}

Q_EXPORT_PLUGIN( DdsPlugin )
