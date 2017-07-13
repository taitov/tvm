#ifndef CDATA_H
#define CDATA_H

#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtWidgets/QWidget>

#include <tvm/type.h>

namespace nVirtualMachine
{

namespace nGui
{

class cData
{
public:
	cData();

public:
	bool makeWidget(const QString& typeName);
	QJsonObject save() const;
	void restore(const QJsonValue& jsonValue);

private:
	bool makeWidgetElement(const QString& typeName, QWidget*& widget);
	QJsonObject saveWidgetElement(const QString& typeName, QWidget* widget) const;
	bool restoreWidgetElement(const QString& typeName, QWidget* widget, const QJsonObject& jsonObject);

public:
	tModuleTypeName moduleTypeName;
	QString filePath;
	QWidget* mainWidget;
	QString typeName;
};

}

}

#endif // CDATA_H
