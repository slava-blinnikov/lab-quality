#ifndef PROGRAMCONFIG_H
#define PROGRAMCONFIG_H

#include <QDomDocument>
#include <QDomElement>

class ProgramConfig
{
public:
	ProgramConfig(const QString &fn);
	bool read();
	bool save();
	QDomDocument *getDocument();
	static QString getCDATA(const QDomElement &tag);

protected:
	QString fileName;
	QDomDocument *document;
};

#endif // PROGRAMCONFIG_H
