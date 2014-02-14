#include "rimy3d.h"
#include "mainwindow.h"
#include "networkmanager.h"
#include "zlib.h"
#include "Mimicry.h"
#include <QMessageBox>
#include <QHash>
#include <QDir>
#include <QLibrary>
#include <QSystemLocale>

#ifdef Q_WS_WIN
#include "mi_macros.h"
#include "Windows.h"
#include <QSysInfo>
#include <QFileInfo>
#pragma comment ( lib, "Shell32.lib" )
#endif

bool Rimy3D::s_bQuiet = false;
Rimy3D::ELanguage Rimy3D::s_enuCurrentLanguage = ELanguage_English;
QSettings * Rimy3D::s_pSettings = 0;
QTranslator * Rimy3D::s_pCurrentQtTranslator = 0;
QTranslator * Rimy3D::s_pCurrentAppTranslator = 0;
QTranslator * Rimy3D::s_pGermanQtTranslator = 0;
QTranslator * Rimy3D::s_pGermanAppTranslator = 0;

Rimy3D::Rimy3D( int & argc, char * argv[] ) :
    QApplication( argc, argv )
{
    QCoreApplication::setOrganizationName( "Baltram" );
    QCoreApplication::setApplicationName( "Rimy3D" );
    s_pSettings = new QSettings;
    mCGenomeVolume::RegisterZlibUncompressFunction( &uncompress );
    initCooking();
}

Rimy3D::~Rimy3D( void )
{
}

void Rimy3D::checkForUpdates( bool a_bReportNetworkErrors, bool a_bReportUpToDate )
{
    struct SUpdateChecker
    {
        SUpdateChecker( void ) : m_iState( 0 ), m_bError( false ) {}
        static SUpdateChecker & getInstance( void )
        {
            static SUpdateChecker * s_pInstance = new SUpdateChecker;
            return *s_pInstance;
        }
        void report( void )
        {
            if ( m_iState < 3 )
                return;
            if ( m_bError && m_bReportErrors )
                showMessage( tr( "Could not connect to http://www.baltr.de." ), tr( "Rimy3D Update" ) );
            else if ( !m_bError )
            {
                if ( m_iVersionNumber > Rimy3D::getVersion() )
                    showMessage( m_strText, tr( "Rimy3D Update" ) );
                else if ( m_bReportUpToDate )
                    showMessage( tr( "Rimy3D is up to date." ), tr( "Rimy3D Update" ) );
            }
            m_bError = false;
            m_iState = 0;
        }
        static void versionNumber( QByteArray a_arrData, QNetworkReply::NetworkError a_Error )
        {
            if ( !( getInstance().m_bError |= ( a_Error != QNetworkReply::NoError ) ) )
                getInstance().m_iVersionNumber = QString( a_arrData ).toInt();
            ++getInstance().m_iState, getInstance().report();
        }
        static void versionText( QByteArray a_arrData, QNetworkReply::NetworkError a_Error )
        {
            if ( !( getInstance().m_bError |= ( a_Error != QNetworkReply::NoError ) ) )
                getInstance().m_strText = a_arrData;
            ++getInstance().m_iState, getInstance().report();
        }
        bool isReady( void )
        {
            return ( m_iState == 0 );
        }
        int     m_iState;
        bool    m_bError;
        bool    m_bReportErrors;
        bool    m_bReportUpToDate;
        int     m_iVersionNumber;
        QString m_strText;
    };
    if ( !SUpdateChecker::getInstance().isReady() )
    {
        SUpdateChecker::getInstance().m_bReportErrors |= a_bReportNetworkErrors;
        SUpdateChecker::getInstance().m_bReportUpToDate |= a_bReportUpToDate;
        return;
    }
    ++SUpdateChecker::getInstance().m_iState;
    SUpdateChecker::getInstance().m_bReportErrors = a_bReportNetworkErrors;
    SUpdateChecker::getInstance().m_bReportUpToDate = a_bReportUpToDate;
    NetworkManager::startRequest( "http://www.baltr.de/rimy3d/version.txt", SUpdateChecker::versionNumber );
    NetworkManager::startRequest( "http://www.baltr.de/rimy3d/newversion_" + tr( "en" ) + ".htm", SUpdateChecker::versionText );
}

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
                    QByteArray arrTemp( ( "\"" + arrPossibleLocations[ i ] + "\"" ).toAscii() );
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
    return dynamic_cast< Rimy3D * >( qApp );
}

Rimy3D::ELanguage Rimy3D::getLanguage( void )
{
    return s_enuCurrentLanguage;
}

QSettings * Rimy3D::getSettings( void )
{
    return s_pSettings;
}

int Rimy3D::getVersion( void )
{
    return ( EVersionMajor << 16 ) + ( EVersionMinor << 8 );
}

QString Rimy3D::getVersionString( void )
{
    return QString::number( EVersionMajor ) + "." + QString::number( EVersionMinor );
}

namespace
{
    void unloadModule( char const * a_pcName )
    {
        a_pcName;
#ifdef Q_WS_WIN
        FreeLibrary( GetModuleHandleA( a_pcName ) );
#endif
    }

    void requestPhysXLoaderFunctions( void )
    {
        QLibrary libPhysXLoader( "PhysXLoader" );
        mCCooking::RegisterPhysXLoaderFunctions( libPhysXLoader.resolve( "NxCreatePhysicsSDK" ), libPhysXLoader.resolve( "NxReleasePhysicsSDK" ), libPhysXLoader.resolve( "NxGetCookingLib" ) );
    }
}

void Rimy3D::initCooking( void )
{
    static bool s_bInit = false;
    if ( s_bInit )
        return;
    requestPhysXLoaderFunctions();
    mCCooking::RegisterMultiVersionFunctions( &unloadModule, &requestPhysXLoaderFunctions );
    s_bInit = true;
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

void Rimy3D::setQuiet( bool a_bEnabled )
{
    s_bQuiet = a_bEnabled;
}

void Rimy3D::showError( QString a_strText, QString a_strTitle )
{
    if ( !quiet() )
        QMessageBox::critical( QApplication::activeWindow(), a_strTitle, a_strText );
}

void Rimy3D::showMessage( QString a_strText, QString a_strTitle )
{
    if ( !quiet() )
        QMessageBox::about( QApplication::activeWindow(), a_strTitle, a_strText );
}

bool Rimy3D::showQuestion( QString a_strText, QString a_strTitle, bool a_bDefault )
{
    return quiet() ? a_bDefault : ( QMessageBox::Yes == QMessageBox::question( QApplication::activeWindow(), a_strTitle, a_strText, QMessageBox::Yes | QMessageBox::No ) );
}

void Rimy3D::showWarning( QString a_strText, QString a_strTitle )
{
    if ( !quiet() )
        QMessageBox::warning( QApplication::activeWindow(), a_strTitle, a_strText );
}

void Rimy3D::loadSettingsIntern( void )
{
    QSettings GeneralSettings( "Baltram", "general" );
    QVariant varDefault( static_cast< int >( ELanguage_Count ) );
    ELanguage enuLanguage = static_cast< ELanguage >( s_pSettings->value( "language", varDefault ).toInt() );
    if ( enuLanguage == ELanguage_Count )
    {
        enuLanguage = ELanguage_English;
        switch ( QSystemLocale().query( QSystemLocale::LanguageId, QVariant() ).toInt() )
        {
        case QLocale::German:
        case QLocale::SwissGerman:
        case QLocale::LowGerman:
            enuLanguage = ELanguage_German;
        }
    }
#ifdef Q_WS_WIN
    if ( ( ( QSysInfo::WindowsVersion == QSysInfo::WV_VISTA ) || ( QSysInfo::WindowsVersion == QSysInfo::WV_WINDOWS7 ) ) &&
         ( !s_pSettings->contains( "MAXComponents" ) ) )
        checkGmaxInstallation();
#endif
    setLanguage( enuLanguage );
    emit settingsLoading( *s_pSettings );
}

void Rimy3D::saveSettingsIntern( void )
{
    s_pSettings->setValue( "language", static_cast< int >( s_enuCurrentLanguage ) );
    s_pSettings->setValue( "exePath", QDir::toNativeSeparators( QCoreApplication::applicationFilePath() ) );
    emit settingsSaving( *s_pSettings );
}
