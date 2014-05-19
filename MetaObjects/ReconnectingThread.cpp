#include "ReconnectingThread.h"

ReconnectingThread::ReconnectingThread(QSqlDatabase *db, unsigned long interval, int trys)
{
	database = db;
	this->interval = interval;
	this->trys = trys;
}

void ReconnectingThread::run()
{
	if (database->isOpen()) database->close();
	if (trys > 0)
	{
		for (int count = 0; count < trys; count++)
		{
			if (database->open())
			{
				emit success();
				return;
			}
			msleep(interval);
		}
		emit failure();
	}
	else
	{
		while (true)
		{
			if (database->open())
			{
				emit success();
				return;
			}
			msleep(interval);
		}
	}
}
