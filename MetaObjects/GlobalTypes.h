#ifndef GLOBALTYPES_H
#define GLOBALTYPES_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QColor>
#include <QMap>

#include "DBConnection.h"
#include "ProgramConfig.h"

enum FeaturteType {FeaturteBrick, FeaturteNonBrick};

enum PermitType {PermitNone, PermitRead, PermitWrite, PermitConfirm};

enum FeatureModelRole {
	ClearRecordsRole = Qt::UserRole + 1,
	SelectRecordsRole = Qt::UserRole + 2,
	SetIntervalDateRole = Qt::UserRole + 3,
	SetIntervalLengthRole = Qt::UserRole + 4};

struct UserRoleHandler
{
	int role;
	QString name;
	QVector<int> shops;
	QVector<int> feature_bricks_values;
	QVector<int> feature_values;
	PermitType summary;
	PermitType batches;
	PermitType control_charts;
};

struct BatchData
{
	int batch;
	int shop;
	QDateTime datetime;
	QString number;
	int substance;
	int state;
	QString note;
};

struct ShopData
{
	int shop;
	QString name;
};

struct LineData
{
	int line;
	int shop;
	QString name;
	int number;
};

struct SubstanceData
{
	int substance;
	int shop;
	QString name;
};

struct State
{
	enum States
	{
		Work = 1,
		Verify = 3,
		Ready = 5
	};

	int state;
	QString name;
	QColor color;
};

class Resources
{
public:
	static QHash<int, State> states;
	static DBConnection* db;
	static ProgramConfig* config;
	static UserRoleHandler role;
	static QHash<int, ShopData> shops;
	static QHash<int, LineData> lines;
	static QHash<int, SubstanceData> substances;
	static QMap<QString, QString> queries;
	static QHash<int, BatchData> batches;

	static void initUserRole();
	static void initShops();
	static void initLines();
	static void initSubstances();
	static void initQueries();
	static void initStates();
	static const BatchData getBatchData(const int& batch);
	static QString batchToolTip(const int& batch);
	static void cleanBatches(const QDateTime& begin, const QDateTime& end);

	static QVector<int> parseArray(const QString &arr);
	static QVector<double> parseDoubleArray(const QString &arr);
};

#endif // GLOBALTYPES_H
