#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>

#include "ProgramConfig.h"

ProgramConfig::ProgramConfig(const QString &fn)
{
	fileName = fn;
	document = 0;
}

bool ProgramConfig::read()
{
	if (document)
	{
		delete document;
		document = 0;
	}
	QFile file(fileName);
	QString errorStr;
	int errorLine;
	int errorColumn;
	document = new QDomDocument();
	if (!document->setContent(&file, true, &errorStr, &errorLine, &errorColumn))
	{
		delete document;
		document = 0;
		QMessageBox::critical(0, qApp->applicationName(), QObject::tr("Ошибка разбора файла-конфигурации!\n%1\nстрока: %2\nстолбец: %3")
							 .arg(errorStr)
							 .arg(errorLine)
							 .arg(errorColumn));
		return false;
	}
	return true;
}

bool ProgramConfig::save()
{
	if (document)
	{
		QFile file(fileName);
		file.open(QIODevice::WriteOnly);
		QTextStream out(&file);
		document->save(out, 4);
		return true;
	}
	return false;
}

QDomDocument *ProgramConfig::getDocument()
{
	return document;
}

QString ProgramConfig::getCDATA(const QDomElement &tag)
{
	QDomNode snode;
	QDomCDATASection cdata;
	snode = tag.firstChild();
	if (!snode.isNull())
	{
		if (snode.isCDATASection())
		{
			cdata = snode.toCDATASection();
			if (!cdata.isNull()) return cdata.data();
		}
	}
	return QString();
}
