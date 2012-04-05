#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rimy3d.h"

MainWindow::MainWindow(QWidget * a_pParent) :
    QMainWindow( a_pParent ),
    m_pUi( new Ui::MainWindow )
{
    m_pUi->setupUi( this );
    connect( Rimy3D::getInstance(), SIGNAL( onSaveSettings( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( onLoadSettings( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
    Rimy3D::loadSettings();
    updateLanguage();
}

MainWindow::~MainWindow()
{
    delete m_pUi;
}

void MainWindow::updateLanguage( void )
{
    char const * const arrIconFileNames[ Rimy3D::ELanguage_Count ] =
    {
        "gb.png",
        "de.png"
    };
    m_pUi->menuLanguage->setIcon( QIcon( QString( ":/flags/icons/" ).append( arrIconFileNames[ Rimy3D::getLanguage() ] ) ) );
    m_pUi->retranslateUi( this );
}

void MainWindow::loadSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "MainWindow" );
    resize( a_Settings.value( "size", size() ).toSize() );
    move( a_Settings.value( "pos", pos() ).toPoint() );
    a_Settings.endGroup();
}

void MainWindow::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "MainWindow" );
    a_Settings.setValue( "size", size() );
    a_Settings.setValue( "pos", pos() );
    a_Settings.endGroup();
}

void MainWindow::changeEvent( QEvent * a_pEvent )
{
    if ( a_pEvent->type() == QEvent::LanguageChange )
        updateLanguage();
    QMainWindow::changeEvent( a_pEvent );
}

void MainWindow::closeEvent( QCloseEvent * a_pEvent )
{
    Rimy3D::saveSettings();
    QMainWindow::closeEvent( a_pEvent );
}

void MainWindow::on_actionEnglish_triggered( void )
{
    Rimy3D::setLanguage( Rimy3D::ELanguage_English );
}

void MainWindow::on_actionGerman_triggered( void )
{
    Rimy3D::setLanguage( Rimy3D::ELanguage_German );
}
