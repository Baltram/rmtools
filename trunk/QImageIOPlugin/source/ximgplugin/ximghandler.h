#ifndef XIMGHANDLER_H_INCLUDED
#define XIMGHANDLER_H_INCLUDED

#include "../ddsplugin/ddshandler.h"

class XimgHandler :
    public DdsHandler
{
public:
    XimgHandler( void );
   ~XimgHandler( void );
public:
    static bool canRead( QIODevice * a_pDevice );
public:
    bool       canRead( void ) const;
    QByteArray name( void ) const;
    QVariant   option( ImageOption a_Option ) const;
    bool       read( QImage * a_pDestImage );
    void       setOption( ImageOption, QVariant const & );
    bool       supportsOption( ImageOption a_Option ) const;
private:
    static bool preProcess( QIODevice * a_pDevice, SImageInfo * a_pDest = 0 );
};

#endif // XIMGHANDLER_H_INCLUDED
