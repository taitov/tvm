#ifndef VM_H
#define VM_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cMemoryString : public std::string ///< memory module
{
public:
	tvmRegisterMemoryModule(cMemoryString)
	{
		registerModule.setModuleName("string");
		registerModule.setMemoryAsString();
	}

public:
	tvmRestore()
	{
		stream.pop(*(std::string*)this);
		return true;
	}

	tvmSave()
	{
		stream.push(*(std::string*)this);
	}

public:
	using std::string::string;
};

//

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

		registerModule.insertMemoryEntry("string", ///< memory entry name
		                                 "string", ///< memory module name
		                                 &cLogicPrint::string); ///< store pointer

		registerModule.insertSignalExit("signal", ///< signal name
		                                &cLogicPrint::flow); ///< store flow id
	}

protected:
	tFlowId signalEntry()
	{
		if (string)
		{
			std::cout << (*string) << std::endl;
		}
		return flow;
	}

protected:
	const std::string* string;

protected:
	tFlowId flow;
};

//

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerMemoryModule<cMemoryString>("Hello!"); ///< constructor's arguments
	virtualMachine.registerRootModule<cRootStart>();
	virtualMachine.registerLogicModule<cLogicPrint>();
	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
