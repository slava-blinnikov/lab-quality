#ifndef FEATURESTABLEMODEL_H
#define FEATURESTABLEMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>

#include "GlobalTypes.h"

class FeaturesTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	struct FeatureHeader
	{
		FeaturteType type;
		int id;
		QString name;
	};
	struct FeatureRecord
	{
		int state;
		double value;
	};
	struct FeatureTableRecord
	{
		int shop_id;
		int batch_id;
		int substance_id;
		int line_id;
		QDateTime datetime;
		int brick_num;
		QVector<FeatureRecord> features;
	};

	FeaturesTableModel(QObject *parent = 0);

	bool setData(const QModelIndex &index, const QVariant &value, int role);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

private:
	int m_column_count;
	QVector<FeatureHeader> m_feature_headers;
	QVector<FeatureTableRecord> m_records;
	QDateTime m_dt1;
	QDateTime m_dt2;

	void createFeatureHeaders();
	void selectRecords();
};

#endif // FEATURESTABLEMODEL_H
