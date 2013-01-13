#include <qimageiohandler.h>
#include "ximghandler.h"

class XimgPlugin :
    public QImageIOPlugin
{
public:
    XimgPlugin( void );
   ~XimgPlugin( void );
public:
    Capabilities      capabilities( QIODevice * a_pDevice, QByteArray const & a_Format ) const;
    QImageIOHandler * create( QIODevice * a_pDevice, QByteArray const & a_Format = QByteArray() ) const;
    QStringList       keys( void ) const;
};

XimgPlugin::XimgPlugin( void )
{
}

XimgPlugin::~XimgPlugin( void )
{
}

QStringList XimgPlugin::keys( void ) const
{
    return QStringList() << "ximg" << "_ximg";
}

QImageIOPlugin::Capabilities XimgPlugin::capabilities( QIODevice * a_pDevice, QByteArray const & a_Format ) const
{
    if ( a_pDevice->isOpen() && a_pDevice->isReadable() )
    {
        if ( XimgHandler::canRead( a_pDevice ) )
            return Capabilities( CanRead );
    }
    else
    {
        if ( ( a_Format == "ximg" ) || ( a_Format == "_ximg" ) )
            return Capabilities( CanRead );
    }
    return 0;
}

QImageIOHandler * XimgPlugin::create( QIODevice * a_pDevice, QByteArray const & a_Format ) const
{
    QImageIOHandler * pHandler = new XimgHandler;
    pHandler->setDevice( a_pDevice );
    pHandler->setFormat( a_Format );
    return pHandler;
}

Q_EXPORT_PLUGIN( XimgPlugin )
