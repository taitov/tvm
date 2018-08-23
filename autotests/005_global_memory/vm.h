#ifndef VM_H
#define VM_H

#include <string>
#include <tuple>
#include <map>

#include <tvm/register.h>

#include "root.h"
#include "logic.h"

namespace tvm
{

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerMemoryModuleBase<int32_t>("integer",
	                                                 {"value",
	                                                  "int32"},
	                                                 0);

	virtualMachine.registerRootModule<cRootStart>();
	virtualMachine.registerRootModule<cRootPrintInteger>();

	virtualMachine.registerLogicModule<cLogicPrintInteger>();
	virtualMachine.registerLogicModule<cLogicIncrement>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
