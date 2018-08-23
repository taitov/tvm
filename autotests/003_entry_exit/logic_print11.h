#ifndef LOGIC_PRINT11_H
#define LOGIC_PRINT11_H

#include <iostream>

#include <tvm/register.h>

namespace tvm
{

class cLogicPrint11
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print_1_1");

		registerModule.insertSignalEntry("entry_1",
		                                 &cLogicPrint11::signalEntryPrint);

		registerModule.insertSignalExit("exit_1",
		                                &cLogicPrint11::flow);
	}

public:
	cLogicPrint11()
	{
		id = staticId++;
	}

protected:
	tFlowId signalEntryPrint()
	{
		checkString += std::to_string(id) + ":print_1_1:entry_1\n";
		return flow;
	}

protected:
	tFlowId flow;

protected:
	unsigned int id;
};

}

#endif // LOGIC_PRINT11_H
