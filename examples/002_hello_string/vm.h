#ifndef VM_H
#define VM_H

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
	tFlowId signalEntryPrint()
	{
		if (string)
		{
			std::cout << (*string) << std::endl;
		}
		return flow;
	}

public:
	const std::string* string;

public:
	tFlowId flow;
};

eResult registerModules(cRegister& virtualMachine)
{
	eResult result;

	/// register memory modules
	result = virtualMachine.registerMemoryModuleBase<std::string>("string", ///< module name
	                                                              {"string"}, ///< value type
	                                                              "Hello!"); ///< constructor's arguments
	if (result != eResult::success)
	{
		return result;
	}

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
	result = virtualMachine.registerLogicModuleBase<cLogicPrint>("print", ///< module name
	                                                             "print", ///< caption name (for gui)
	                                                             {{"signal", ///< signal name
	                                                               &cLogicPrint::signalEntryPrint}}, ///< callback
	                                                             {{"string", ///< memory entry name
	                                                               "string", ///< memory module name
	                                                               reinterpret_cast<const void* (cLogicPrint::*)>(&cLogicPrint::string)}}, ///< store pointer
	                                                             {{"signal", ///< signal name
	                                                               &cLogicPrint::flow}}, ///< store flow id
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
