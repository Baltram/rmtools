#include "rimy3d.h"
#include "mainwindow.h"

int main( int argc, char *argv[] )
{
    Rimy3D a( argc, argv );
    MainWindow mainWindow;
    mainWindow.show();
    if ( argc == 2 )
        mainWindow.open( argv[ 1 ] );
    return a.exec();
}
