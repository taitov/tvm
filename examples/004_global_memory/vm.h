#ifndef VM_H
#define VM_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cMemoryInteger ///< memory module
{
public:
	cMemoryInteger() :
	        value(0)
	{
	}

public:
	tvmRegisterMemoryModule()
	{
		moduleName = "integer";
		valueType.valueSetType("int32"); ///< @todo: define
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(value);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(value);
	}

public:
	inline const int32_t& getValue() const
	{
		return value;
	}

	inline int32_t& getValue()
	{
		return value;
	}

protected:
	int32_t value;
};

class cRootStart ///< root module
{
public:
	tvmRegisterRootModule()
	{
		moduleName = "start";
		captionName = "start";

		rootSignalExits.insert("signal1", ///< signal name
		                       &cRootStart::rootSignalExit1); ///< store signal id

		rootSignalExits.insert("signal2", ///< signal name
		                       &cRootStart::rootSignalExit2); ///< store signal id
	}

public:
	tRootSignalExitId rootSignalExit1;
	tRootSignalExitId rootSignalExit2;
};

class cLogicPrint ///< logic module
{
public:
	tvmRegisterLogicModule()
	{
		moduleName = "print";
		captionName = "print";

		signalEntries.insert("signal", ///< signal name
		                     &cLogicPrint::signalEntry); ///< callback

		memoryEntries.insert("memoryEntry", ///< memory entry name
		                     "integer", ///< memory module name
		                     &cLogicPrint::integer); ///< store pointer

		signalExits.insert("signal", ///< signal name
		                   &cLogicPrint::flow); ///< store flow id
	}

protected:
	tFlowId signalEntry()
	{
		if (integer)
		{
			std::cout << std::to_string(integer->getValue()) << std::endl;
		}
		return flow;
	}

protected:
	const cMemoryInteger* integer;

protected:
	tFlowId flow;
};

class cLogicIncrement ///< logic module
{
public:
	tvmRegisterLogicModule()
	{
		moduleName = "increment";
		captionName = "increment";

		signalEntries.insert("signal", ///< signal name
		                     &cLogicIncrement::signalEntry); ///< callback

		memoryEntryExits.insert("memoryEntryExit", ///< memory entry name
		                        "integer", ///< memory module name
		                        &cLogicIncrement::integer); ///< store pointer

		signalExits.insert("signal", ///< signal name
		                   &cLogicIncrement::flow); ///< store flow id
	}

protected:
	tFlowId signalEntry()
	{
		if (integer)
		{
			(integer->getValue())++;
		}
		return flow;
	}

protected:
	cMemoryInteger* integer;

protected:
	tFlowId flow;
};

eResult registerModules(cRegister& virtualMachine)
{
	/// register memory modules
	virtualMachine.registerMemoryModule<cMemoryInteger>();

	/// register root modules
	virtualMachine.registerRootModule<cRootStart>();

	/// register logic modules
	virtualMachine.registerLogicModule<cLogicPrint>();
	virtualMachine.registerLogicModule<cLogicIncrement>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
