#ifndef RECONNECTINGTHREAD_H
#define RECONNECTINGTHREAD_H

#include <QThread>
#include <QSqlDatabase>

class ReconnectingThread : public QThread
{
	Q_OBJECT
public:
	ReconnectingThread(QSqlDatabase *db, unsigned long interval, int trys);

protected:
	void run();

private:
	unsigned long interval;
	int trys;
	QSqlDatabase *database;

signals:
	void success();
	void failure();
};

#endif // RECONNECTINGTHREAD_H
