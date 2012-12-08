#include "rimy3d.h"
#include <QMessageBox>

#ifdef Q_WS_WIN
#include "mi_macros.h"
#include "Windows.h"
#include <QSysInfo>
#include <QFileInfo>
#pragma comment ( lib, "Shell32.lib" )
#endif

bool Rimy3D::s_bQuiet = false;
Rimy3D::ELanguage Rimy3D::s_enuCurrentLanguage = ELanguage_English;
Rimy3D * Rimy3D::s_pInstance = 0;
QSettings * Rimy3D::s_pSettings = 0;
QWidget * Rimy3D::s_pMainWindow = 0;
QTranslator * Rimy3D::s_pCurrentQtTranslator = 0;
QTranslator * Rimy3D::s_pCurrentAppTranslator = 0;
QTranslator * Rimy3D::s_pGermanQtTranslator = 0;
QTranslator * Rimy3D::s_pGermanAppTranslator = 0;

bool Rimy3D::checkGmaxInstallation( void )
{
#ifdef Q_WS_WIN
    QSettings settingsClasses( "HKEY_CLASSES_ROOT\\MAXComponents.SkinEngine", QSettings::NativeFormat );
    if ( !settingsClasses.childGroups().count() )
    {
        settingsClasses.clear();
        QSettings settingsGmax1( "HKEY_CLASSES_ROOT\\3DStudio.GMAX\\shell\\open\\command", QSettings::NativeFormat );
        QSettings settingsGmax2( "HKEY_CLASSES_ROOT\\Applications\\gmax.exe\\shell\\open\\command", QSettings::NativeFormat );
        MI_CRT_NO_WARNINGS( QString strGMaxLoc = getenv( "GMAXLOC" ); )
        QStringList arrPossibleLocations;
        arrPossibleLocations << settingsGmax1.value( "Default", "" ).toString().replace( "gmax.exe %1", "MAXComponents.dll" )
                             << settingsGmax2.value( "Default", "" ).toString().replace( "\"", "" ).replace( "gmax.exe %1", "MAXComponents.dll" )
                             << strGMaxLoc.append( "MAXComponents.dll" );
        for ( int i = arrPossibleLocations.count(); i--; )
        {
            if ( QFileInfo( arrPossibleLocations[ i ] ).exists() )
            {
                bool bResult = showQuestion( tr( "Rimy3D detected that you are using GMax under Windows " ) +
                                             QString( QSysInfo::WindowsVersion == QSysInfo::WV_VISTA ? "Vista" : "7" ) +
                                             tr( " without having registered the file MAXComponents.dll in the windows registry.\n\n"
                                                 "This may cause errors related to the 'Skin' modifier. "
                                                 "It is highly recommended to let Rimy3D fix this. Continue?" ) );
                if ( bResult )
                {
                    QByteArray arrTemp( arrPossibleLocations[ i ].toAscii() );
                    SHELLEXECUTEINFOA Info = { 0 };
                    Info.cbSize = sizeof( SHELLEXECUTEINFO );
                    Info.fMask = SEE_MASK_NOCLOSEPROCESS;
                    Info.hwnd = NULL;
                    Info.lpVerb = "runas";
                    Info.lpFile = "regsvr32.exe";
                    Info.lpParameters = arrTemp.constData();
                    Info.lpDirectory = NULL;
                    Info.nShow = SW_SHOW;
                    Info.hInstApp = NULL;
                    ShellExecuteExA( &Info );
                    WaitForSingleObject( Info.hProcess, INFINITE );
                }
                settingsClasses.sync();
                bResult = settingsClasses.childGroups().count() != 0;
                s_pSettings->setValue( "MAXComponents", bResult );
                return bResult;
            }
        }
    }
#endif
    return true;
}

Rimy3D * Rimy3D::getInstance( void )
{
    return s_pInstance;
}

Rimy3D::ELanguage Rimy3D::getLanguage( void )
{
    return s_enuCurrentLanguage;
}

QSettings * Rimy3D::getSettings( void )
{
    return s_pSettings;
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

bool Rimy3D::quiet( void )
{
    return s_bQuiet;
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
        s_pGermanQtTranslator->load( ":/translations/translations/qt_de" );
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

void Rimy3D::setQuiet( bool a_bEnabled )
{
    s_bQuiet = a_bEnabled;
}

void Rimy3D::showError( QString a_strText, QString a_strTitle )
{
    if ( !quiet() )
        QMessageBox::critical( s_pMainWindow, a_strTitle, a_strText );
}

void Rimy3D::showMessage( QString a_strText, QString a_strTitle )
{
    if ( !quiet() )
        QMessageBox::about( s_pMainWindow, a_strTitle, a_strText );
}

bool Rimy3D::showQuestion( QString a_strText, QString a_strTitle, bool a_bDefault )
{
    return quiet() ? a_bDefault : ( QMessageBox::Yes == QMessageBox::question( s_pMainWindow, a_strTitle, a_strText, QMessageBox::Yes | QMessageBox::No ) );
}

void Rimy3D::showWarning( QString a_strText, QString a_strTitle )
{
    if ( !quiet() )
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
#ifdef Q_WS_WIN
    if ( ( ( QSysInfo::WindowsVersion == QSysInfo::WV_VISTA ) || ( QSysInfo::WindowsVersion == QSysInfo::WV_WINDOWS7 ) ) &&
         ( !s_pSettings->contains( "MAXComponents" ) ) )
        checkGmaxInstallation();
#endif
    emit settingsLoading( *s_pSettings );
    setLanguage( enuLanguage );
}

void Rimy3D::saveSettingsIntern( void )
{
    s_pSettings->setValue( "language", static_cast< int >( s_enuCurrentLanguage ) );
    emit settingsSaving( *s_pSettings );
}
