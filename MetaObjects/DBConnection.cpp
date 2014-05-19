#include <QtDebug>

#include <QApplication>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlDriver>

#include "DBConnection.h"
#include "DBConnectionDialog.h"

#include "GlobalTypes.h"


DBConnection::DBConnection()
{
	connectionID = -1;
	reconnectingThread = 0;

    QDomDocument *doc = Resources::config->getDocument();
	QDomNodeList lst = doc->elementsByTagName("database");
	if (lst.isEmpty()) throw(tr("Отсутствуют настройки базы данных в файле-конфигурации!"));

	QDomElement db_tag = lst.at(0).toElement();

	QString db_name = db_tag.attribute("name");
	if (db_name.isEmpty()) throw(tr("Не указано имя базы данных!"));

	QString host = db_tag.attribute("host");
	if (host.isEmpty()) throw(tr("Не указан сетевой адрес базы данных!"));

	int port;
	QString s;
	bool ok;

	s = db_tag.attribute("port");
	if (s.isEmpty()) throw(tr("Не указан сетевой порт базы данных!"));
	port = s.toInt(&ok);
	if (!ok || port < 0) throw(tr("Некорректный параметр сетевого порта базы данных!"));

	s = db_tag.attribute("reconnecting_interval");
	if (!s.isEmpty())
	{
		reconnInterval = s.toULong(&ok);
		if (!ok) throw(tr("Некорректный параметр периода переподключения к базе данных!"));
	}
	else reconnInterval = 60000;

	s = db_tag.attribute("reconnecting_trys");
	if (!s.isEmpty())
	{
		reconnTrys = s.toInt(&ok);
		if (!ok || reconnTrys < 0) throw(tr("Некорректный параметр количества попыток переподключения к базе данных!"));
	}
	else reconnTrys = 0;

    Resources::initQueries();

	database = QSqlDatabase::addDatabase("QPSQL", db_name);
	database.setDatabaseName(db_name);
	database.setHostName(host);
	database.setPort(port);
	dbListen = QSqlDatabase::addDatabase("QPSQL", db_name + "_listen");
	dbListen.setDatabaseName(db_name);
	dbListen.setHostName(host);
	dbListen.setPort(port);
}

void DBConnection::initPID()
{
	connectionID = -1;
	QSqlQuery *qry = createQuery();
    QueryKiller killer(qry);
	if (qry)
	{
		if (exec(qry, "select pg_backend_pid()"))
			if (qry->next()) connectionID = qry->value(0).toInt();
	}
}

int DBConnection::getPID()
{
	return connectionID;
}

QSqlDatabase *DBConnection::getDatabase()
{
	if (reconnectingThread) return 0;
	return &database;
}

bool DBConnection::createListenConnection()
{
	dbListen.setUserName(database.userName());
	dbListen.setPassword(database.password());
	return dbListen.open();
}

QSqlDatabase *DBConnection::getListenDatabase()
{
	if (reconnectingThread) return 0;
	return &dbListen;
}


bool DBConnection::login()
{
	QString s;
	int dlg_type = -1;
	bool store_session = false;
	bool ret;
	QString last_user;
	QString db_group;
	QString db_user;
	QString db_password;
	QStringList *user_list = 0;

    QDomDocument *doc = Resources::config->getDocument();
	QDomNodeList lst = doc->elementsByTagName("logon");
	if (lst.isEmpty())
	{
		DBConnectionDialog dlg(0);
		dlg.setDatabase(&database);
		dlg.show();
		ret = dlg.exec() == QDialog::Accepted;
		if (ret)
		{
			initPID();
			createListenConnection();
            Resources::initUserRole();
		}
		return ret;
	}
	else
	{
		QDomElement logon_tag = lst.at(0).toElement();

		s = logon_tag.attribute("type");
		if (s.isEmpty() || s == "default") dlg_type = LogonDefault;
			else if (s == "auto") dlg_type = LogonAuto;
				else if (s == "list") dlg_type = LogonList;
					else if (s == "db_list") dlg_type = LogonDBList;
						else if (s == "no_logon") dlg_type = NoLogon;
		s = logon_tag.attribute("store_session");
		store_session = s == "true";

		if (store_session)
			last_user = logon_tag.attribute("last_user");

		db_group = logon_tag.attribute("group");
		db_user = logon_tag.attribute("user");
		db_password = logon_tag.attribute("password");

		if (dlg_type == LogonAuto && (db_user.isEmpty() || db_password.isEmpty()))
		{
			QMessageBox::critical(0, qApp->applicationName(), tr("Для автоматического подключения к базе данных в файле конфигурации должны быть указаны имя пользователя и пароль!"));
			return false;
		}
		if (dlg_type == LogonDBList && (db_user.isEmpty() || db_password.isEmpty()))
		{
			QMessageBox::critical(0, qApp->applicationName(), tr("Для получения списка пользователей из базы данных в файле конфигурации должны быть указаны имя пользователя, пароль и группа!"));
			return false;
		}
		if (dlg_type == LogonList || dlg_type == LogonDBList)
		{
			user_list = new QStringList();
			if (dlg_type == LogonList)
			{
				QDomElement user_tag = logon_tag.firstChildElement("user");
				while (!user_tag.isNull())
				{
					s = user_tag.attribute("name");
					if (!s.isEmpty()) user_list->append(s);
					user_tag = user_tag.nextSiblingElement("user");
				}
			}
			else
			{
				database.setUserName(db_user);
				database.setPassword(db_password);
				if (!database.open())
				{
					delete user_list;
					QMessageBox::critical(0, qApp->applicationName(), tr("Не удалось подключиться к базе данных для получения списка пользователей указанной группы!"));
					return false;
				}
				QSqlQuery *qry = new QSqlQuery(database);
                QueryKiller killer(qry);
				qry->prepare("select pg_user.usename AS name from pg_user where pg_user.usesysid = ANY(ARRAY[(select pg_group.grolist from pg_group where pg_group.groname= :grp )])");
				qry->bindValue(":grp", QVariant(db_group));
				if (!qry->exec())
				{
					delete user_list;
					database.close();
					QMessageBox::critical(0, qApp->applicationName(), tr("Ошибка при выполнении запроса для формирования списка пользователей!"));
					return false;
				}
				while (qry->next())
					user_list->append(qry->value(0).toString());
				database.close();
			}
		}
	}
	if (dlg_type == LogonAuto)
	{
		database.setUserName(db_user);
		database.setPassword(db_password);
		if (!database.open())
		{
			QString err = database.lastError().text();
			if (err.indexOf("password authentication failed") >= 0)
				QMessageBox::critical(0, qApp->applicationName(), tr("Ошибка автоматического подключения к базе данных!\nНеверно указаны имя пользователя или пароль."));
			else
				QMessageBox::critical(0, qApp->applicationName(), tr("Не могу подключиться к базе данных!\nВозможно, не запущен сервер базы данных."));
			return false;
		}
		initPID();
		createListenConnection();
        Resources::initUserRole();
		return true;
	}
	else
	{
		DBConnectionDialog logon_dlg;
		logon_dlg.setDatabase(&database);
		if ((dlg_type == LogonList || dlg_type == LogonDBList) && (user_list != 0))
			logon_dlg.setUserList(user_list);
		if (store_session && !last_user.isEmpty())
			logon_dlg.setLastUser(last_user);
		logon_dlg.show();
		logon_dlg.exec();
		int r = logon_dlg.result();
		if (r == QDialog::Accepted)
		{
			if (store_session && !lst.isEmpty())
			{
				logon_dlg.getPassword();
				QDomElement logon_tag = lst.at(0).toElement();
				logon_tag.setAttribute("last_user", logon_dlg.getUserName());
                Resources::config->save();
			}
			initPID();
			createListenConnection();
            Resources::initUserRole();
		}
		return r == QDialog::Accepted;
	}
}

void DBConnection::close()
{
	database.close();
	dbListen.close();
	connectionID = -1;
}

void DBConnection::reconnect(const QString &userName, const QString &password)
{
	database.close();
	dbListen.close();
	database.setUserName(userName);
	database.setPassword(password);
	dbListen.setUserName(userName);
	dbListen.setPassword(password);
	database.open();
	dbListen.open();
	initPID();
	createListenConnection();
    Resources::initUserRole();
}

void DBConnection::reconnect()
{
	if (reconnectingThread) return;
	connectionID = -1;
	emit connectionLost();
	database.close();
	dbListen.close();
	reconnectingThread = new ReconnectingThread(&database, reconnInterval, reconnTrys);
	connect(reconnectingThread, SIGNAL(success()), this, SLOT(reconnectSuccess()));
	connect(reconnectingThread, SIGNAL(failure()), this, SLOT(reconnectFailure()));
	reconnectingThread->start();
}

bool DBConnection::exec(QSqlQuery *query)
{
	if (query->exec()) return true;
	QSqlError::ErrorType errType = query->lastError().type();
	if (errType == 0 || errType == 1) reconnect();
	return false;
}

bool DBConnection::exec(QSqlQuery *query, const QString &sql)
{
	if (query->exec(sql)) return true;
	QSqlError::ErrorType errType = query->lastError().type();
	if (errType == 0 || errType == 1) reconnect();
	return false;
}

void DBConnection::reconnectSuccess()
{
	reconnectingThread->wait();
	delete reconnectingThread;
	reconnectingThread = 0;
	initPID();
	createListenConnection();
    Resources::initUserRole();
	emit connectionRepaired();
}

void DBConnection::reconnectFailure()
{
	reconnectingThread->wait();
	delete reconnectingThread;
	reconnectingThread = 0;
	emit connectionFailure();
}

QSqlQuery *DBConnection::createQuery()
{
	if (reconnectingThread) return 0;
	QSqlQuery *ret = new QSqlQuery(database);
	return ret;
}
