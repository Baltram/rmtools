#include "networkmanager.h"
#include <QTimer>

NetworkManager * NetworkManager::s_pInstance = 0;

void NetworkManager::startRequest( QString a_strUrl, Callback a_fpCallback )
{
    if ( !getInstance() )
        return;
    QNetworkReply * pReply = getInstance()->m_AccessManager->get( QNetworkRequest( QUrl( a_strUrl ) ) );
    QTimer * pTimer = new QTimer( pReply );
    connect( pTimer, SIGNAL( timeout( void ) ), getInstance(), SLOT( timeout( void ) ) );
    pTimer->start( 5000 );
    getInstance()->m_mapCallbacks[ pReply ] = a_fpCallback;
}

NetworkManager * NetworkManager::getInstance( void )
{
    static bool s_bInit = false;
    if ( !s_bInit )
        s_bInit = true, s_pInstance = new NetworkManager;
    return s_pInstance;
}

void NetworkManager::replyFinished( QNetworkReply * a_pReply )
{
    ( *m_mapCallbacks[ a_pReply ] )( a_pReply->readAll(), a_pReply->error() );
    m_mapCallbacks.remove( a_pReply );
    a_pReply->deleteLater();
}

void NetworkManager::timeout( void )
{
    dynamic_cast< QNetworkReply * >( QObject::sender()->parent() )->abort();
}

NetworkManager::NetworkManager( void )
{
    m_AccessManager = new QNetworkAccessManager;
    connect( m_AccessManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( replyFinished ( QNetworkReply * ) ) );
}

NetworkManager::~NetworkManager( void )
{
    for ( QHash< QNetworkReply *, Callback >::iterator i = m_mapCallbacks.begin(), ie = m_mapCallbacks.end(); i != ie; ++i )
        delete i.key();
    delete m_AccessManager;
}

