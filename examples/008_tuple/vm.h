#ifndef VM_H
#define VM_H

#include <iostream>

#include <tvm/register.h>

namespace tvm
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

class cMemoryString : public std::string ///< memory module
{
public:
	tvmRegisterMemoryModule()
	{
		moduleName = "string";
		valueType.valueSetType("string");
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

class cMemoryTupleIntegerString : public std::tuple<cMemoryInteger, cMemoryString> ///< memory module
{
public:
	tvmRegisterMemoryModule()
	{
		moduleName = "keyValue";

		valueType.tupleInsertMemoryModule("key", "integer");
		valueType.tupleInsertMemoryModule("value", "string");
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(*(std::tuple<cMemoryInteger, cMemoryString>*)this);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(*(std::tuple<cMemoryInteger, cMemoryString>*)this);
	}

public:
	using std::tuple<cMemoryInteger, cMemoryString>::tuple;
};

class cRootStart ///< root module
{
public:
	tvmRegisterRootModule()
	{
		moduleName = "start";
		captionName = "start"; ///< for gui

		rootSignalExits.insert("signal1", ///< signal name
		                       &cRootStart::rootSignalExit1); ///< store signal id

		rootSignalExits.insert("signal2", ///< signal name
		                       &cRootStart::rootSignalExit2); ///< store signal id
	}

public:
	tRootSignalExitId rootSignalExit1;
	tRootSignalExitId rootSignalExit2;
};

class cLogicCopy ///< logic module
{
public:
	tvmRegisterLogicModule()
	{
		moduleName = "copy";
		captionName = "copy"; ///< for gui

		signalEntries.insert("signal", ///< signal name
		                     &cLogicCopy::signalEntryCopy); ///< callback

		memoryEntries.insert("string", ///< memory entry name
		                     "string", ///< memory module name
		                     &cLogicCopy::from); ///< store pointer

		memoryExits.insert("string", ///< memory entry name
		                   "string", ///< memory module name
		                   &cLogicCopy::to); ///< store pointer
	}

protected:
	tFlowId signalEntryCopy()
	{
		if (from && to)
		{
			*to = *from;
		}
		return flowNowhere;
	}

protected:
	const cMemoryString* from;
	cMemoryString* to;
};

class cLogicPrint ///< logic module
{
public:
	tvmRegisterLogicModule()
	{
		moduleName = "print";
		captionName = "print"; ///< for gui

		signalEntries.insert("signal", ///< signal name
		                     &cLogicPrint::signalEntryPrint); ///< callback

		memoryEntries.insert("keyValue", ///< memory entry name
		                     "keyValue", ///< memory module name
		                     &cLogicPrint::keyValue); ///< store pointer
	}

protected:
	tFlowId signalEntryPrint()
	{
		if (keyValue)
		{
			std::cout << std::get<0>(*keyValue).getValue()
			          << ": "
			          << std::get<1>(*keyValue)
			          << std::endl;
		}
		return flowNowhere;
	}

protected:
	const cMemoryTupleIntegerString* keyValue;
};

eResult registerModules(cRegister& virtualMachine)
{
	/// register memory modules
	virtualMachine.registerMemoryModule<cMemoryInteger>();
	virtualMachine.registerMemoryModule<cMemoryString>();
	virtualMachine.registerMemoryModule<cMemoryTupleIntegerString>();

	/// @todo: use registerMemoryModuleBase() for example

	/// register root modules
	virtualMachine.registerRootModule<cRootStart>();

	/// register logic modules
	virtualMachine.registerLogicModule<cLogicCopy>();
	virtualMachine.registerLogicModule<cLogicPrint>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
