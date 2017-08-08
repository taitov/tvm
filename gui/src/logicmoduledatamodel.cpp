#include "logicmoduledatamodel.h"

using namespace nVirtualMachine::nGui;

cLogicModuleDataModel::cLogicModuleDataModel(const tLibraryName& libraryName,
                                             const tModuleTypeName& moduleTypeName,
                                             const tModuleName& moduleName,
                                             const tCaptionName& captionName,
                                             const tGuiSignalEntries& guiSignalEntries,
                                             const tGuiMemoryEntries& guiMemoryEntries,
                                             const tGuiSignalExits& guiSignalExits,
                                             const tGuiMemoryExits& guiMemoryExits,
                                             const bool& deprecated) :
        libraryName(libraryName),
        moduleTypeName(moduleTypeName),
        moduleName(moduleName),
        captionName(captionName),
        guiSignalEntries(guiSignalEntries),
        guiMemoryEntries(guiMemoryEntries),
        guiSignalExits(guiSignalExits),
        guiMemoryExits(guiMemoryExits),
        deprecated(deprecated)
{
	data.moduleTypeName = moduleTypeName;

	if (moduleTypeName.value == "logic")
	{
		NodeStyle style = nodeStyle();
		style.GradientColor0 = QColor(0x234d20);
		style.GradientColor1 = QColor(0x234d20);
		style.GradientColor2 = QColor(0x234d20);
		style.GradientColor3 = QColor(0x234d20);
		setNodeStyle(style);
	}
	else
	{
		NodeStyle style = nodeStyle();
		style.GradientColor0 = QColor(0x555555);
		style.GradientColor1 = QColor(0x555555);
		style.GradientColor2 = QColor(0x555555);
		style.GradientColor3 = QColor(0x555555);
		setNodeStyle(style);
	}
}

cLogicModuleDataModel::~cLogicModuleDataModel()
{
}

QJsonObject cLogicModuleDataModel::save() const
{
	QJsonObject jsonObject = NodeDataModel::save();
	jsonObject["moduleTypeName"] = QString::fromUtf8(moduleTypeName.value.c_str());
	jsonObject["libraryName"] = QString::fromUtf8(libraryName.value.c_str());
	jsonObject["moduleName"] = QString::fromUtf8(moduleName.value.c_str());
	return jsonObject;

}

void cLogicModuleDataModel::restore(const QJsonObject& jsonObject)
{
}

QString cLogicModuleDataModel::caption() const
{
	if (!captionName.value.length())
	{
		return QString::fromUtf8((libraryName.value + ":" + moduleName.value).c_str());
	}

	return QString::fromUtf8((captionName.value).c_str());
}

QString cLogicModuleDataModel::name() const
{
	return QString::fromUtf8((libraryName.value + ":" + moduleTypeName.value + ":" + moduleName.value).c_str());
}

std::unique_ptr<cLogicModuleDataModel::NodeDataModel> cLogicModuleDataModel::clone() const
{
	return std::make_unique<cLogicModuleDataModel>(libraryName,
	                                               moduleTypeName,
	                                               moduleName,
	                                               captionName,
	                                               guiSignalEntries,
	                                               guiMemoryEntries,
	                                               guiSignalExits,
	                                               guiMemoryExits,
	                                               deprecated);
}

const void* cLogicModuleDataModel::getData() const
{
	return &data;
}

const bool cLogicModuleDataModel::canConnect(cLogicModuleDataModel::PortType portType,
                                             cLogicModuleDataModel::NodeDataModel* model,
                                             cLogicModuleDataModel::NodeDataType nodeDataType) const
{
	cData* secondData = (cData*)model->getData();
	if (!secondData)
	{
		return true;
	}

	if (secondData->moduleTypeName.value == "scheme")
	{
		return true;
	}

	if (nodeDataType.id != "signal" &&
	    secondData->moduleTypeName.value != "memory")
	{
		return false;
	}

	return true;
}

unsigned int cLogicModuleDataModel::nPorts(cLogicModuleDataModel::PortType portType) const
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

cLogicModuleDataModel::NodeDataType cLogicModuleDataModel::dataType(cLogicModuleDataModel::PortType portType,
                                                                    cLogicModuleDataModel::PortIndex portIndex) const
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

QtNodes::NodeDataModel::ConnectionPolicy cLogicModuleDataModel::portInConnectionPolicy(cLogicModuleDataModel::PortIndex portIndex) const
{
	if (portIndex < guiSignalEntries.size())
	{
		return ConnectionPolicy::Many;
	}
	return ConnectionPolicy::One;
}

QtNodes::NodeDataModel::ConnectionPolicy cLogicModuleDataModel::portOutConnectionPolicy(cLogicModuleDataModel::PortIndex) const
{
	return ConnectionPolicy::One;
}

std::shared_ptr<cLogicModuleDataModel::NodeData> cLogicModuleDataModel::outData(cLogicModuleDataModel::PortIndex)
{
	return nullptr;
}

void cLogicModuleDataModel::setInData(std::shared_ptr<cLogicModuleDataModel::NodeData>, int)
{
}

QWidget* cLogicModuleDataModel::embeddedWidget()
{
	return nullptr;
}

cLogicModuleDataModel::NodeValidationState cLogicModuleDataModel::validationState() const
{
	if (deprecated)
	{
		return NodeValidationState::Warning;
	}
	return NodeValidationState::Valid;
}

QString cLogicModuleDataModel::validationMessage() const
{
	if (deprecated)
	{
		return "deprecated";
	}
	return "";
}
