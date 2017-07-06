#include <tvm/vm.h>

#include <nodes/DataModelRegistry>

#include "flowscene.h"
#include "memorydatamodel.h"
#include "rootmoduledatamodel.h"
#include "logicmoduledatamodel.h"
#include "schemesignalmoduledatamodel.h"
#include "schemememorymoduledatamodel.h"

using namespace nVirtualMachine::nGui;

cFlowSceneWidget::cFlowSceneWidget(const cVirtualMachine* virtualMachine,
                                   bool addSchemeModules) :
        virtualMachine(virtualMachine)
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

	setRegistry(dataModelRegistry);
}
