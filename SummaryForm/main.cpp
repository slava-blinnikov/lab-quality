#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include <QTextCodec>
#include <QDebug>

#include "DBConnection.h"
#include "ProgramConfig.h"
#include "MainWindow.h"
#include "GlobalTypes.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	//QTranslator *qt_translator = new QTranslator;
	//if (qt_translator->load(":tr/qt_ru.qm")) a.installTranslator(qt_translator);

	QString cfg_file = a.applicationFilePath();
	#ifdef Q_OS_WIN32
		cfg_file.replace(".exe", ".xml");
	#else
		cfg_file += ".xml";
	#endif
	ProgramConfig conf(cfg_file);
	if (!conf.read()) return 0;
	Resources::config = &conf;

	try
	{
		Resources::db = new DBConnection();
	}
	catch(QString e)
	{
		QMessageBox::critical(0, a.applicationName(), e);
		return 0;
	}

	if (!Resources::db->login())
	{
		delete Resources::db;
		Resources::db = 0;
		return 0;
	}

	Resources::initStates();
	Resources::initShops();
	Resources::initLines();
	Resources::initSubstances();


	MainWindow w;
	QObject::connect(Resources::db, SIGNAL(connectionLost()), &w, SLOT(connectionLost()));
	QObject::connect(Resources::db, SIGNAL(connectionRepaired()), &w, SLOT(connectionRepaired()));
	w.setGeometry(400, 50, 1024, 700);
	w.showNormal();

	int ret = a.exec();
	if (Resources::db)
	{
		Resources::db->close();
		delete Resources::db;
		Resources::db = 0;
	}
	return ret;
}
