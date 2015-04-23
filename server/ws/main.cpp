#include <QtCore/QCoreApplication>
#include "wsserver.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	WSServer *server = new WSServer(8001, 1280, 1024, 640, 512,
	                                QString("/dev/shm/busstop/thumb.png"),
	                                QString("/dev/shm/busstop/banner.png"),
	                                QString("/usr/share/kivijalka/banners"));
	QObject::connect(server, &WSServer::closed, &a, &QCoreApplication::quit);

	return a.exec();
}
