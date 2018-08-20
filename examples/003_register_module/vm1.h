#ifndef VM1_H
#define VM1_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cRootStart1 ///< root module
{
public:
	tRootSignalExitId rootSignalExit;
};

class cLogicPrint1 ///< logic module
{
public:
	tFlowId signalEntry()
	{
		if (string)
		{
			std::cout << (*string) << std::endl;
		}
		return flowNowhere;
	}

public:
	const std::string* string;
};

eResult registerModules1(cRegister& virtualMachine)
{
	eResult result;

	/// register memory modules
	result = virtualMachine.registerMemoryModuleBase<std::string>("string1", ///< module name
	                                                              {"value", ///< type
	                                                               "string"}, ///< value type
	                                                              "Hello world 1"); ///< constructor's arguments
	if (result != eResult::success)
	{
		return result;
	}

	/// register root modules
	result = virtualMachine.registerRootModuleBase<cRootStart1>("start1", ///< module name
	                                                            "start", ///< caption name (for gui)
	                                                            {{"signal", ///< signal name
	                                                              &cRootStart1::rootSignalExit}}, ///< store signal id
	                                                            {});
	if (result != eResult::success)
	{
		return result;
	}

	/// register logic modules
	result = virtualMachine.registerLogicModuleBase<cLogicPrint1>("print1", ///< module name
	                                                              "print", ///< caption name (for gui)
	                                                              {{"signal", ///< signal name
	                                                                &cLogicPrint1::signalEntry}}, ///< callback
	                                                              {{"memoryEntry", ///< memory entry name
	                                                                "string", ///< memory module name
	                                                                reinterpret_cast<const void* (cLogicPrint1::*)>(&cLogicPrint1::string)}}, ///< store pointer
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

#endif // VM1_H
