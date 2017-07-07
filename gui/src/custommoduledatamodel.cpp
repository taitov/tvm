#include "custommoduledatamodel.h"

using namespace nVirtualMachine::nGui;

cCustomModuleDataModel::cCustomModuleDataModel(const QString& moduleFullName,
                                               const QString& captionName,
                                               const tGuiSignalEntries& guiSignalEntries,
                                               const tGuiMemoryEntries& guiMemoryEntries,
                                               const tGuiSignalExits& guiSignalExits,
                                               const tGuiMemoryExits& guiMemoryExits) :
        moduleFullName(moduleFullName),
        captionName(captionName),
        guiSignalEntries(guiSignalEntries),
        guiMemoryEntries(guiMemoryEntries),
        guiSignalExits(guiSignalExits),
        guiMemoryExits(guiMemoryExits)
{
	NodeStyle style = nodeStyle();
	style.GradientColor0 = QColor(0x800080);
	style.GradientColor1 = QColor(0x800080);
	style.GradientColor2 = QColor(0x800080);
	style.GradientColor3 = QColor(0x800080);
	setNodeStyle(style);
}

cCustomModuleDataModel::~cCustomModuleDataModel()
{
}

QJsonObject cCustomModuleDataModel::save() const
{
	QJsonObject jsonObject = NodeDataModel::save();
	jsonObject["moduleTypeName"] = "custom";
	jsonObject["schemeName"] = moduleFullName;
	return jsonObject;
}

void cCustomModuleDataModel::restore(const QJsonObject& jsonObject)
{
}

QString cCustomModuleDataModel::caption() const
{
	return captionName;
}

QString cCustomModuleDataModel::name() const
{
	return moduleFullName;
}

std::unique_ptr<cCustomModuleDataModel::NodeDataModel> cCustomModuleDataModel::clone() const
{
	return std::make_unique<cCustomModuleDataModel>(moduleFullName,
	                                                captionName,
	                                                guiSignalEntries,
	                                                guiMemoryEntries,
	                                                guiSignalExits,
	                                                guiMemoryExits);
}

const bool cCustomModuleDataModel::canConnect(cCustomModuleDataModel::PortType portType,
                                              cCustomModuleDataModel::NodeDataModel* model,
                                              cCustomModuleDataModel::NodeDataType nodeDataType) const
{
	/** @todo */
	return true;
}

unsigned int cCustomModuleDataModel::nPorts(cCustomModuleDataModel::PortType portType) const
{
	if (portType == PortType::In)
	{
		return guiSignalEntries.size() + guiMemoryEntries.size();
	}
	else if (portType == PortType::Out)
	{
		return guiSignalExits.size() + guiMemoryExits.size();
	}

	return 0;
}

cCustomModuleDataModel::NodeDataType cCustomModuleDataModel::dataType(cCustomModuleDataModel::PortType portType,
                                                                      cCustomModuleDataModel::PortIndex portIndex) const
{
	if (portType == PortType::In)
	{
		if (portIndex < guiSignalEntries.size())
		{
			auto iter = guiSignalEntries.begin();
			std::advance(iter, portIndex);
			return NodeDataType {"signal",
			                     QString::fromUtf8(iter->first.value.c_str())};
		}
		else if (portIndex < guiSignalEntries.size() + guiMemoryEntries.size())
		{
			auto iter = guiMemoryEntries.begin();
			std::advance(iter, portIndex - guiSignalEntries.size());
			return NodeDataType {QString::fromUtf8((std::get<0>(iter->second)).value.c_str()),
			                     QString::fromUtf8((iter->first).value.c_str())};
		}
	}
	else if (portType == PortType::Out)
	{
		if (portIndex < guiSignalExits.size())
		{
			auto iter = guiSignalExits.begin();
			std::advance(iter, portIndex);
			return NodeDataType {"signal",
			                     QString::fromUtf8(iter->first.value.c_str())};
		}
		else if (portIndex < guiSignalExits.size() + guiMemoryExits.size())
		{
			auto iter = guiMemoryExits.begin();
			std::advance(iter, portIndex - guiSignalExits.size());
			return NodeDataType {QString::fromUtf8((std::get<0>(iter->second)).value.c_str()),
			                     QString::fromUtf8((iter->first).value.c_str())};
		}
	}

	return NodeDataType();
}

QtNodes::NodeDataModel::ConnectionPolicy cCustomModuleDataModel::portInConnectionPolicy(cCustomModuleDataModel::PortIndex portIndex) const
{
	return ConnectionPolicy::Many;
}

QtNodes::NodeDataModel::ConnectionPolicy cCustomModuleDataModel::portOutConnectionPolicy(cCustomModuleDataModel::PortIndex portIndex) const
{
	if (portIndex < guiSignalExits.size())
	{
		return ConnectionPolicy::One;
	}
	return ConnectionPolicy::Many;
}

std::shared_ptr<cCustomModuleDataModel::NodeData> cCustomModuleDataModel::outData(cCustomModuleDataModel::PortIndex)
{
	return nullptr;
}

void cCustomModuleDataModel::setInData(std::shared_ptr<cCustomModuleDataModel::NodeData>, int)
{
}

QWidget* cCustomModuleDataModel::embeddedWidget()
{
	return nullptr;
}
