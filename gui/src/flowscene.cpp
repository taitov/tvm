#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

#include <tvm/vm.h>

#include <nodes/DataModelRegistry>

#include "flowscene.h"
#include "memorydatamodel.h"
#include "rootmoduledatamodel.h"
#include "logicmoduledatamodel.h"
#include "schemesignalmoduledatamodel.h"
#include "schemememorymoduledatamodel.h"
#include "custommoduledatamodel.h"

using namespace nVirtualMachine::nGui;

cFlowSceneWidget::cFlowSceneWidget(const cVirtualMachine* virtualMachine,
                                   bool addSchemeModules) :
        virtualMachine(virtualMachine),
        addSchemeModules(addSchemeModules)
{
	setRegistry(makeVirtualMachineDataModelRegistry(addSchemeModules));
}

void cFlowSceneWidget::setCustomModulePaths(const std::vector<QString>& paths)
{
	customModulePaths = paths;

	auto dataModelRegistry = makeVirtualMachineDataModelRegistry(addSchemeModules);

	for (const QString& customModulePath : customModulePaths)
	{
		QDir dir(customModulePath);
		updateCustomModuleDir(dataModelRegistry, dir, ":custom:");
	}

	setRegistry(dataModelRegistry);
}

std::shared_ptr<QtNodes::DataModelRegistry> cFlowSceneWidget::makeVirtualMachineDataModelRegistry(bool addSchemeModules)
{
	auto dataModelRegistry = std::make_shared<DataModelRegistry>();

	const auto memoryTypes = virtualMachine->getGuiMemoryTypes();
	for (const auto& iterMemoryType : memoryTypes)
	{
		dataModelRegistry->registerModel<cMemoryDataModel>(std::make_unique<cMemoryDataModel>(iterMemoryType.first));
	}

	const auto rootModules = virtualMachine->getGuiRootModules();
	for (const auto& iterRootModule : rootModules)
	{
		dataModelRegistry->registerModel<cRootModuleDataModel>
		        (std::make_unique<cRootModuleDataModel>(std::get<0>(iterRootModule.first),
		                                                std::get<1>(iterRootModule.first),
		                                                std::get<0>(iterRootModule.second),
		                                                std::get<1>(iterRootModule.second)));
	}

	const auto modules = virtualMachine->getGuiModules();
	for (const auto& iterModule : modules)
	{
		dataModelRegistry->registerModel<cLogicModuleDataModel>
		        (std::make_unique<cLogicModuleDataModel>(std::get<0>(iterModule.first),
		                                                 iterModule.second->getModuleTypeName(),
		                                                 std::get<1>(iterModule.first),
		                                                 iterModule.second->getCaptionName(),
		                                                 iterModule.second->getSignalEntries(),
		                                                 iterModule.second->getMemoryEntries(),
		                                                 iterModule.second->getSignalExits(),
		                                                 iterModule.second->getMemoryExits()));
	}

	const auto memoryModules = virtualMachine->getGuiMemoryModules();
	for (const auto& iterMemoryModules : memoryModules)
	{
		dataModelRegistry->registerModel<cLogicModuleDataModel>
		                (std::make_unique<cLogicModuleDataModel>(":memory:" + std::get<0>(iterMemoryModules.first).value,
		                                                         iterMemoryModules.second->getModuleTypeName(),
		                                                         std::get<1>(iterMemoryModules.first),
		                                                         iterMemoryModules.second->getCaptionName(),
		                                                         iterMemoryModules.second->getSignalEntries(),
		                                                         iterMemoryModules.second->getMemoryEntries(),
		                                                         iterMemoryModules.second->getSignalExits(),
		                                                         iterMemoryModules.second->getMemoryExits()));
	}

	if (addSchemeModules)
	{
		dataModelRegistry->registerModel<cSchemeSignalModuleDataModel>(std::make_unique<cSchemeSignalModuleDataModel>(":scheme:inSignal",
		                                                                                                              QtNodes::PortType::Out));
		dataModelRegistry->registerModel<cSchemeSignalModuleDataModel>(std::make_unique<cSchemeSignalModuleDataModel>(":scheme:outSignal",
		                                                                                                              QtNodes::PortType::In));
		dataModelRegistry->registerModel<cSchemeMemoryModuleDataModel>(std::make_unique<cSchemeMemoryModuleDataModel>(":scheme:inMemory",
		                                                                                                              QtNodes::PortType::Out));
		dataModelRegistry->registerModel<cSchemeMemoryModuleDataModel>(std::make_unique<cSchemeMemoryModuleDataModel>(":scheme:outMemory",
		                                                                                                              QtNodes::PortType::In));
	}

	return dataModelRegistry;
}

static bool readCustomModule(const QString& filePath,
                             cCustomModuleDataModel::tGuiSignalEntries& signalEntries,
                             cCustomModuleDataModel::tGuiMemoryEntries& memoryEntries,
                             cCustomModuleDataModel::tGuiSignalExits& signalExits,
                             cCustomModuleDataModel::tGuiMemoryExits& memoryExits)
{
	if (!QFileInfo::exists(filePath))
	{
		printf("error: QFileInfo::exists()\n");
		return false;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		printf("error: file.open()\n");
		return false;
	}

	QByteArray wholeFile = file.readAll();

	QJsonObject const jsonDocument = QJsonDocument::fromJson(wholeFile).object();

	QJsonArray nodeJsonArray = jsonDocument["nodes"].toArray();
	for (int i = 0; i < nodeJsonArray.size(); ++i)
	{
		QJsonObject nodeJson = nodeJsonArray[i].toObject();
		if (nodeJson.find("model") != nodeJson.end())
		{
			QJsonObject modelJson = nodeJson["model"].toObject();
			if (modelJson.find("name") != modelJson.end())
			{
				if (modelJson["name"].toString() == ":scheme:inSignal")
				{
					QString portName = modelJson["portName"].toString();
					if (portName.isEmpty())
					{
						portName = "signal";
					}

					const auto value = std::make_tuple(signalEntries.size(),
					                                   nullptr);
					signalEntries[portName.toStdString()] = value;
				}
				else if (modelJson["name"].toString() == ":scheme:outSignal")
				{
					QString portName = modelJson["portName"].toString();
					if (portName.isEmpty())
					{
						portName = "signal";
					}

					const auto value = signalExits.size();
					signalExits[portName.toStdString()] = value;
				}
				else if (modelJson["name"].toString() == ":scheme:inMemory")
				{
					QString portName = modelJson["portName"].toString();
					QString portType = modelJson["portType"].toString();

					if (!portType.isEmpty())
					{
						if (portName.isEmpty())
						{
							portName = portType;
						}

						const auto value = std::make_tuple(portType.toStdString(),
						                                   0);
						memoryEntries[portName.toStdString()] = value;
					}
				}
				else if (modelJson["name"].toString() == ":scheme:outMemory")
				{
					QString portName = modelJson["portName"].toString();
					QString portType = modelJson["portType"].toString();

					if (!portType.isEmpty())
					{
						if (portName.isEmpty())
						{
							portName = portType;
						}

						const auto value = std::make_tuple(portType.toStdString(),
						                                   0);
						memoryExits[portName.toStdString()] = value;
					}
				}
			}
		}
	}

	return true;
}

void cFlowSceneWidget::updateCustomModuleDir(std::shared_ptr<QtNodes::DataModelRegistry> dataModelRegistry,
                                             const QDir& dir,
                                             const QString& modulePrefixName)
{
	for (const QFileInfo& fileInfo : dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs))
	{
		QDir subDir(fileInfo.absoluteFilePath());
		updateCustomModuleDir(dataModelRegistry, subDir, modulePrefixName + fileInfo.baseName() + ":");
	}

	for (const QFileInfo& fileInfo : dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files))
	{
		if (fileInfo.absoluteFilePath().endsWith(".tvmcustom", Qt::CaseInsensitive))
		{
			QString moduleFullName = modulePrefixName + fileInfo.baseName();
			QString captionName = fileInfo.baseName();

			cCustomModuleDataModel::tGuiSignalEntries signalEntries;
			cCustomModuleDataModel::tGuiMemoryEntries memoryEntries;
			cCustomModuleDataModel::tGuiSignalExits signalExits;
			cCustomModuleDataModel::tGuiMemoryExits memoryExits;

			if (readCustomModule(fileInfo.absoluteFilePath(),
			                     signalEntries,
			                     memoryEntries,
			                     signalExits,
			                     memoryExits))
			{
				dataModelRegistry->registerModel<cCustomModuleDataModel>(std::make_unique<cCustomModuleDataModel>(moduleFullName,
				                                                                                                  captionName,
				                                                                                                  fileInfo.absoluteFilePath(),
				                                                                                                  signalEntries,
				                                                                                                  memoryEntries,
				                                                                                                  signalExits,
				                                                                                                  memoryExits));
			}
		}
	}
}
