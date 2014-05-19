#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>

#include "FeaturesTableModel.h"
#include "GlobalTypes.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);

public slots:
	void connectionLost();
	void connectionRepaired();
};

#endif // MAINWINDOW_H
