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
    static PreferencesDialog & getInstance( void );
public:
    bool    autoUpdate( void ) const;
    QString defaultImageFileExt( void ) const;
    bool    lookUpTextures( void ) const;
    bool    removeAscPrefixes( void ) const;
    bool    removeXmacCollisionMesh( void ) const;
    bool    showTransformGizmos( void ) const;
signals:
    void materialLookupRequested( void );
protected:
    void changeEvent( QEvent * a_pEvent );
private:
    PreferencesDialog( void );
   ~PreferencesDialog( void );
private:
    void updateLanguage( void );
private slots:
    void loadSettings( QSettings & a_Settings );
    void on_cbLookUpMaterials_toggled( bool a_bChecked );
    void on_pbLookUpMaterials_clicked( void );
    void on_pbOk_clicked( void );
    void on_pbRestoreDefaults_clicked( void );
    void on_pbUpdate_clicked( void );
    void saveSettings( QSettings & a_Settings );
private:
    Ui::PreferencesDialog * m_pUi;
};

#endif // PREFERENCESDIALOG_H_INCLUDED
