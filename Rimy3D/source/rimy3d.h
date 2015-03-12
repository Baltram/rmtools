#ifndef RIMY3D_H_INCLUDED
#define RIMY3D_H_INCLUDED

#include <QtGui/QApplication>
#include <QTranslator>
#include <QSettings>

class Rimy3D :
    public QApplication
{
    Q_OBJECT
public:
    enum { EVersionMajor = 0 };
    enum { EVersionMinor = 4 };
    enum ELanguage
    {
        ELanguage_English,
        ELanguage_German,
        ELanguage_Count
    };
    enum EMessage
    {
        EMessage_Message,
        EMessage_Warning,
        EMessage_Error,
        EMessage_Count
    };
public:
    Rimy3D( int & argc, char * argv[] );
   ~Rimy3D( void );
public:
    static void        checkForUpdates( bool a_bReportNetworkErrors, bool a_bReportUpToDate = false );
    static bool        checkGmaxInstallation( void );
    static Rimy3D *    getInstance( void );
    static ELanguage   getLanguage( void );
    static QSettings * getSettings( void );
    static int         getVersion( void );
    static QString     getVersionString( void );
    static void        init( int & argc, char * argv[] );
    static void        initCooking( void );
    static bool        isUpToDate( void );
    static void        loadSettings( void );
    static bool        quiet( void );
    static void        saveSettings( void );
    static void        setLanguage( ELanguage a_enuLanguage );
    static void        setQuiet( bool a_bEnabled );
    static void        showError( QString a_strText, QString a_strTitle = "Rimy3D" );
    static void        showMessage( QString a_strText, QString a_strTitle = "Rimy3D" );
    static bool        showQuestion( QString a_strText, QString a_strTitle = "Rimy3D", bool a_bDefault = false );
    static void        showWarning( QString a_strText, QString a_strTitle = "Rimy3D" );
signals:
    void message( QString const & a_strText, Rimy3D::EMessage a_enuType );
    void settingsLoading( QSettings & a_Settings );
    void settingsSaving( QSettings & a_Settings );
private:
    void loadSettingsIntern( void );
    void saveSettingsIntern( void );
private:
    static bool          s_bQuiet;
    static ELanguage     s_enuCurrentLanguage;
    static QSettings *   s_pSettings;
    static QTranslator * s_pCurrentQtTranslator;
    static QTranslator * s_pCurrentAppTranslator;
    static QTranslator * s_pGermanQtTranslator;
    static QTranslator * s_pGermanAppTranslator;
};

#endif  // RIMY3D_H_INCLUDED
