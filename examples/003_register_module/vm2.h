#ifndef VM2_H
#define VM2_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cMemoryString2 : public std::string ///< memory module
{
public:
	tvmRegisterMemoryModule()
	{
		registerModule.setModuleName("string2");
		registerModule.setMemoryAsString();
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(*(std::string*)this);
		return true;
	}

	void save(cStreamOut& stream)
	{
		stream.push(*(std::string*)this);
	}

public:
	using std::string::string;
};

class cRootStart2 ///< root module
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("start2");
		registerModule.setCaptionName("start");

		registerModule.insertRootSignalExit("signal", ///< signal name
		                                    &cRootStart2::rootSignalExit); ///< store signal id
	}

public:
	tRootSignalExitId rootSignalExit;
};

class cLogicPrint2 ///< logic module
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print2");
		registerModule.setCaptionName("print");

		registerModule.insertSignalEntry("signal", ///< signal name
		                                 &cLogicPrint2::signalEntry); ///< callback

		registerModule.insertMemoryEntry("memoryEntry", ///< memory entry name
		                                 "string", ///< memory module name
		                                 &cLogicPrint2::string); ///< store pointer
	}

protected:
	tFlowId signalEntry()
	{
		if (string)
		{
			std::cout << (*string) << std::endl;
		}
		return flowNowhere;
	}

protected:
	const cMemoryString2* string;
};

eResult registerModules2(cRegister& virtualMachine)
{
	/// register memory modules
	virtualMachine.registerMemoryModule<cMemoryString2>("Hello world 2"); ///< constructor's arguments

	/// register root modules
	virtualMachine.registerRootModule<cRootStart2>();

	/// register logic modules
	virtualMachine.registerLogicModule<cLogicPrint2>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM2_H
