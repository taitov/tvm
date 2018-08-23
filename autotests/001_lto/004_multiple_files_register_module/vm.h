#ifndef VM_H
#define VM_H

#include <iostream>
#include <string>

#include <tvm/register.h>

#include "memory_string1.h"
#include "memory_string2.h"
#include "root_start1.h"
#include "root_start2.h"
#include "logic_print1.h"
#include "logic_print2.h"

namespace tvm
{

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerMemoryModule<cMemoryString1>("_TEXT_ONLY_FOR_ENGINE_1_");
	virtualMachine.registerMemoryModule<cMemoryString2>("_TEXT_ONLY_FOR_ENGINE_2_");
	virtualMachine.registerRootModule<cRootStart1>("_TEXT_ONLY_FOR_ENGINE_3_");
	virtualMachine.registerRootModule<cRootStart2>("_TEXT_ONLY_FOR_ENGINE_4_");
	virtualMachine.registerLogicModule<cLogicPrint1>("_TEXT_ONLY_FOR_ENGINE_5_");
	virtualMachine.registerLogicModule<cLogicPrint2>("_TEXT_ONLY_FOR_ENGINE_6_");
	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
