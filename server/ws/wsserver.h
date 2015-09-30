#ifndef WSSERVER_H
#define WSSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtNetwork/QHostAddress>

#include "img_data.h"
#include "cache.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WSServer : public QObject
{
	Q_OBJECT
public:
	explicit WSServer(const QString &addr, quint16 port,
	                  quint16 captureWidth, quint16 captureHeight,
	                  quint16 displayWidth, quint16 displayHeight,
	                  quint16 thumbWidth, quint16 thumbHeight,
	                  cache_t *cache,
	                  QObject *parent = Q_NULLPTR);
	~WSServer();
	bool listen();

signals:
	void closed();

public slots:
	void thumbnailUpdated();

private slots:
	void onNewConnection();
	void processTextMessage(QString message);
	void respondToHS(QWebSocket *dest);
	void recvBinary(QByteArray msg);

	void socketDisconnected();

private:
	bool tryUpdateThumbnail(img_data_t **old);
	void pushCacheInfo(QWebSocket *dest,
	                   uint8_t    *info,
	                   size_t      size);
	void pushThumbnail(QWebSocket *dest);
	void pushThumbnails();

	QWebSocketServer *m_pWebSocketServer;
	QList<QWebSocket *> clients;
	QHostAddress serverAddr;
	quint16 serverPort;
	quint16 captureWidth, captureHeight;
	quint16 displayWidth, displayHeight;
	quint16 thumbWidth, thumbHeight;
	QByteArray thumbNail;
	img_data_t *thumbnail;
	cache_t *cache;
};

#endif //WSSERVER_H
