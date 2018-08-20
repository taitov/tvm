#ifndef LOGIC_PRINT20_H
#define LOGIC_PRINT20_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cLogicPrint20
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print_2_0");

		registerModule.insertSignalEntry("entry_1",
		                                 &cLogicPrint20::signalEntryPrint1);

		registerModule.insertSignalEntry("entry_2",
		                                 &cLogicPrint20::signalEntryPrint2);
	}

public:
	cLogicPrint20()
	{
		id = staticId++;
	}

protected:
	tFlowId signalEntryPrint1()
	{
		checkString += std::to_string(id) + ":print_2_0:entry_1\n";
		return flowNowhere;
	}

	tFlowId signalEntryPrint2()
	{
		checkString += std::to_string(id) + ":print_2_0:entry_2\n";
		return flowNowhere;
	}

protected:
	unsigned int id;
};

}

#endif // LOGIC_PRINT20_H
