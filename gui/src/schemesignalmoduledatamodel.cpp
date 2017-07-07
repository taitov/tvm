#include "schemesignalmoduledatamodel.h"

using namespace nVirtualMachine::nGui;

cSchemeSignalModuleDataModel::cSchemeSignalModuleDataModel(const QString& moduleFullName,
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

	lineEdit = new QLineEdit();
	lineEdit->setPlaceholderText("signalName");
}

cSchemeSignalModuleDataModel::~cSchemeSignalModuleDataModel()
{
}

QJsonObject cSchemeSignalModuleDataModel::save() const
{
	QJsonObject jsonObject = NodeDataModel::save();
	jsonObject["moduleTypeName"] = "scheme";
	jsonObject["portName"] = lineEdit->text();
	jsonObject["direction"] = (direction == PortType::Out ? "inSignal" : "outSignal");
	return jsonObject;
}

void cSchemeSignalModuleDataModel::restore(const QJsonObject& jsonObject)
{
	lineEdit->setText(jsonObject["portName"].toString());
}

QString cSchemeSignalModuleDataModel::caption() const
{
	if (direction == PortType::Out)
	{
		return "inSignal";
	}
	return "outSignal";
}

QString cSchemeSignalModuleDataModel::name() const
{
	return moduleFullName;
}

std::unique_ptr<cSchemeSignalModuleDataModel::NodeDataModel> cSchemeSignalModuleDataModel::clone() const
{
	return std::make_unique<cSchemeSignalModuleDataModel>(moduleFullName,
	                                                      direction);
}

const void* cSchemeSignalModuleDataModel::getData() const
{
	return &data;
}

unsigned int cSchemeSignalModuleDataModel::nPorts(cSchemeSignalModuleDataModel::PortType portType) const
{
	if (portType == direction)
	{
		return 1;
	}

	return 0;
}

cSchemeSignalModuleDataModel::NodeDataType cSchemeSignalModuleDataModel::dataType(cSchemeSignalModuleDataModel::PortType portType,
                                                                                  cSchemeSignalModuleDataModel::PortIndex portIndex) const
{
	if (portType == direction)
	{
		if (portIndex == 0)
		{
			if (lineEdit->text().isEmpty())
			{
				return NodeDataType {"signal", "signal"};
			}

			return NodeDataType {"signal",
			                     lineEdit->text()};
		}
	}

	return NodeDataType();
}

QtNodes::NodeDataModel::ConnectionPolicy cSchemeSignalModuleDataModel::portInConnectionPolicy(cSchemeSignalModuleDataModel::PortIndex portIndex) const
{
	return ConnectionPolicy::Many;
}

QtNodes::NodeDataModel::ConnectionPolicy cSchemeSignalModuleDataModel::portOutConnectionPolicy(cSchemeSignalModuleDataModel::PortIndex) const
{
	return ConnectionPolicy::One;
}

bool cSchemeSignalModuleDataModel::portCaptionVisible(cSchemeSignalModuleDataModel::PortType, cSchemeSignalModuleDataModel::PortIndex) const
{
	return true;
}

std::shared_ptr<cSchemeSignalModuleDataModel::NodeData> cSchemeSignalModuleDataModel::outData(cSchemeSignalModuleDataModel::PortIndex)
{
	return nullptr;
}

void cSchemeSignalModuleDataModel::setInData(std::shared_ptr<cSchemeSignalModuleDataModel::NodeData>, int)
{
}

QWidget* cSchemeSignalModuleDataModel::embeddedWidget()
{
	return lineEdit;
}
