#ifndef PREFERENCESDIALOG_H_INCLUDED
#define PREFERENCESDIALOG_H_INCLUDED

#include <QDialog>

class QSettings;

namespace Ui
{
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PreferencesDialog( QWidget * a_pParent = 0 );
            ~PreferencesDialog( void );
public:
    bool removeAscPrefixes( void ) const;
    bool removeXmacCollisionMesh( void ) const;
public slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
protected:
    void changeEvent( QEvent * a_pEvent );
private:
    void updateLanguage( void );
private slots:
    void on_pbOk_clicked( void );
    void on_pbRestoreDefaults_clicked( void );
private:
    Ui::PreferencesDialog * m_pUi;
};

#endif // PREFERENCESDIALOG_H_INCLUDED
