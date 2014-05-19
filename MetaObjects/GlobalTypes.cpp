#include "GlobalTypes.h"
#include <QDebug>
#include <QSqlQuery>

QHash<int, State> Resources::states;
DBConnection* Resources::db;
ProgramConfig* Resources::config;
UserRoleHandler Resources::role;
QHash<int, ShopData> Resources::shops;
QHash<int, LineData> Resources::lines;
QHash<int, SubstanceData> Resources::substances;
QMap<QString, QString> Resources::queries;
QHash<int, BatchData> Resources::batches;

void Resources::initUserRole()
{
	qDebug() << "Loading user role rights...";
	QSqlQuery *qry = db->createQuery();
	DBConnection::QueryKiller killer(qry);
	if (qry)
	{
		if (qry->prepare(queries["get_user_role"]))
		{
			qry->bindValue(":name", QVariant(db->getDatabase()->userName()));
			if (db->exec(qry))
			{
				if (qry->next()) role.role = qry->value(0).toInt();
			}
		}
		if (qry->prepare(queries["get_role_properies"]))
		{
			qry->bindValue(":role", QVariant(role.role));
			if (db->exec(qry))
			{
				if (qry->next())
				{
					role.name = qry->value(0).toString();
					role.shops = parseArray(qry->value(1).toString());
					role.feature_bricks_values = parseArray(qry->value(2).toString());
					role.feature_values= parseArray(qry->value(3).toString());
					QString perm;

					perm = qry->value(4).toString();
					if (perm == "read")
						role.summary = PermitRead;
					else if (perm == "write")
						role.summary = PermitWrite;
					else if (perm == "confirm")
						role.summary = PermitConfirm;
					else
						role.summary = PermitNone;

					perm = qry->value(5).toString();
					if (perm == "read")
						role.batches = PermitRead;
					else if (perm == "write")
						role.batches = PermitWrite;
					else if (perm == "confirm")
						role.batches = PermitConfirm;
					else
						role.batches = PermitNone;

					perm = qry->value(6).toString();
					if (perm == "read")
						role.control_charts = PermitRead;
					else if (perm == "write")
						role.control_charts = PermitWrite;
					else if (perm == "confirm")
						role.control_charts = PermitConfirm;
					else
						role.control_charts = PermitNone;
				}
			}
		}
	}
}

void Resources::initShops()
{
	qDebug() << "Loading shops...";
	QSqlQuery* q = db->createQuery();
	DBConnection::QueryKiller killer(q);
	if(q)
	{
		if(db->exec(q, queries["get_shops"]))
		{
			while(q->next())
			{
				ShopData s;
				s.shop = q->value(0).toInt();
				s.name = q->value(1).toString();
				if(role.shops.indexOf(s.shop) != -1)
				{
					shops[s.shop] = s;
					qDebug() << s.shop << "=" << s.name;
				}
				else
				{
					qDebug() << "Skip" << s.shop << "=" << s.name;
				}
			}
		}
	}
}

void Resources::initLines()
{
	qDebug() << "Loading lines...";
	QSqlQuery* q = db->createQuery();
	DBConnection::QueryKiller killer(q);
	if(q)
	{
		if(db->exec(q, queries["get_lines"]))
		{
			while(q->next())
			{
				LineData l;
				l.line = q->value(0).toInt();
				l.shop = q->value(1).toInt();
				l.name = q->value(2).toString();
				l.number = q->value(3).toInt();
				if(role.shops.indexOf(l.shop) != -1)
				{
					lines[l.line] = l;
					qDebug() << l.line << l.shop << l.name << l.number;
				}
				else
				{
					qDebug() << "Skip" << l.line << l.shop << l.name << l.number;
				}
			}
		}
	}
}

void Resources::initSubstances()
{
	qDebug() << "Loading substances...";
	QSqlQuery* q = db->createQuery();
	DBConnection::QueryKiller killer(q);
	if(q)
	{
		if(db->exec(q, queries["get_substances"]))
		{
			while(q->next())
			{
				SubstanceData s;
				s.substance = q->value(0).toInt();
				s.shop = q->value(1).toInt();
				s.name = q->value(2).toString();
				if(role.shops.indexOf(s.shop) != -1)
				{
					substances[s.substance] = s;
					qDebug() << s.substance << s.shop << s.name;
				}
				else
				{
					qDebug() << "Skip" << s.substance << s.shop << s.name;;
				}
			}
		}
	}
}

void Resources::initQueries()
{
	qDebug() << "Loading queries...";
	QDomElement root = config->getDocument()->documentElement();
	QDomElement tag_queries = root.firstChildElement("queries");
	if (!tag_queries.isNull())
	{
		QDomElement tag_qry = tag_queries.firstChildElement("query");
		while (!tag_qry.isNull())
		{
			queries[tag_qry.attribute("type")] = config->getCDATA(tag_qry);
			tag_qry = tag_qry.nextSiblingElement("query");
			qDebug() << "query" << tag_qry.attribute("type") << "loaded.";
		}
	}
}

void Resources::initStates()
{
	qDebug() << "Loading states...";
	QSqlQuery* q = db->createQuery();
	DBConnection::QueryKiller killer(q);
	if(q)
	{
		if(db->exec(q, queries["get_states"]))
		{
			while(q->next())
			{
				State s;
				s.state = q->value(0).toInt();
				s.name = q->value(1).toString();
				s.color.setNamedColor(q->value(2).toString());
				states[s.state] = s;
				qDebug() << s.state << s.name << s.color;
			}
		}
	}
}

const BatchData Resources::getBatchData(const int &batch)
{
	if(batches.contains(batch))
	{
		return batches[batch];
	}
	else
	{
		QSqlQuery* q = db->createQuery();
		DBConnection::QueryKiller killer(q);
		if(q)
		{
			q->prepare(queries["get_batch"]);
			q->bindValue(":batch", batch);
			if(db->exec(q))
			{
				//batch, shop, datetime, number, substance, state, note
				if(q->next())
				{
					BatchData b;
					b.batch = q->value(0).toInt();
					b.shop = q->value(1).toInt();
					b.datetime = q->value(2).toDateTime();
					b.number = q->value(3).toString();
					b.substance = q->value(4).toInt();
					b.state = q->value(5).toInt();
					b.note = q->value(6).toString();

					batches[batch] = b;
					return b;
				}
			}
		}
	}
	return BatchData();
}

QString Resources::batchToolTip(const int &batch)
{
	const BatchData& bd = getBatchData(batch);
	QString toolTip = "";
	toolTip += QObject::tr("<html> <body> <h2> Партия №%1 </h2> ").arg(bd.number);
	toolTip += QObject::tr("<p> Цех: %1 </p> ").arg(Resources::shops[bd.shop].name);
	toolTip += QObject::tr("<p> Дата и время создания: %1 </p> ").arg(bd.datetime.toString("dd.MM.yy hh:mm:ss"));
	toolTip += QObject::tr("<p> Марка: %1 </p> ").arg(Resources::substances[bd.substance].name);
	toolTip += QObject::tr("<p> Состояние: %1 </p> ").arg(Resources::states[bd.state].name);
	if(bd.note.length() > 0)
		toolTip += QObject::tr("<p> <span style=\"color:#ff0000;\"> Примечание: %1 </span></p> ").arg(bd.note);
	toolTip += "</body> </html> ";

	return toolTip;
}

void Resources::cleanBatches(const QDateTime &begin, const QDateTime &end)
{
	QVector<int> listForRemove;

	QHashIterator<int, BatchData> i(batches);
	while (i.hasNext())
	{
		i.next();
		QDateTime dt = i.value().datetime;
		if(begin.msecsTo(dt) < 0 || end.msecsTo(dt) > 0)
			listForRemove << i.key();
	}

	for(int i = 0; i < listForRemove.size(); i++)
		batches.remove(listForRemove.at(i));
}

QVector<int> Resources::parseArray(const QString &arr)
{
	QVector<int> ret;
	bool ok;
	int l = arr.length();
	if (l > 2)
	{
		QString s = arr.mid(1, l - 2);
		QStringList lst = s.split(',');
		for (int i = 0; i < lst.size(); i++)
		{
			l = lst.at(i).toInt(&ok);
			if (!ok) l = -1;
			ret.append(l);
		}
	}
	return ret;
}

QVector<double> Resources::parseDoubleArray(const QString &arr)
{
	QVector<double> ret;
	int l = arr.length();
	if (l > 2)
	{
		QString s = arr.mid(1, l - 2);
		QStringList lst = s.split(',');
		for (int i = 0; i < lst.size(); i++)
			ret.append(lst.at(i).toDouble());
	}
	return ret;
}
