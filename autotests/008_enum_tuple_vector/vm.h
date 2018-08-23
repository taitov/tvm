#ifndef VM_H
#define VM_H

#include <string>
#include <tuple>
#include <map>

#include <tvm/register.h>

#include "root.h"
#include "logic.h"
#include "memory.h"

namespace tvm
{

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerMemoryModule<cMemoryEnumPrintType>();
	virtualMachine.registerMemoryModule<cMemoryInteger>();
	virtualMachine.registerMemoryModule<cMemoryString>();
	virtualMachine.registerMemoryModule<cMemoryLine>();
	virtualMachine.registerMemoryModule<cMemoryVectorLines>();

	virtualMachine.registerRootModule<cRootStart>();

	virtualMachine.registerLogicModule<cLogicPrintLine>();
	virtualMachine.registerLogicModule<cLogicPrintLines>();
// XXX
//	virtualMachine.registerLogicModule<cLogicLineSetId>();
//	virtualMachine.registerLogicModule<cLogicLineSetValue>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
