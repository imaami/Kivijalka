#include "wsserver.h"
#include "global.h"

#include <cstring>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

QT_USE_NAMESPACE

WSServer::WSServer(const QString &addr, quint16 port,
                   quint16 displayWidth, quint16 displayHeight,
                   quint16 thumbWidth, quint16 thumbHeight,
                   quint16 bannerX, quint16 bannerY,
                   QObject *parent) :
	QObject(parent),
	m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Websocket Server"),
	                                        QWebSocketServer::NonSecureMode, this)),
	clients(),
	serverAddr(addr),
	thumbNail()
{
	this->serverPort = port;
	this->displayWidth = displayWidth;
	this->displayHeight = displayHeight;
	this->thumbWidth = thumbWidth;
	this->thumbHeight = thumbHeight;
	this->bannerX = bannerX;
	this->bannerY = bannerY;
	thumbnail = NULL;
}

WSServer::~WSServer()
{
	m_pWebSocketServer->close();
	qDeleteAll(clients.begin(), clients.end());
	if (thumbnail) {
		img_data_free (thumbnail);
		thumbnail = NULL;
	}
}

bool WSServer::listen()
{
	if (!m_pWebSocketServer) {
		std::fprintf (stderr, "WSServer::%s: null server object\n",
		              __func__);
	} else if (!m_pWebSocketServer->listen (serverAddr, serverPort)) {
		std::fprintf (stderr, "WSServer::%s: failed to start server\n",
		              __func__);
	} else {
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
		        this, &WSServer::onNewConnection);
		connect(m_pWebSocketServer, &QWebSocketServer::closed,
		        this, &WSServer::closed);
		std::printf ("WSServer::%s: running on %u\n", __func__, serverPort);
		return true;
	}
	return false;
}

void WSServer::thumbnailUpdated()
{
	printf ("WSServer::%s: triggered\n", __func__);
	img_data_t *old;
	if (tryUpdateThumbnail (&old)) {
		if (!thumbNail.isEmpty()) {
			pushThumbnails();
		}
		if (old) {
			img_data_free (old);
			old = NULL;
		}
	}
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
			if (!thumbNail.isEmpty()) {
				pushThumbnail (pClient);
			}
		}
	}
}

void WSServer::recvBanner(QByteArray message)
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (!pClient || message.isEmpty()) {
		return;
	}

	printf ("WSServer::%s: received banner from %s\n", __func__,
	        pClient->peerAddress().toString().toUtf8().data());

	img_data_t *imd;

	if ((imd = img_data_new_from_buffer ((size_t) message.size(),
	                                     message.constData()))) {
		img_file_replace_data (&banner_file, imd);
		if (sem_post (&process_sem)) {
			std::fprintf (stderr, "%s: sem_post failed: %s\n",
			              __func__, std::strerror (errno));
		}
		imd = NULL;
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

bool WSServer::tryUpdateThumbnail(img_data_t **old)
{
	img_data_t *imd = img_file_steal_data (&thumb_file);
	if (imd) {
		*old = thumbnail;
		thumbnail = imd;
		thumbNail = QByteArray::fromRawData ((const char *) imd->data,
		                                     (int) imd->size);
		imd = NULL;
		printf ("WSServer::%s: have new thumbnail\n", __func__);
		return true;
	}
	return false;
}

void WSServer::pushThumbnail(QWebSocket *dest)
{
	dest->sendBinaryMessage (thumbNail);
	printf ("WSServer::%s: sent thumbnail to %s\n", __func__,
	        dest->peerAddress().toString().toUtf8().data());
}

void WSServer::pushThumbnails()
{
	for (int i = 0; i < clients.size(); ++i) {
		pushThumbnail (clients.at(i));
	}
}
