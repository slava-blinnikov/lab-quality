#include <QtDebug>

#include "FeaturesTableModel.h"

FeaturesTableModel::FeaturesTableModel(QObject *parent) :
	QAbstractTableModel(parent)
{
	createFeatureHeaders();
}

bool FeaturesTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole)
	{
		int col = index.column();
		int row = index.row();

	}
	else if (role == ClearRecordsRole)
	{
		beginResetModel();
		m_records.clear();
		endResetModel();
	}
	else if (role == SelectRecordsRole)
	{
		beginResetModel();
		selectRecords();
		endResetModel();
	}
	else if (role == SetIntervalDateRole)
	{
		m_dt1 = value.toDateTime();
	}
	else if (role == SetIntervalLengthRole)
	{
		m_dt2 = m_dt1.addSecs(value.toLongLong());
	}
	return false;
}

int FeaturesTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED (parent)
	return m_records.size();
}

int FeaturesTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED (parent)
	return m_column_count;
}

QVariant FeaturesTableModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid())
	{
		if (role == Qt::DisplayRole)
		{
			int row = index.row();
			int col = index.column();
			FeatureTableRecord record = m_records.at(row);
			switch (col)
			{
				case 0: return record.shop_id >=0 ? QVariant(Resources::shops[record.shop_id].name) : QVariant(QVariant::String);
				case 1: return record.batch_id >= 0 ? QVariant(Resources::batches[record.batch_id].number) : QVariant(QVariant::String);
				case 2: return record.substance_id >= 0 ? QVariant(Resources::substances[record.substance_id].name) : QVariant(QVariant::String);
				case 3: return QVariant(Resources::lines[record.line_id].name);
				case 4: return QVariant(record.datetime.toString("dd.MM.yy hh:mm"));
				case 5: return record.brick_num >=0 ? QVariant(record.brick_num) : QVariant(QVariant::Int);
				default:
				{
					FeatureRecord fr = record.features.at(col - 6);
					return fr.state > 0 ? QVariant(QString::number(fr.value, 'f', 3)) : QVariant(QVariant::Double);
				}
			}
		}
		else if (role == Qt::EditRole)
		{
			int row = index.row();
			int col = index.column();
			FeatureTableRecord record = m_records.at(row);
			switch (col)
			{
				case 0: return QVariant(record.shop_id);
				case 1: return QVariant(record.batch_id);
				case 2: return QVariant(record.substance_id);
				case 3: return QVariant(record.line_id);
				case 4: return QVariant(record.datetime);
				case 5: return QVariant(record.brick_num);
				default: return QVariant(record.features.at(col - 6).value);
			}
		}
		else if (role == Qt::TextAlignmentRole)
		{
			return QVariant(int(Qt::AlignHCenter | Qt::AlignVCenter));
		}
	}
	return QVariant();
}

QVariant FeaturesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			switch (section)
			{
				case 0: return QVariant(tr("Цех"));
				case 1: return QVariant(tr("№ партии"));
				case 2: return QVariant(tr("Марка"));
				case 3: return QVariant(tr("Линия"));
				case 4: return QVariant(tr("Дата/Время"));
				case 5: return QVariant(tr("№ брикета"));
				default: return QVariant(m_feature_headers.at(section - 6).name);
			}
		}
	}
	return QVariant();
}

Qt::ItemFlags FeaturesTableModel::flags(const QModelIndex &index) const
{
	Q_UNUSED (index)
	Qt::ItemFlags flg = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
	return flg;
}

void FeaturesTableModel::createFeatureHeaders()
{
	m_records.clear();
	m_feature_headers.clear();
	QSqlQuery *qry = Resources::db->createQuery();
	DBConnection::QueryKiller killer(qry);
	if (qry)
	{
		if (qry->prepare(Resources::queries["get_feature_name"]))
		{
			int i, sz, feature;
			FeatureHeader hdr;
			sz = Resources::role.feature_bricks_values.size();
			for (i = 0; i < sz; i++)
			{
				feature = Resources::role.feature_bricks_values.at(i);
				qry->bindValue(":feature", QVariant(feature));
				if (Resources::db->exec(qry))
				{
					if (qry->next())
					{
						hdr.type = FeaturteBrick;
						hdr.id = feature;
						hdr.name = qry->value(0).toString();
						m_feature_headers.append(hdr);
					}
				}
			}

			sz = Resources::role.feature_values.size();
			for (i = 0; i < sz; i++)
			{
				feature = Resources::role.feature_values.at(i);
				qry->bindValue(":feature", QVariant(feature));
				if (Resources::db->exec(qry))
				{
					if (qry->next())
					{
						hdr.type = FeaturteNonBrick;
						hdr.id = feature;
						hdr.name = qry->value(0).toString();
						m_feature_headers.append(hdr);
					}
				}
			}
		}
	}
	m_column_count = m_feature_headers.size() + 6;
}

void FeaturesTableModel::selectRecords()
{
	m_records.clear();
	QSqlQuery *qry = Resources::db->createQuery();
	DBConnection::QueryKiller killer(qry);
	if (qry)
	{
		if (qry->prepare(Resources::queries["get_feature_values"]))
		{
			qry->bindValue(":dt_from", QVariant(m_dt1));
			qry->bindValue(":dt_to", QVariant(m_dt2));
			qry->bindValue(":role", QVariant(Resources::role.role));
			if (Resources::db->exec(qry))
			{
				while (qry->next())
				{
					FeatureTableRecord rec;
					rec.shop_id = qry->value(0).toInt();
					rec.line_id = qry->value(1).toInt();
					rec.datetime = qry->value(2).toDateTime();
					rec.batch_id = !qry->value(3).isNull() ? qry->value(3).toInt() : -1;
					rec.brick_num = !qry->value(4).isNull() ? qry->value(4).toInt() : -1;
					rec.substance_id = !qry->value(5).isNull() ? qry->value(5).toInt() : -1;
					QVector<double> values = Resources::parseDoubleArray(qry->value(6).toString());
					QVector<int> states = Resources::parseArray(qry->value(7).toString());
					if (values.size() == states.size())
					{
						for (int i = 0; i < values.size(); i++)
						{
							FeatureRecord fr;
							fr.value = values.at(i);
							fr.state = states.at(i);
							rec.features.append(fr);
						}
					}

					values = Resources::parseDoubleArray(qry->value(8).toString());
					states = Resources::parseArray(qry->value(9).toString());
					if (values.size() == states.size())
					{
						for (int i = 0; i < values.size(); i++)
						{
							FeatureRecord fr;
							fr.value = values.at(i);
							fr.state = states.at(i);
							rec.features.append(fr);
						}
					}
					m_records.append(rec);
				}
			}
		}
	}
}
