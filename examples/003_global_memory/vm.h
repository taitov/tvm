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
	tvmRegisterMemoryModule(cMemoryInteger)
	{
		registerModule.setModuleName("integer");
		registerModule.setMemoryAsInteger32();
	}

public:
	tvmRestore()
	{
		stream.pop(value);
		return true;
	}

	tvmSave() const
	{
		stream.push(value);
	}

public:
	int32_t value;
};

//

class cRootStart ///< root module
{
public:
	tvmRegisterRootModule(cRootStart)
	{
		registerModule.setModuleName("start");
		registerModule.setCaptionName("start");

		registerModule.insertRootSignalExit("signal1", ///< signal name
		                                    &cRootStart::rootSignalExit1); ///< store signal id

		registerModule.insertRootSignalExit("signal2", ///< signal name
		                                    &cRootStart::rootSignalExit2); ///< store signal id
	}

public:
	tRootSignalExitId rootSignalExit1;
	tRootSignalExitId rootSignalExit2;
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

		registerModule.insertMemoryEntry("integer", ///< memory entry name
		                                 "integer", ///< memory module name
		                                 &cLogicPrint::integer); ///< store pointer

		registerModule.insertSignalExit("signal", ///< signal name
		                                &cLogicPrint::flow); ///< store flow id
	}

protected:
	tFlowId signalEntry()
	{
		if (integer)
		{
			std::cout << std::to_string(integer->value) << std::endl;
		}
		return flow;
	}

protected:
	const cMemoryInteger* integer;

protected:
	tFlowId flow;
};

//

class cLogicIncrement ///< logic module
{
public:
	tvmRegisterLogicModule(cLogicIncrement)
	{
		registerModule.setModuleName("increment");
		registerModule.setCaptionName("increment");

		registerModule.insertSignalEntry("signal", ///< signal name
		                                 &cLogicIncrement::signalEntry); ///< callback

		registerModule.insertMemoryEntryExit("integer", ///< memory entry name
		                                     "integer", ///< memory module name
		                                     &cLogicIncrement::integer); ///< store pointer

		registerModule.insertSignalExit("signal", ///< signal name
		                                &cLogicIncrement::flow); ///< store flow id
	}

protected:
	tFlowId signalEntry()
	{
		if (integer)
		{
			(integer->value)++;
		}
		return flow;
	}

protected:
	cMemoryInteger* integer;

protected:
	tFlowId flow;
};

//

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerMemoryModule<cMemoryInteger>();
	virtualMachine.registerRootModule<cRootStart>();
	virtualMachine.registerLogicModule<cLogicPrint>();
	virtualMachine.registerLogicModule<cLogicIncrement>();
	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
