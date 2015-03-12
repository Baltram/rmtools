#ifndef BATCHDIALOG_H_INCLUDED
#define BATCHDIALOG_H_INCLUDED

#include <QDialog>
#include "rimy3d.h"

class QDir;
class QSettings;
class exportSettingsDialog;

namespace Ui
{
    class BatchDialog;
}

class BatchDialog : public QDialog
{
    Q_OBJECT
public:
    static BatchDialog & getInstance( void );
protected:
    void changeEvent( QEvent * a_pEvent );
    void closeEvent ( QCloseEvent * a_pEvent );
private:
    BatchDialog( void );
   ~BatchDialog( void );
private:
    void abort( void );
    void addFilesFromDirectory( const QDir & a_Dir );
    void run( exportSettingsDialog const * a_pDialog ) const;
    void stateChanged( void );
    void updateLanguage( void );
private slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
    void on_cbOutputToSourceDirs_clicked( void );
    void on_pbAddDirectory_clicked( void );
    void on_pbAddFiles_clicked( void );
    void on_pbCancel_clicked( void );
    void on_pbChooseOutputFolder_clicked( void );
    void on_pbRemove_clicked( void );
    void on_pbRemoveAll_clicked( void );
    void on_pbStart_clicked( void );
    void onFinished( void );
    void onMessage( QString const & a_strText, Rimy3D::EMessage a_enuType );
private:
    static QGLContext * s_pContext;
private:
    Ui::BatchDialog * m_pUi;
    bool              m_bRunning;
    bool              m_bAborted;
};

#endif  // BATCHDIALOG_H_INCLUDED
