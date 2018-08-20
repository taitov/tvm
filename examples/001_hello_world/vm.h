#ifndef VM_H
#define VM_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cRootStart ///< root module
{
public:
	tvmRegisterRootModule(cRootStart)
	{
		registerModule.setModuleName("start");
		registerModule.setCaptionName("start");

		registerModule.insertRootSignalExit("signal", ///< signal name
		                                    &cRootStart::rootSignalExit); ///< store signal id
	}

public:
	tRootSignalExitId rootSignalExit;
};

//

class cLogicPrint ///< logic module
{
public:
	tvmRegisterLogicModule(cLogicPrint)
	{
		registerModule.setModuleName("print");
		registerModule.setCaptionName("print");

		registerModule.insertSignalEntry("signal", ///< signal name
		                                 &cLogicPrint::signalEntry); ///< callback
	}

public:
	tFlowId signalEntry()
	{
		std::cout << "Hello world" << std::endl;
		return flowNowhere;
	}
};

//

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerRootModule<cRootStart>();
	virtualMachine.registerLogicModule<cLogicPrint>();
	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
