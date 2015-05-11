#include "wsserver.h"
#include "watcherthread.h"

#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

QT_USE_NAMESPACE

WSServer::WSServer(quint16 port,
                   quint16 displayWidth, quint16 displayHeight,
                   quint16 thumbWidth, quint16 thumbHeight,
                   const QString &thumbFile,
                   const QString &bannerFile,
                   const QString &bannerDir,
                   QObject *parent) :
	QObject(parent),
	m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Websocket Server"),
	                                        QWebSocketServer::NonSecureMode, this)),
	clients(),
	thumbnail(thumbFile),
	banner(bannerFile),
	bannerCache(bannerDir, this)
{
	this->displayWidth = displayWidth;
	this->displayHeight = displayHeight;
	this->thumbWidth = thumbWidth;
	this->thumbHeight = thumbHeight;

	if (m_pWebSocketServer->listen(QHostAddress::LocalHost, port)) {
		qDebug() << "Websocket server listening on port" << port;
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
		        this, &WSServer::onNewConnection);
		connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &WSServer::closed);

		if ((watcherThread = new WatcherThread(thumbFile, this))) {
			connect(watcherThread, &WatcherThread::fileUpdated, this, &WSServer::thumbnailUpdated);
			watcherThread->start();
		}
	}

}

WSServer::~WSServer()
{
	m_pWebSocketServer->close();
	qDeleteAll(clients.begin(), clients.end());
}

void WSServer::onNewConnection()
{
	QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
	connect(pSocket, &QWebSocket::textMessageReceived, this, &WSServer::processTextMessage);
	connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WSServer::recvBanner);
	connect(pSocket, &QWebSocket::disconnected, this, &WSServer::socketDisconnected);
	clients << pSocket;
	qDebug() << "New client:" << pSocket->peerAddress().toString();
}

void WSServer::respondToHS(QWebSocket *dest)
{
	dest->sendTextMessage(
		"{\n\t\"type\": \"c9f82f30e6a00db0\",\n\t\"ts\": "
		+ QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch())
		+ ",\n\t\"w\": "
		+ QString::number(displayWidth)
		+ ",\n\t\"h\": "
		+ QString::number(displayHeight)
		+ ",\n\t\"tw\": "
		+ QString::number(thumbWidth)
		+ ",\n\t\"th\": "
		+ QString::number(thumbHeight)
		+ "\n}"
	);
}

void WSServer::processTextMessage(QString message)
{
	qDebug() << "Received string:" << message;
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (pClient) {
		if (message == "71bf2d31e9e4e15c") {
			respondToHS(pClient);
		} else {
			pushThumbnail(pClient);
		}
	}
}

void WSServer::recvBanner(QByteArray message)
{
	qDebug() << "recvBanner: " + banner;
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (pClient) {
		if (!message.isEmpty()) {
			QFile file(banner);
			if (file.open(QIODevice::WriteOnly)) {
				qDebug() << "opened banner";
				qint64 bytes = file.write(message);
				file.close();
				qDebug() << "wrote" << bytes << "bytes";
			}
		}
	}
}

void WSServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        qDebug() << "Websocket client disconnected";
        clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void WSServer::pushThumbnail(QWebSocket *dest)
{
	if (imageData.isEmpty()) {
		qDebug() << "pushThumbnail: no image data";
	} else {
		dest->sendBinaryMessage(imageData);
	}
}

void WSServer::pushThumbnails()
{
	if (imageData.isEmpty()) {
		qDebug() << "pushThumbnails: no image data";
	} else {
		if (clients.size() > 0) {
			for (int i = 0; i < clients.size(); ++i) {
				clients.at(i)->sendBinaryMessage(imageData);
			}
			qDebug() << "pushThumbnails: Sent thumbnail to client(s)";
		} else {
			qDebug() << "pushThumbnails: no client(s)";
		}
	}
}


void WSServer::thumbnailUpdated()
{
	qDebug() << "Thumbnail updated";
	if (thumbnail.open(QIODevice::ReadOnly)) {
		if (!imageData.isEmpty()) {
			imageData.clear();
		}
		imageData = thumbnail.readAll();
		thumbnail.close();
		pushThumbnails();
	} else {
		qDebug() << "thumbnailUpdated: Failed to open" + thumbnail.fileName();
	}
}
