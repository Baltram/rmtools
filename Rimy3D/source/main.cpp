#include "main.h"
#include "rimy3d.h"
#include "mainwindow.h"
#include "preferencesdialog.h"
#include <QtPlugin>
#include <QFile>
#include <windows.h>

Q_IMPORT_PLUGIN( TgaPlugin )
Q_IMPORT_PLUGIN( DdsPlugin )
Q_IMPORT_PLUGIN( XimgPlugin )

const char * optionNames[ CliOption_Count ] =
{
    "texout",
    "normals",
    "nsgs",
    "nangle",
    "nkeep",
    "vcolors",
    "base",
    "onlyverts",
    "direct",
    "convex",
    "mtl",
    "g3ds",
    "sgs",
    "texlook",
    "matlook",
    "texin",
    "nocol",
    "noprefix"
};

extern const char * helpScreen;

ConsoleHelper::ConsoleHelper( void )
{
    m_bAttached = AttachConsole( ATTACH_PARENT_PROCESS ) ? true : false;
    if ( !m_bAttached )
        return;
    freopen( "CONOUT$", "w", stdout );
    freopen( "CONOUT$", "w", stderr );
    freopen( "CONIN$", "r", stdin );
    connect( Rimy3D::getInstance(), SIGNAL( message( QString const &, Rimy3D::EMessage ) ), this, SLOT( onMessage( QString const &, Rimy3D::EMessage ) ) );
}

ConsoleHelper::~ConsoleHelper( void )
{
    if ( !m_bAttached )
        return;
    // Send <enter> to make a new prompt in the terminal
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VK_RETURN;
    SendInput( 1, &input, sizeof( INPUT ) );
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput( 1, &input, sizeof( INPUT ) );
    FreeConsole();
}

void ConsoleHelper::onMessage( QString const & a_strText, Rimy3D::EMessage a_enuType )
{
    QString text = a_strText;
    if ( a_enuType == Rimy3D::EMessage_Error )
        text = "ERROR: " + text;
    else if ( a_enuType == Rimy3D::EMessage_Warning )
        text = "WARNING: " + text;
    printf( "%s\n", text.toLatin1().data() );
}

int main( int argc, char *argv[] )
{
    Rimy3D app( argc, argv );
    QStringList args = QCoreApplication::arguments();
    bool help = argc >= 2 && QString( args[ 1 ] ).replace( "\\", "" ).replace( "-", "" ).toLower() == "help";

    if ( argc < 3 && !help )  // GUI mode
    {
        MainWindow::getInstance().show();
        if ( argc == 2 )
            MainWindow::getInstance().open( argv[ 1 ] );
        return app.exec();
    }

    Rimy3D::setQuiet( true );
    ConsoleHelper helper = ConsoleHelper();
    if ( help )
        return printf( "\n\n%s", helpScreen ), 0;

    QString inputFilePath = args[ argc - 2 ];
    QString outputFilePath = args[ argc - 1 ];
    if ( inputFilePath.startsWith( "-" ) || outputFilePath.startsWith( "-" ) )
        return printf( "ERROR: Incorrect usage. The last two arguments must be file paths.\n" ), 1;

    QVariant options[ CliOption_Count ];
    for ( int i = 1; i < argc - 2; ++i )
    {
        QString arg = args[ i ].toLower();
        if ( !arg.startsWith( "-" ) )
            return printf( "ERROR: Expected option instead of %s\n", argv[ i ] ), 1;
        arg.remove( 0, 1 );

        CliOption o = CliOption_Count;
        for ( int j = 0; j < CliOption_Count; ++j )
            if ( arg == optionNames[ j ] )
                o = static_cast< CliOption >( j );
        if ( o == CliOption_Count )
            return printf( "ERROR: Unknown option %s\n", argv[ i ] ), 1;

        QString value = args[ ++i ];
        switch ( o )  // parse the option's value
        {
            case CliOption_NormalsKeepIfPossible:
            case CliOption_NormalsRecalcBySmoothingGroups:
                options[o] = true;
                --i;  // these options have no value, therefore undo the increment from before
                break;
            case CliOption_ExportTextures:
                if ( value == "0" )
                {
                    options[ o ] = false;
                    break;
                }
                // fall through
            case CliOption_DefaultImageFileExt:
                value = value.toLower();
                if ( value != "tga" && value != "jpg" && value != "png" && value != "bmp" )
                    return printf( "ERROR: Unknown image format: %s\n", argv[ i ] ), 1;
                options[ o ] = value;
                break;
            case CliOption_NormalsRecalcByAngle:
                if ( value != "0" && ( value.toInt() <= 0 || value.toInt() > 180 ) )
                    return printf( "ERROR: Invalid angle: %s\n", argv[ i ] ), 1;
                options[ o ] = value.toInt();
                break;
            case CliOption_BaseFile:
                if ( !QFile::exists( value ) )
                    return printf( "ERROR: Base file not found: %s\n", argv[ i ] ), 1;
                options[ o ] = value;
                break;
            default:
                if ( value != "0" && value != "1" )
                    return printf( "ERROR: Invalid value: %s\n", argv[ i ] ), 1;
                options[ o ] = value.toInt() ? true : false;
                break;
        }
        if ( i >= argc - 2 )  // no output file specified
            return printf( helpScreen ), 1;
    }

    MainWindow::getInstance().applyCliOptions( options );  // All GUI settings are loaded upon MainWindow instanciation
    PreferencesDialog::getInstance().applyCliOptions( options );
    Rimy3D::setLanguage( Rimy3D::ELanguage_English );  // CLI mode only available in English
    if ( !MainWindow::getInstance().open( inputFilePath ) )
        return 1;
    if ( !MainWindow::getInstance().save( outputFilePath ) )
        return 1;
    printf( "Done.\n" );
    return 0;
}

const char * helpScreen =
    "Usage:\n"
    "\n"
    "  1. Rimy3D.exe\n"
    "     - Start the application in GUI mode.\n"
    "\n"
    "  2. Rimy3D.exe <input file path>\n"
    "     - Open the specified file in GUI mode.\n"
    "\n"
    "  3. Rimy3D.exe -help\n"
    "     - Display this help screen.\n"
    "\n"
    "  4. Rimy3D.exe [options] <input file path> <output file path>\n"
    "     - Convert the input file to the output file in non-GUI mode using the\n"
    "       import/export settings as they were last set in the GUI. To override\n"
    "       these settings, use the optional arguments below.\n"
    "\n"
    "Note: Paths containing spaces should be enclosed in quotes.\n"
    "\n"
    "Export options:\n"
    "\n"
    "   (for .3db, .asc .ase, .obj, .xcmsh, .xact, xlmsh, ._xmsh, ._xmac)\n"
    "  -texout <0|tga|jpg|png|bmp>   Export textures in the specified format or \n"
    "                                disable (0) texture export.\n"
    "\n"
    "   (for .ase, .obj)\n"
    "  -normals <0|1>      Enable (1) or disable (0) the export of vertex normals.\n"
    "\n"
    "   (for .3db, .ase, .obj, .xcmsh, .xact, ._xmsh, ._xmac)\n"
    "  -nsgs               Recalculate vertex normals using smoothing groups.\n"
    "  -nangle <0-180>     Recalculate vertex normals based on the specified angle.\n"
    "  -nkeep              Keeps existing vertex normals if possible (except .3db).\n"
    "\n"
    "   (for .ase, .3db, .xcmsh, ._xmsh)\n"
    "  -vcolors <0|1>      Enable (1) or disable (0) the export of vertex colors.\n"
    "\n"
    "   (for .xact, ._xmac)\n"
    "  -base <file path>   Specifiy the base .xact or ._xmac_ file.\n"
    "  -onlyverts <0|1>    If enabled (1), transplant only the vertices.\n"
    "  -direct <0|1>       Use direct (1) or indirect (0) auto-skin vertex matching.\n"
    "\n"
    "   (for .xnvmsh, ._xcom)\n"
    "  -convex <0|1>       Specify whether the input mesh is convex.\n"
    "\n"
    "   (for .obj)\n"
    "  -mtl <0|1>          Enable (1) or disable (0) the export of .mtl materials.\n"
    "\n"
    "   (for .3ds)\n"
    "  -g3ds <0|1>         Enable (1) or disable (0) the use of Gothic 3DS format.\n"
    "  -sgs <0|1>          Enable (1) or disable (0) the export of smoothing groups\n"
    "                      (for .3db, .asc and .ase, this is always enabled).\n"
    "\n"
    "Import options:\n"
    "\n"
    "  -texlook <0|1>      Enable (1) or disable (0) texture lookup using the\n"
    "                      search paths and archives specified in the GUI.\n"
    "\n"
    "   (for .xcmsh, .xact, ._xmsh, ._xmac)\n"
    "  -matlook <0|1>      Enable (1) or disable (0) material lookup using the\n"
    "                      search paths and archives specified in the GUI.\n"
    "  -texin <tga|jpg|png|bmp>   Specify the texture file type.\n"
    "\n"
    "   (for ._xmac)\n"
    "  -nocol <0|1>        Remove (1) or keep (0) the collision mesh.\n"
    "\n"
    "   (for .asc)\n"
    "  -noprefix <0|1>     Remove (1) or keep (0) the 'ZM_' and 'Bip01' prefixes.\n"
    "\n"
    "Examples:\n"
    "  - To convert a .xact file to .3db, while importing and saving any textures in\n"
    "    the .jpg file format, and using all other settings as set in the GUI:\n"
    "  \n"
    "  Rimy3D.exe -texin jpg -texout jpg \"C:\\input.xact\" \"C:\\output.3db\"\n"
    "\n"
    "  - To convert a .3db file to .xact using base.xact as a base file, with  \n"
    "    texture lookup disabled, recalculating normals based on an angle of 180\n"
    "    degrees and not exporting textures:\n"
    "  \n"
    "  Rimy3D.exe -texlook 0 -texout 0 -nangle 180 -base \"C:\\base.xact\" \\\n"
    "             \"C:\\input.3db\" \"C:\\output.xact\"\n";
