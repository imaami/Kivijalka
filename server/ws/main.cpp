#include <QtCore/QCoreApplication>
#include "wsserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    WSServer *server = new WSServer(8001, QString("/dev/shm/busstop/thumb.png"),
                                    QString("/dev/shm/busstop/banner.png"));
    QObject::connect(server, &WSServer::closed, &a, &QCoreApplication::quit);

    return a.exec();
}
