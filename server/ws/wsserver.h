#ifndef WSSERVER_H
#define WSSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include "bannercache.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WSServer : public QObject
{
	Q_OBJECT
public:
	explicit WSServer(quint16 port,
	                  quint16 displayWidth, quint16 displayHeight,
	                  quint16 thumbWidth, quint16 thumbHeight,
	                  quint16 bannerX, quint16 bannerY,
	                  const QString &bannerDir,
	                  QObject *parent = Q_NULLPTR);
	~WSServer();

signals:
	void closed();

private slots:
	void onNewConnection();
	void processTextMessage(QString message);
	void respondToHS(QWebSocket *dest);
	void recvBanner(QByteArray message);
	void socketDisconnected();
//	void captureUpdated();
	void pushThumbnail(QWebSocket *dest);
	void pushThumbnails();

private:
	QWebSocketServer *m_pWebSocketServer;
	QList<QWebSocket *> clients;
	quint16 displayWidth, displayHeight;
	quint16 thumbWidth, thumbHeight;
	quint16 bannerX, bannerY;
	BannerCache bannerCache;
};

#endif //WSSERVER_H
