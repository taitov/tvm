#ifndef VM_H
#define VM_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cRootStart ///< root module
{
public:
	tRootSignalExitId rootSignalExit;
};

class cLogicPrint ///< logic module
{
public:
	tFlowId signalEntry()
	{
		std::cout << "Hello world" << std::endl;
		return flowNowhere;
	}
};

eResult registerModules(cRegister& virtualMachine)
{
	eResult result;

	/// register root modules
	result = virtualMachine.registerRootModuleBase<cRootStart>("start", ///< module name
	                                                           "start", ///< caption name (for gui)
	                                                           {{"signal", ///< signal name
	                                                             &cRootStart::rootSignalExit}}, ///< store signal id
	                                                           {});
	if (result != eResult::success)
	{
		return result;
	}

	/// register logic modules
	result = virtualMachine.registerLogicModuleBase<cLogicPrint>("printHelloWorld", ///< module name
	                                                             "printHelloWorld", ///< caption name (for gui)
	                                                             {{"signal", ///< signal name
	                                                               &cLogicPrint::signalEntry}}, ///< callback
	                                                             {},
	                                                             {},
	                                                             {},
	                                                             {});
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

}

#endif // VM_H
