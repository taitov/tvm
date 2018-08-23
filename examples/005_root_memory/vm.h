#ifndef VM_H
#define VM_H

#include <tvm/register.h>

namespace tvm
{

class cMemoryString : public std::string ///< memory module
{
public:
	tvmRegisterMemoryModule()
	{
		registerModule.setModuleName("string");
		registerModule.setMemoryAsValue("string");
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

class cRootStart ///< root module
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("start");
		registerModule.setCaptionName("start");

		registerModule.insertRootSignalExit("signal", ///< signal name
		                                    &cRootStart::rootSignalExit); ///< store signal id

		registerModule.insertRootMemoryExit("memoryExit", ///< memory exit name
		                                    "string", ///< memory module name
		                                    &cRootStart::memoryExit); ///< store pointer
	}

public:
	tRootSignalExitId rootSignalExit;

public:
	cMemoryString* memoryExit;
};

class cLogicPrint ///< logic module
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print");
		registerModule.setCaptionName("print");

		registerModule.insertSignalEntry("signal", ///< signal name
		                                 &cLogicPrint::signalEntryPrint); ///< callback

		registerModule.insertMemoryEntry("memoryEntry", ///< memory entry name
		                                 "string", ///< memory module name
		                                 &cLogicPrint::string); ///< store pointer
	}

protected:
	tFlowId signalEntryPrint()
	{
		if (string)
		{
			std::cout << (*string) << std::endl;
		}
		return flowNowhere;
	}

protected:
	const cMemoryString* string;
};

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerMemoryModule<cMemoryString>();
	virtualMachine.registerRootModule<cRootStart>();
	virtualMachine.registerLogicModule<cLogicPrint>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
