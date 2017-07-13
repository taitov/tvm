#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>

#include "data.h"

using namespace nVirtualMachine::nGui;

cData::cData()
{
	mainWidget = nullptr;
}

bool cData::makeWidget(const QString& typeName)
{
	this->typeName = typeName;
	makeWidgetElement(typeName, mainWidget);
	return true;
}

QJsonObject cData::save() const
{
	QJsonObject jsonObject;
	jsonObject[typeName] = saveWidgetElement(typeName, mainWidget);
	return jsonObject;
}

void cData::restore(const QJsonValue& jsonValue)
{
	restoreWidgetElement(typeName, mainWidget, jsonValue.toObject()[typeName].toObject());
}

class QTextEditSizeHint : public QTextEdit
{
public:
	QSize sizeHint() const override
	{
		return QSize(128, 16);
	}
};

bool cData::makeWidgetElement(const QString& typeName, QWidget*& widget)
{
	widget = nullptr;

	if (typeName == "boolean")
	{
		QCheckBox* checkBox = new QCheckBox();
		checkBox->setAttribute(Qt::WA_NoSystemBackground, true);
		widget = checkBox;
		return true;
	}
	else if (typeName == "string")
	{
		QTextEditSizeHint* textEdit = new QTextEditSizeHint();
		textEdit->setAttribute(Qt::WA_NoSystemBackground, true);
		textEdit->setAcceptRichText(false);
		widget = textEdit;
		return true;
	}
	else if (typeName == "integer")
	{
		QLineEdit* lineEdit = new QLineEdit();
		lineEdit->setAttribute(Qt::WA_NoSystemBackground, true);
		widget = lineEdit;
		return true;
	}
	else if (typeName == "float")
	{
		QLineEdit* lineEdit = new QLineEdit();
		lineEdit->setAttribute(Qt::WA_NoSystemBackground, true);
		widget = lineEdit;
		return true;
	}

	/** @todo: array, vector, map, tuple */

	return false;
}

QJsonObject cData::saveWidgetElement(const QString& typeName, QWidget* widget) const
{
	if (typeName == "boolean")
	{
		const QCheckBox* checkBox = (QCheckBox*)widget;
		QJsonObject json;
		json["value"] = QJsonValue(checkBox->isChecked());
		return json;
	}
	else if (typeName == "string")
	{
		const QTextEditSizeHint* textEdit = (QTextEditSizeHint*)widget;
		QJsonObject json;
		json["value"] = QJsonValue(textEdit->toPlainText());
		return json;
	}
	else if (typeName == "integer")
	{
		const QLineEdit* lineEdit = (QLineEdit*)widget;
		QJsonObject json;
		json["value"] = QJsonValue(lineEdit->text());
		return json;
	}
	else if (typeName == "float")
	{
		const QLineEdit* lineEdit = (QLineEdit*)widget;
		QJsonObject json;
		json["value"] = QJsonValue(lineEdit->text());
		return json;
	}

	/** @todo: array, vector, map, tuple */

	return QJsonObject();
}

bool cData::restoreWidgetElement(const QString& typeName, QWidget* widget, const QJsonObject& jsonObject)
{
	if (!widget)
	{
		return false;
	}

	if (typeName == "boolean")
	{
		QCheckBox* checkBox = (QCheckBox*)widget;
		checkBox->setChecked(jsonObject["value"].toBool());
		return true;
	}
	else if (typeName == "string")
	{
		QTextEditSizeHint* textEdit = (QTextEditSizeHint*)widget;
		textEdit->setPlainText(jsonObject["value"].toString());
		return true;
	}
	else if (typeName == "integer")
	{
		QLineEdit* lineEdit = (QLineEdit*)widget;
		lineEdit->setText(jsonObject["value"].toString());
		return true;
	}
	else if (typeName == "float")
	{
		QLineEdit* lineEdit = (QLineEdit*)widget;
		lineEdit->setText(jsonObject["value"].toString());
		return true;
	}

	/** @todo: array, vector, map, tuple */

	return false;
}
