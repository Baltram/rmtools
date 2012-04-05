#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow( QWidget * a_pParent = 0 );
    ~MainWindow();
public:
    void updateLanguage( void );
public slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
protected:
    void changeEvent( QEvent * a_pEvent );
    void closeEvent( QCloseEvent * a_pEvent );
private slots:
    void on_actionEnglish_triggered( void );
    void on_actionGerman_triggered( void );
private:
    Ui::MainWindow * m_pUi;
};

#endif // MAINWINDOW_H
