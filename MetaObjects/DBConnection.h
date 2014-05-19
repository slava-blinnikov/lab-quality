#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QComboBox>
#include <QSqlError>

#include "ReconnectingThread.h"
#include "ProgramConfig.h"

class DBConnection : public QObject
{
	Q_OBJECT

public:
    class QueryKiller
    {
    public:
        QueryKiller(QSqlQuery* query)
            :m_query(query)
        {}

        ~QueryKiller()
        {
            if(m_query)
                delete m_query;
        }

    private:
        QSqlQuery* m_query;
    };

	enum LogonDialogType {NoLogon, LogonDefault, LogonAuto, LogonList, LogonDBList};

    DBConnection();
	bool login();
	void close();
	bool exec(QSqlQuery *query);
	bool exec(QSqlQuery *query, const QString &sql);
	QSqlQuery *createQuery();
	QSqlDatabase *getDatabase();
	QSqlDatabase *getListenDatabase();
	int getPID();
	void reconnect(const QString &userName, const QString &password);

private:
	unsigned long reconnInterval;
	int reconnTrys;
	ReconnectingThread *reconnectingThread;
	QSqlDatabase database;
	QSqlDatabase dbListen;
	int connectionID;
	bool createListenConnection();
	void reconnect();
	void initPID();

private slots:
	void reconnectSuccess();
	void reconnectFailure();

signals:
	void connectionLost();
	void connectionFailure();
	void connectionRepaired();
};

#endif // DBCONNECTION_H
