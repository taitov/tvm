#ifndef LOGIC_PRINT10_H
#define LOGIC_PRINT10_H

#include <iostream>

#include <tvm/register.h>

namespace tvm
{

class cLogicPrint10
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print_1_0");

		registerModule.insertSignalEntry("entry_1",
		                                 &cLogicPrint10::signalEntryPrint);
	}

public:
	cLogicPrint10()
	{
		id = staticId++;
	}

protected:
	tFlowId signalEntryPrint()
	{
		checkString += std::to_string(id) + ":print_1_0:entry_1\n";
		return flowNowhere;
	}

protected:
	unsigned int id;
};

}

#endif // LOGIC_PRINT10_H
