#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	setWindowTitle(tr("Контроль качества. Сводная форма"));

	FeaturesTableModel *mdl = new FeaturesTableModel(this);
	mdl->setData(QModelIndex(), QVariant(QDateTime(QDate(2014,04,01), QTime(00,00,00,01))), SetIntervalDateRole);
	mdl->setData(QModelIndex(), QVariant(30*24*60*60), SetIntervalLengthRole);
	mdl->setData(QModelIndex(), QVariant(), SelectRecordsRole);
	QTableView *table = new QTableView(this);
	table->setModel(mdl);
	setCentralWidget(table);
}

void MainWindow::connectionLost()
{
}

void MainWindow::connectionRepaired()
{
}
