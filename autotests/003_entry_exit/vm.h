#ifndef VM_H
#define VM_H

#include <string>
#include <tuple>
#include <map>

#include <tvm/register.h>

namespace tvm
{

static unsigned int staticId = 1001;
std::string checkString;

}

#include "root_start1.h"
#include "root_start2.h"
#include "root_second_start1.h"
#include "root_second_start2.h"
#include "logic_print10.h"
#include "logic_print11.h"
#include "logic_print20.h"
#include "logic_print22.h"
#include "logic_print32.h"
#include "logic_print33.h"

namespace tvm
{

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerRootModule<cRootStart1>();
	virtualMachine.registerRootModule<cRootStart2>();
	virtualMachine.registerRootModule<cRootSecondStart1>();
	virtualMachine.registerRootModule<cRootSecondStart2>();

	virtualMachine.registerLogicModule<cLogicPrint10>();
	virtualMachine.registerLogicModule<cLogicPrint11>();
	virtualMachine.registerLogicModule<cLogicPrint20>();
	virtualMachine.registerLogicModule<cLogicPrint22>();
	virtualMachine.registerLogicModule<cLogicPrint32>();
	virtualMachine.registerLogicModule<cLogicPrint33>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
