#ifndef BANNER_H
#define BANNER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QImage>

class Banner : public QObject
{
	Q_OBJECT
public:
	explicit Banner(const QString &imageFile,
	                QObject *parent = Q_NULLPTR);
	~Banner();

	QString metadata();
	QString file;
	QImage img;

signals:
	void placementChanged();

private:
	void constructMetadata();
	void setPlacement(quint16 x, quint16 y);
	struct { quint16 x, y; } placement;
	QString json;
};

#endif // BANNER_H
