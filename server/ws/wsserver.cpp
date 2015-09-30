#include "wsserver.h"
#include "global.h"
#include "display.h"
#include "banner.h"
#include "img.h"
#include "packet.h"

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <climits>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

QT_USE_NAMESPACE

WSServer::WSServer(const QString &addr, quint16 port,
                   quint16 captureWidth, quint16 captureHeight,
                   quint16 displayWidth, quint16 displayHeight,
                   quint16 thumbWidth, quint16 thumbHeight,
                   cache_t *cache,
                   QObject *parent) :
	QObject(parent),
	m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Websocket Server"),
	                                        QWebSocketServer::NonSecureMode, this)),
	clients(),
	serverAddr(addr),
	thumbNail()
{
	this->serverPort = port;
	this->captureWidth = captureWidth;
	this->captureHeight = captureHeight;
	this->displayWidth = displayWidth;
	this->displayHeight = displayHeight;
	this->thumbWidth = thumbWidth;
	this->thumbHeight = thumbHeight;
	thumbnail = NULL;
	this->cache = cache;
}

WSServer::~WSServer()
{
	m_pWebSocketServer->close();
	qDeleteAll(clients.begin(), clients.end());
	if (thumbnail) {
		img_data_free (thumbnail);
		thumbnail = NULL;
	}
	cache = NULL;
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
//	printf ("WSServer::%s: triggered\n", __func__);
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
	connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WSServer::recvBinary);
	connect(pSocket, &QWebSocket::disconnected, this, &WSServer::socketDisconnected);
	clients << pSocket;
	qDebug() << "New client:" << pSocket->peerAddress().toString();
}

void WSServer::respondToHS(QWebSocket *dest)
{
	dest->sendTextMessage(
		"{\n\t\"type\": \"c9f82f30e6a00db0\",\n\t\"ts\": "
		+ QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch())
		+ ",\n\t\"cw\": "
		+ QString::number(captureWidth)
		+ ",\n\t\"ch\": "
		+ QString::number(captureHeight)
		+ ",\n\t\"dw\": "
		+ QString::number(displayWidth)
		+ ",\n\t\"dh\": "
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
			uint8_t *info;
			size_t size;
			if (cache_info_packet (cache, &info, &size)) {
				pushCacheInfo (pClient, info, size);
				free (info);
				info = NULL;
				size = 0;
			}
			if (!thumbNail.isEmpty()) {
				pushThumbnail (pClient);
			}
		}
	}
}

void WSServer::recvBinary(QByteArray msg)
{
	QWebSocket *src = qobject_cast<QWebSocket *>(sender());

	if (!src || msg.isEmpty()) {
		return;
	}

	std::printf ("WSServer::%s: received message from %s\n", __func__,
	             src->peerAddress().toString().toUtf8().data());

	int size = msg.size();
	const char *data = msg.constData();
	std::uint32_t flags = packet_inspect (&data, &size);

	union {
		banner_packet_t  *b;
		banner_modpkt_t  *bm;
		display_modpkt_t *dm;
	} p;

	union {
		char       *c;
		const char *cc;
	} rep;
	std::size_t rsiz;

	switch (flags) {
	case PACKET_ADD|PACKET_DISPLAY|PACKET_BANNER|PACKET_IMG|PACKET_DATA:
		if (!(p.b = banner_packet_inspect (data, size))) {
			goto _malformed_packet;
		}

		if (!cache_import_packet (cache, p.b)) {
			std::fprintf (stderr, "%s: import error\n",
			              __func__);
		}

		break;

	case PACKET_MOD|PACKET_BANNER:
		if (!(p.bm = banner_modpkt_inspect (data, size))) {
			goto _malformed_packet;
		}

		if (!cache_apply_modpkt (cache, p.bm, &rep.c, &rsiz)) {
			std::fprintf (stderr,
			              "%s: cache_apply_modpkt failed\n",
			              __func__);
		} else {
			if (rsiz <= INT_MAX) {
				// broadcast banner modifications to clients
				size = (int) rsiz;
				QByteArray ba = QByteArray::fromRawData (rep.cc,
				                                         size);

				for (int i = 0; i < clients.size(); ++i) {
					clients.at(i)->sendBinaryMessage (ba);
				}

				ba = QByteArray();
			}

			std::free (rep.c);
		}

		break;

	case PACKET_MOD|PACKET_DISPLAY:
		if (!(p.dm = display_modpkt_inspect (data, size))) {
			goto _malformed_packet;
		}

		if (!cache_apply_display_modpkt (cache, p.dm, &rep.c, &rsiz)) {
			std::fprintf (stderr,
			              "%s: cache_apply_display_modpkt failed\n",
			              __func__);
		} else {
			if (rsiz <= INT_MAX) {
				// broadcast display modifications to clients
				size = (int) rsiz;
				QByteArray ba = QByteArray::fromRawData (rep.cc,
				                                         size);

				for (int i = 0; i < clients.size(); ++i) {
					clients.at(i)->sendBinaryMessage (ba);
				}

				ba = QByteArray();
			}

			std::free (rep.c);
		}

		break;

	default:
	_malformed_packet:
		std::fprintf (stderr, "%s: malformed packet\n",
		              __func__);
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
		thumbNail.resize (4 + (int) imd->size);
		((uint32_t *) thumbNail.data())[0] = PACKET_ADD|PACKET_DISPLAY|PACKET_DATA;
		memcpy ((void *) (thumbNail.data() + 4),
		        (const void *) imd->data, imd->size);
		imd = NULL;
//		printf ("WSServer::%s: have new thumbnail\n", __func__);
		return true;
	}
	return false;
}

void WSServer::pushCacheInfo(QWebSocket *dest,
                             uint8_t    *info,
                             size_t      size)
{
	QByteArray cacheInfo;
	cacheInfo.resize (4 + size);
	((uint32_t *) cacheInfo.data())[0] = PACKET_ADD|PACKET_DISPLAY|PACKET_BANNER|PACKET_IMG;
	memcpy ((void *) (cacheInfo.data() + 4), (const void *) info, size);
	dest->sendBinaryMessage (cacheInfo);
}

void WSServer::pushThumbnail(QWebSocket *dest)
{
	dest->sendBinaryMessage (thumbNail);
//	printf ("WSServer::%s: sent thumbnail to %s\n", __func__,
//	        dest->peerAddress().toString().toUtf8().data());
}

void WSServer::pushThumbnails()
{
	for (int i = 0; i < clients.size(); ++i) {
		pushThumbnail (clients.at(i));
	}
}
