#ifndef VM_H
#define VM_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cMemoryString : public std::string ///< memory module
{
public:
	tvmRegisterMemoryModule()
	{
		registerModule.setModuleName("string");
		registerModule.setMemoryAsString();
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(*(std::string*)this);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(*(std::string*)this);
	}

public:
	using std::string::string;
};

class cMemoryVectorStrings : public std::vector<cMemoryString> ///< memory module
{
public:
	tvmRegisterMemoryModule()
	{
		registerModule.setModuleName("strings");
		registerModule.setMemoryAsVector("string");
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(*(std::vector<cMemoryString>*)this);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(*(std::vector<cMemoryString>*)this);
	}

public:
	using std::vector<cMemoryString>::vector;
};

class cRootStart ///< root module
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("start");

		registerModule.insertRootSignalExit("signal", ///< signal name
		                                    &cRootStart::rootSignalExit); ///< store signal id
	}

public:
	tRootSignalExitId rootSignalExit;
};

class cLogicPrint ///< logic module
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print");

		registerModule.insertSignalEntry("signal", ///< signal name
		                                 &cLogicPrint::signalEntryPrint); ///< callback

		registerModule.insertMemoryEntry("memoryEntry", ///< memory entry name
		                                 "strings", ///< memory module name
		                                 &cLogicPrint::strings); ///< store pointer
	}

protected:
	tFlowId signalEntryPrint()
	{
		if (strings)
		{
			for (const auto& string : *strings)
			{
				std::cout << string << std::endl;
			}
		}
		return flowNowhere;
	}

protected:
	const cMemoryVectorStrings* strings;
};

eResult registerModules(cRegister& virtualMachine)
{
	/// register memory modules
	virtualMachine.registerMemoryModule<cMemoryString>();
	virtualMachine.registerMemoryModule<cMemoryVectorStrings>();

	/// @todo: use registerMemoryModuleBase() for example

	/// register root modules
	virtualMachine.registerRootModule<cRootStart>();

	/// register logic modules
	virtualMachine.registerLogicModule<cLogicPrint>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
