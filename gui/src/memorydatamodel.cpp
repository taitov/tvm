#include "memorydatamodel.h"

using namespace nVirtualMachine::nGui;

cMemoryDataModel::cMemoryDataModel(const tMemoryTypeName& memoryTypeName) :
        memoryTypeName(memoryTypeName)
{
	NodeStyle style = nodeStyle();
	style.GradientColor0 = QColor(0x83502e);
	style.GradientColor1 = QColor(0x83502e);
	style.GradientColor2 = QColor(0x83502e);
	style.GradientColor3 = QColor(0x83502e);
	setNodeStyle(style);
}

cMemoryDataModel::~cMemoryDataModel()
{
}

QJsonObject cMemoryDataModel::save() const
{
	QJsonObject jsonObject = NodeDataModel::save();
	jsonObject["moduleTypeName"] = "memory";
	jsonObject["memoryTypeName"] = QString::fromUtf8(memoryTypeName.value.c_str());
	return jsonObject;
}

void cMemoryDataModel::restore(const QJsonObject& jsonObject)
{
}

QString cMemoryDataModel::caption() const
{
	return QString::fromUtf8(("memory:" + memoryTypeName.value).c_str());
}

QString cMemoryDataModel::name() const
{
	return QString::fromUtf8((":memory:" + memoryTypeName.value).c_str());
}

std::unique_ptr<cMemoryDataModel::NodeDataModel> cMemoryDataModel::clone() const
{
	return std::make_unique<cMemoryDataModel>(memoryTypeName);
}

const void* cMemoryDataModel::getData() const
{
	return nullptr;
}

const bool cMemoryDataModel::canConnect(cMemoryDataModel::PortType portType,
                                        cMemoryDataModel::NodeDataModel* model,
                                        cMemoryDataModel::NodeDataType nodeDataType) const
{
//	cModelData* data = (cModelData*)model->getData();
//	if (data->moduleTypeName == modelData.moduleTypeName)
//	{
//		return false;
//	}
	return true;
}

unsigned int cMemoryDataModel::nPorts(cMemoryDataModel::PortType portType) const
{
	if (portType == PortType::In)
	{
		return 1;
	}
	else if (portType == PortType::Out)
	{
		return 1;
	}

	return 0;
}

cMemoryDataModel::NodeDataType cMemoryDataModel::dataType(cMemoryDataModel::PortType portType, cMemoryDataModel::PortIndex portIndex) const
{
	if (portType == PortType::In)
	{
		if (portIndex == 0)
		{
			return NodeDataType {QString::fromUtf8((memoryTypeName.value).c_str()),
			                     ""};
		}
	}
	else if (portType == PortType::Out)
	{
		if (portIndex == 0)
		{
			return NodeDataType {QString::fromUtf8((memoryTypeName.value).c_str()),
			                     ""};
		}
	}

	return NodeDataType();
}

QtNodes::NodeDataModel::ConnectionPolicy cMemoryDataModel::portInConnectionPolicy(cMemoryDataModel::PortIndex) const
{
	return ConnectionPolicy::Many;
}

QtNodes::NodeDataModel::ConnectionPolicy cMemoryDataModel::portOutConnectionPolicy(cMemoryDataModel::PortIndex) const
{
	return ConnectionPolicy::Many;
}

bool cMemoryDataModel::portCaptionVisible(cMemoryDataModel::PortType, cMemoryDataModel::PortIndex) const
{
	return true;
}

std::shared_ptr<cMemoryDataModel::NodeData> cMemoryDataModel::outData(cMemoryDataModel::PortIndex)
{
	return nullptr;
}

void cMemoryDataModel::setInData(std::shared_ptr<cMemoryDataModel::NodeData>, int)
{
}

QWidget* cMemoryDataModel::embeddedWidget()
{
	return nullptr;
}
