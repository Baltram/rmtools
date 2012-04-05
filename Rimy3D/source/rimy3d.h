#ifndef RIMY3D_H_INCLUDED
#define RIMY3D_H_INCLUDED

#include <QtGui/QApplication>
#include <QTranslator>
#include <QSettings>

class Rimy3D : public QApplication
{
    Q_OBJECT
public:
    enum ELanguage
    {
        ELanguage_English,
        ELanguage_German,
        ELanguage_Count
    };
public:
    static Rimy3D *  getInstance( void );
    static void      init( int & argc, char * argv[] );
    static void      loadSettings( void );
    static ELanguage getLanguage( void );
    static void      saveSettings( void );
    static void      setLanguage( ELanguage a_enuLanguage );
    static void      setMainWindow( QWidget * a_pMainWindow );
    static void      showError( QString a_strText, QString a_strTitle = "Rimy3D" );
    static void      showMessage( QString a_strText, QString a_strTitle = "Rimy3D" );
    static void      showWarning( QString a_strText, QString a_strTitle = "Rimy3D" );
signals:
    void onSaveSettings( QSettings & a_Settings );
    void onLoadSettings( QSettings & a_Settings );
protected:
    Rimy3D( int & argc, char * argv[] );
    ~Rimy3D( void );
private:
    void loadSettingsIntern( void );
    void saveSettingsIntern( void );
private:
    static ELanguage     s_enuCurrentLanguage;
    static Rimy3D *      s_pInstance;
    static QSettings *   s_pSettings;
    static QWidget *     s_pMainWindow;
    static QTranslator * s_pCurrentQtTranslator;
    static QTranslator * s_pCurrentAppTranslator;
    static QTranslator * s_pGermanQtTranslator;
    static QTranslator * s_pGermanAppTranslator;
};

#endif  // RIMY3D_H_INCLUDED
