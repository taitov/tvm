#include <QtWidgets/QVBoxLayout>

#include "schemememorymoduledatamodel.h"

using namespace nVirtualMachine::nGui;

cSchemeMemoryModuleDataModel::cSchemeMemoryModuleDataModel(const QString& moduleFullName,
                                                           const PortType& direction) :
        moduleFullName(moduleFullName),
        direction(direction)
{
	data.moduleTypeName = "scheme";

	NodeStyle style = nodeStyle();
	style.GradientColor0 = QColor(0x800080);
	style.GradientColor1 = QColor(0x800080);
	style.GradientColor2 = QColor(0x800080);
	style.GradientColor3 = QColor(0x800080);
	setNodeStyle(style);

	lineEditName = new QLineEdit();
	lineEditName->setPlaceholderText("memoryName");

	lineEditType = new QLineEdit();
	lineEditType->setPlaceholderText("memoryType");

	mainWidget = new QWidget();
	mainWidget->setAttribute(Qt::WA_NoSystemBackground, true);

	QVBoxLayout* layout = new QVBoxLayout(mainWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(lineEditName);
	layout->addWidget(lineEditType);
}

cSchemeMemoryModuleDataModel::~cSchemeMemoryModuleDataModel()
{
}

QJsonObject cSchemeMemoryModuleDataModel::save() const
{
	QJsonObject jsonObject = NodeDataModel::save();
	jsonObject["moduleTypeName"] = "scheme";
	jsonObject["portName"] = lineEditName->text();
	jsonObject["portType"] = lineEditType->text();
	jsonObject["direction"] = (direction == PortType::Out ? "inMemory" : "outMemory");
	return jsonObject;
}

void cSchemeMemoryModuleDataModel::restore(const QJsonObject& jsonObject)
{
	lineEditName->setText(jsonObject["portName"].toString());
	lineEditType->setText(jsonObject["portType"].toString());
}

QString cSchemeMemoryModuleDataModel::caption() const
{
	if (direction == PortType::Out)
	{
		return "inMemory";
	}
	return "outMemory";
}

QString cSchemeMemoryModuleDataModel::name() const
{
	return moduleFullName;
}

std::unique_ptr<cSchemeMemoryModuleDataModel::NodeDataModel> cSchemeMemoryModuleDataModel::clone() const
{
	return std::make_unique<cSchemeMemoryModuleDataModel>(moduleFullName,
	                                                      direction);
}

const void* cSchemeMemoryModuleDataModel::getData() const
{
	return &data;
}

unsigned int cSchemeMemoryModuleDataModel::nPorts(cSchemeMemoryModuleDataModel::PortType portType) const
{
	if (portType == direction)
	{
		return 1;
	}

	return 0;
}

cSchemeMemoryModuleDataModel::NodeDataType cSchemeMemoryModuleDataModel::dataType(cSchemeMemoryModuleDataModel::PortType portType,
                                                                                  cSchemeMemoryModuleDataModel::PortIndex portIndex) const
{
	if (portType == direction)
	{
		if (portIndex == 0)
		{
			if (lineEditName->text().isEmpty())
			{
				return NodeDataType {lineEditType->text(), lineEditType->text()};
			}

			return NodeDataType {lineEditType->text(),
			                     lineEditName->text()};
		}
	}

	return NodeDataType();
}

QtNodes::NodeDataModel::ConnectionPolicy cSchemeMemoryModuleDataModel::portInConnectionPolicy(cSchemeMemoryModuleDataModel::PortIndex portIndex) const
{
	return ConnectionPolicy::Many;
}

QtNodes::NodeDataModel::ConnectionPolicy cSchemeMemoryModuleDataModel::portOutConnectionPolicy(cSchemeMemoryModuleDataModel::PortIndex) const
{
	return ConnectionPolicy::Many;
}

bool cSchemeMemoryModuleDataModel::portCaptionVisible(cSchemeMemoryModuleDataModel::PortType, cSchemeMemoryModuleDataModel::PortIndex) const
{
	return true;
}

std::shared_ptr<cSchemeMemoryModuleDataModel::NodeData> cSchemeMemoryModuleDataModel::outData(cSchemeMemoryModuleDataModel::PortIndex)
{
	return nullptr;
}

void cSchemeMemoryModuleDataModel::setInData(std::shared_ptr<cSchemeMemoryModuleDataModel::NodeData>, int)
{
}

QWidget* cSchemeMemoryModuleDataModel::embeddedWidget()
{
	return mainWidget;
}
