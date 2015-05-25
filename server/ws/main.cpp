#include <QtCore/QCoreApplication>
#include "wsserver.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	WSServer *server = new WSServer(8001, 1280, 1024, 640, 512, 1024, 768,
	                                QString("/dev/shm/kivijalka/cap-0510.png"),
	                                QString("/dev/shm/kivijalka/out-0510.png"),
	                                QString("/usr/share/kivijalka/banners"));
	QObject::connect(server, &WSServer::closed, &a, &QCoreApplication::quit);

	return a.exec();
}
