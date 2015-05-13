#ifndef WSSERVER_H
#define WSSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include "bannercache.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(WatcherThread)

class WSServer : public QObject
{
	Q_OBJECT
public:
	explicit WSServer(quint16 port,
	                  quint16 displayWidth, quint16 displayHeight,
	                  quint16 thumbWidth, quint16 thumbHeight,
	                  const QString &thumbFile,
	                  const QString &bannerFile,
	                  const QString &bannerDir,
	                  QObject *parent = Q_NULLPTR);
	~WSServer();

Q_SIGNALS:
	void closed();

private Q_SLOTS:
	void onNewConnection();
	void processTextMessage(QString message);
	void respondToHS(QWebSocket *dest);
	void recvBanner(QByteArray message);
	void socketDisconnected();
	bool readThumbnail();
	void thumbnailUpdated();
	void pushThumbnail(QWebSocket *dest);
	void pushThumbnails();

private:
	QWebSocketServer *m_pWebSocketServer;
	QList<QWebSocket *> clients;
	quint16 displayWidth, displayHeight;
	quint16 thumbWidth, thumbHeight;
	QFile thumbnail;
	QString banner;
	QByteArray imageData;
	WatcherThread *watcherThread;
	BannerCache bannerCache;
};

#endif //WSSERVER_H
