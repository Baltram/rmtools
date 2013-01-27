#include "rimy3d.h"
#include "mainwindow.h"
#include <QtPlugin>

Q_IMPORT_PLUGIN( TgaPlugin )
Q_IMPORT_PLUGIN( DdsPlugin )
Q_IMPORT_PLUGIN( XimgPlugin )

int main( int argc, char *argv[] )
{
    Rimy3D a( argc, argv );
    MainWindow mainWindow;
    mainWindow.show();
    if ( argc == 2 )
        mainWindow.open( argv[ 1 ] );
    return a.exec();
}
