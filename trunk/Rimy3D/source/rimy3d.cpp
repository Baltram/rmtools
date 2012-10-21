#include "rimy3d.h"
#include <QMessageBox>


Rimy3D::ELanguage Rimy3D::s_enuCurrentLanguage = ELanguage_English;
Rimy3D * Rimy3D::s_pInstance = 0;
QSettings * Rimy3D::s_pSettings = 0;
QWidget * Rimy3D::s_pMainWindow = 0;
QTranslator * Rimy3D::s_pCurrentQtTranslator = 0;
QTranslator * Rimy3D::s_pCurrentAppTranslator = 0;
QTranslator * Rimy3D::s_pGermanQtTranslator = 0;
QTranslator * Rimy3D::s_pGermanAppTranslator = 0;

Rimy3D * Rimy3D::getInstance( void )
{
    return s_pInstance;
}

Rimy3D::ELanguage Rimy3D::getLanguage( void )
{
    return s_enuCurrentLanguage;
}

void Rimy3D::init( int & argc, char * argv[] )
{
    if ( s_pInstance )
        return;
    QCoreApplication::setOrganizationName( "Baltram" );
    QCoreApplication::setApplicationName( "Rimy3D" );
    s_pInstance = new Rimy3D( argc, argv );
    s_pSettings = new QSettings;
}

void Rimy3D::loadSettings( void )
{
    if ( s_pSettings && getInstance() )
        getInstance()->loadSettingsIntern();
}

void Rimy3D::saveSettings( void )
{
    if ( s_pSettings && getInstance() )
        getInstance()->saveSettingsIntern();
}

void Rimy3D::setLanguage( ELanguage a_enuLanguage )
{
    if ( !s_pGermanQtTranslator )
    {
        s_pGermanQtTranslator = new QTranslator;
        s_pGermanQtTranslator->load( "qt_de" );
        s_pGermanAppTranslator = new QTranslator;
        s_pGermanAppTranslator->load( ":/translations/translations/Rimy3D_de" );
    }
    if ( s_pCurrentQtTranslator )
    {
        removeTranslator( s_pCurrentQtTranslator );
        removeTranslator( s_pCurrentAppTranslator );
        s_pCurrentQtTranslator = s_pCurrentAppTranslator = 0;
    }
    switch ( a_enuLanguage )
    {
    case ELanguage_German:
        s_pCurrentQtTranslator = s_pGermanQtTranslator;
        s_pCurrentAppTranslator = s_pGermanAppTranslator;
    }
    if ( s_pCurrentQtTranslator )
    {
        installTranslator( s_pCurrentQtTranslator );
        installTranslator( s_pCurrentAppTranslator );
    }
    s_enuCurrentLanguage = a_enuLanguage;
}

void Rimy3D::setMainWindow( QWidget * a_pMainWindow )
{
    s_pMainWindow = a_pMainWindow;
}

void Rimy3D::showError( QString a_strText, QString a_strTitle )
{
    QMessageBox::critical( s_pMainWindow, a_strTitle, a_strText );
}

void Rimy3D::showMessage( QString a_strText, QString a_strTitle )
{
    QMessageBox::about( s_pMainWindow, a_strTitle, a_strText );
}

void Rimy3D::showWarning( QString a_strText, QString a_strTitle )
{
    QMessageBox::warning( s_pMainWindow, a_strTitle, a_strText );
}

Rimy3D::Rimy3D( int & argc, char * argv[] ) :
    QApplication( argc, argv )
{
}

Rimy3D::~Rimy3D( void )
{
}

void Rimy3D::loadSettingsIntern( void )
{
    QSettings GeneralSettings( "Baltram", "general" );
    QVariant varDefault( static_cast< int >( ELanguage_Count ) );
    ELanguage enuLanguage = static_cast< ELanguage >( s_pSettings->value( "language", varDefault ).toInt() );
    if ( enuLanguage == ELanguage_Count )
    {
        switch ( GeneralSettings.value( "NSISLanguage" ).toInt() )
        {
        case 1031:
            enuLanguage = ELanguage_German;
            break;
        default:
            enuLanguage = ELanguage_English;
        }
    }
    emit onLoadSettings( *s_pSettings );
    setLanguage( enuLanguage );
}

void Rimy3D::saveSettingsIntern( void )
{
    s_pSettings->setValue( "language", static_cast< int >( s_enuCurrentLanguage ) );
    emit onSaveSettings( *s_pSettings );
}
