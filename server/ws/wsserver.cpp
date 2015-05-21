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
                   quint16 bannerX, quint16 bannerY,
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
	thumbData(),
	bannerCache(bannerDir, this)
{
	this->displayWidth = displayWidth;
	this->displayHeight = displayHeight;
	this->thumbWidth = thumbWidth;
	this->thumbHeight = thumbHeight;
	this->bannerX = bannerX;
	this->bannerY = bannerY;

	(void) img_init (&img);

	if (m_pWebSocketServer->listen(QHostAddress::LocalHost, port)) {
		qDebug() << "Websocket server listening on port" << port;
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
		        this, &WSServer::onNewConnection);
		connect(m_pWebSocketServer, &QWebSocketServer::closed,
		        this, &WSServer::closed);

		if ((watcherThread = new WatcherThread(thumbFile, this))) {
			connect(watcherThread, &WatcherThread::capFileUpdated,
			        this, &WSServer::captureUpdated);
			connect(watcherThread, &WatcherThread::fileUpdated,
			        this, &WSServer::thumbnailUpdated);
			watcherThread->start();
		}
	}
}

WSServer::~WSServer()
{
	m_pWebSocketServer->close();
	img_destroy (&img);
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
			pushThumbnail(pClient);
		}
	}
}

void WSServer::recvBanner(QByteArray message)
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (pClient) {
		if (!message.isEmpty()) {
			if (img_load_banner (&img, (const uint8_t *) message.constData(),
			                      message.size())) {
				(void) img_render_thumb (&img,
				                         bannerX, bannerY,
				                         thumbWidth, thumbHeight);
			} else {
				fprintf (stderr, "img_load_banner failed\n");
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
	if (!thumbData.isEmpty()) {
		dest->sendBinaryMessage(thumbData);
	}
}

void WSServer::pushThumbnails()
{
	if (!thumbData.isEmpty()) {
		for (int i = 0; i < clients.size(); ++i) {
			clients.at(i)->sendBinaryMessage(thumbData);
		}
	}
}


bool WSServer::readThumbnail()
{
	bool r;
	if ((r = thumbnail.open(QIODevice::ReadOnly))) {
		if (!thumbData.isEmpty()) {
			thumbData.clear();
		}
		thumbData = thumbnail.readAll();
		thumbnail.close();
	}
	return r;
}

void WSServer::thumbnailUpdated()
{
	if (readThumbnail()) {
		pushThumbnails();
	} else {
		qDebug() << "thumbnailUpdated: Failed to open" + thumbnail.fileName();
	}
}

void WSServer::captureUpdated()
{
	if (!img_load_screen (&img, "/dev/shm/busstop/cap-0510.png")) {
		fprintf (stderr, "img_load_screen failed\n");
	}
}
