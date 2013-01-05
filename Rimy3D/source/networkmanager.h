#ifndef NETWORKMANAGER_H_INCLUDED
#define NETWORKMANAGER_H_INCLUDED

#include <QHash>
#include <QNetworkReply>

class NetworkManager :
    public QObject
{
    Q_OBJECT
public:
    typedef void ( *Callback )( QByteArray a_arrData, QNetworkReply::NetworkError a_Error );
public:
    static void startRequest( QString a_strUrl, Callback a_fpCallback );
private:
    static NetworkManager * getInstance( void );
private slots:
    void replyFinished( QNetworkReply * a_pReply );
    void timeout( void );
private:
    NetworkManager( void );
   ~NetworkManager( void );
private:
    static NetworkManager * s_pInstance;
private:
    QHash< QNetworkReply *, Callback > m_mapCallbacks;
    QNetworkAccessManager *            m_AccessManager;
};

#endif // NETWORKMANAGER_H_INCLUDED
