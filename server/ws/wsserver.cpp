#include "wsserver.h"
#include "watcherthread.h"
#include "diskreader.h"
#include "imgthread.h"
#include "diskwriter.h"
#include "global.h"

#include <cstring>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

QT_USE_NAMESPACE

WSServer::WSServer(quint16 port,
                   quint16 displayWidth, quint16 displayHeight,
                   quint16 thumbWidth, quint16 thumbHeight,
                   quint16 bannerX, quint16 bannerY,
                   const QString &captureFile,
                   const QString &outputFile,
                   const QString &bannerDir,
                   QObject *parent) :
	QObject(parent),
	m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Websocket Server"),
	                                        QWebSocketServer::NonSecureMode, this)),
	clients(),
	bannerCache(bannerDir, this)
{
	global_init(bannerX, bannerY, thumbWidth, thumbHeight);

	(void) img_file_set_path (&capture_file, captureFile.toUtf8().data());
	(void) img_file_set_path (&output_file, outputFile.toUtf8().data());

	this->displayWidth = displayWidth;
	this->displayHeight = displayHeight;
	this->thumbWidth = thumbWidth;
	this->thumbHeight = thumbHeight;
	this->bannerX = bannerX;
	this->bannerY = bannerY;

	if (m_pWebSocketServer->listen(QHostAddress::LocalHost, port)) {
		qDebug() << "Websocket server listening on port" << port;
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
		        this, &WSServer::onNewConnection);
		connect(m_pWebSocketServer, &QWebSocketServer::closed,
		        this, &WSServer::closed);

		if ((watcherThread = new WatcherThread(this))
		    && (diskReader = new DiskReader(this))
		    && (imgThread = new ImgThread(this))
		    && (diskWriter = new DiskWriter(this))) {
			diskWriter->start();
			imgThread->start();
			diskReader->start();
			watcherThread->start();
		}
	}
}

WSServer::~WSServer()
{
	m_pWebSocketServer->close();
	qDeleteAll(clients.begin(), clients.end());
	global_fini();
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
	if (!pClient || message.isEmpty()) {
		return;
	}

	printf ("received banner\n");

	img_data_t *imd;

	if ((imd = img_data_new_from_buffer (message.size(),
	                                     (char *) message.constData()))) {
		img_file_replace_data (&banner_file, imd);
		if (sem_post (&process_sem)) {
			std::fprintf (stderr, "%s: sem_post failed: %s\n",
			              __func__, std::strerror (errno));
		}
		imd = NULL;
	}
/*
	if (img_load_data (&img, 1, message.constData(),
	                   message.size())) {
		if (img_render (&img, bannerX, bannerY)) {
			(void) img_write (&img, 0, output_file.path);
			if (img_scale (&img, 0, thumbWidth, thumbHeight)
			    && img_file_import_layer (&thumb_file, &img, 0)) {
				pushThumbnails();
			}
		} else {
			fprintf (stderr, "img_render failed\n");
		}
	} else {
		fprintf (stderr, "img_load_data failed\n");
	}
*/
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
	QByteArray thumbData(thumb_file.data->data);
	if (!thumbData.isEmpty()) {
		dest->sendBinaryMessage(thumbData);
	}
}

void WSServer::pushThumbnails()
{
	QByteArray thumbData(thumb_file.data->data);
	if (!thumbData.isEmpty()) {
		for (int i = 0; i < clients.size(); ++i) {
			clients.at(i)->sendBinaryMessage(thumbData);
		}
	}
}

/*
void WSServer::captureUpdated()
{
	printf ("capture file updated\n");
	if (img_load_file (&img, 0, capture_file.path)) {
		(void) img_render (&img, bannerX, bannerY);
		(void) img_write (&img, 0, output_file.path);
		if (img_scale (&img, 0, thumbWidth, thumbHeight)
		    && img_file_import_layer (&thumb_file, &img, 0)) {
			pushThumbnails();
		}
	} else {
		fprintf (stderr, "img_load_file failed\n");
	}
}
*/
