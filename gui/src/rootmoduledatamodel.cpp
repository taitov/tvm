#include "rootmoduledatamodel.h"

using namespace nVirtualMachine::nGui;

cRootModuleDataModel::cRootModuleDataModel(const tLibraryName& libraryName,
                                           const tRootModuleName& rootModuleName,
                                           const tGuiRootSignalExits& guiRootSignalExits,
                                           const tGuiRootMemoryExits& guiRootMemoryExits) :
        libraryName(libraryName),
        rootModuleName(rootModuleName),
        guiRootSignalExits(guiRootSignalExits),
        guiRootMemoryExits(guiRootMemoryExits)
{
	NodeStyle style = nodeStyle();
	style.GradientColor0 = QColor(0xc30101);
	style.GradientColor1 = QColor(0xc30101);
	style.GradientColor2 = QColor(0xc30101);
	style.GradientColor3 = QColor(0xc30101);
	setNodeStyle(style);
}

cRootModuleDataModel::~cRootModuleDataModel()
{
}

QJsonObject cRootModuleDataModel::save() const
{
	QJsonObject jsonObject = NodeDataModel::save();
	jsonObject["moduleTypeName"] = "root";
	jsonObject["libraryName"] = QString::fromUtf8(libraryName.value.c_str());
	jsonObject["rootModuleName"] = QString::fromUtf8(rootModuleName.value.c_str());
	return jsonObject;
}

void cRootModuleDataModel::restore(const QJsonObject& jsonObject)
{
}

QString cRootModuleDataModel::caption() const
{
	return QString::fromUtf8((libraryName.value + ":" + rootModuleName.value).c_str());
}

QString cRootModuleDataModel::name() const
{
	return QString::fromUtf8((libraryName.value + ":root:" + rootModuleName.value).c_str());
}

std::unique_ptr<cRootModuleDataModel::NodeDataModel> cRootModuleDataModel::clone() const
{
	return std::make_unique<cRootModuleDataModel>(libraryName,
	                                              rootModuleName,
	                                              guiRootSignalExits,
	                                              guiRootMemoryExits);
}

const void* cRootModuleDataModel::getData() const
{
	return nullptr;
}

size_t cRootModuleDataModel::uniqueCount() const
{
	return 1;
}

unsigned int cRootModuleDataModel::nPorts(cRootModuleDataModel::PortType portType) const
{
	if (portType == PortType::In)
	{
		return 0;
	}
	else if (portType == PortType::Out)
	{
		return guiRootSignalExits.size() + guiRootMemoryExits.size();
	}

	return 0;
}

cRootModuleDataModel::NodeDataType cRootModuleDataModel::dataType(cRootModuleDataModel::PortType portType,
                                                                  cRootModuleDataModel::PortIndex portIndex) const
{
	if (portType == PortType::In)
	{
	}
	else if (portType == PortType::Out)
	{
		if (portIndex < guiRootSignalExits.size())
		{
			auto& iter = guiRootSignalExits[portIndex];
			return NodeDataType {"signal",
			                     QString::fromUtf8(iter.value.c_str())};
		}
		else if (portIndex < guiRootSignalExits.size() + guiRootMemoryExits.size())
		{
			auto& iter = guiRootMemoryExits[portIndex - guiRootSignalExits.size()];
			return NodeDataType {QString::fromUtf8((std::get<1>(iter)).value.c_str()),
			                     QString::fromUtf8((std::get<0>(iter)).value.c_str())};
		}
	}

	return NodeDataType();
}

QtNodes::NodeDataModel::ConnectionPolicy cRootModuleDataModel::portOutConnectionPolicy(cRootModuleDataModel::PortIndex) const
{
	return ConnectionPolicy::One;
}

std::shared_ptr<cRootModuleDataModel::NodeData> cRootModuleDataModel::outData(cRootModuleDataModel::PortIndex)
{
	return nullptr;
}

void cRootModuleDataModel::setInData(std::shared_ptr<cRootModuleDataModel::NodeData>, int)
{
}

QWidget* cRootModuleDataModel::embeddedWidget()
{
	return nullptr;
}
