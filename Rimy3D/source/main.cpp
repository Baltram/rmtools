#include "rimy3d.h"
#include "mainwindow.h"
#include <QtPlugin>

Q_IMPORT_PLUGIN( TgaPlugin )
Q_IMPORT_PLUGIN( DdsPlugin )
Q_IMPORT_PLUGIN( XimgPlugin )

int main( int argc, char *argv[] )
{
    Rimy3D a( argc, argv );
    MainWindow::getInstance().show();
    if ( argc == 2 )
        MainWindow::getInstance().open( argv[ 1 ] );
    return a.exec();
}
