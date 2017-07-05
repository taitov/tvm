#include <tvm/vm.h>

#include <nodes/DataModelRegistry>

#include "flowscene.h"
#include "memorydatamodel.h"

using namespace nVirtualMachine::nGui;

cFlowSceneWidget::cFlowSceneWidget(const cVirtualMachine* virtualMachine) :
        virtualMachine(virtualMachine)
{
	auto dataModelRegistry = std::make_shared<DataModelRegistry>();

	const auto memoryTypes = virtualMachine->getGuiMemoryTypes();
	for (const auto& iterMemoryType : memoryTypes)
	{
		dataModelRegistry->registerModel<cMemoryDataModel>(std::make_unique<cMemoryDataModel>(iterMemoryType.first));
	}

	setRegistry(dataModelRegistry);
}
