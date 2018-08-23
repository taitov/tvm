#ifndef LOGIC_PRINT22_H
#define LOGIC_PRINT22_H

#include <iostream>

#include <tvm/register.h>

namespace tvm
{

class cLogicPrint22
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print_2_2");

		registerModule.insertSignalEntry("entry_1",
		                                 &cLogicPrint22::signalEntryPrint1);

		registerModule.insertSignalEntry("entry_2",
		                                 &cLogicPrint22::signalEntryPrint2);

		registerModule.insertSignalExit("exit_1",
		                                &cLogicPrint22::flow1);

		registerModule.insertSignalExit("exit_2",
		                                &cLogicPrint22::flow2);
	}

public:
	cLogicPrint22()
	{
		id = staticId++;
	}

protected:
	tFlowId signalEntryPrint1()
	{
		checkString += std::to_string(id) + ":print_2_2:entry_1\n";
		return flow1;
	}

	tFlowId signalEntryPrint2()
	{
		checkString += std::to_string(id) + ":print_2_2:entry_2\n";
		return flow2;
	}

protected:
	tFlowId flow1;
	tFlowId flow2;

protected:
	unsigned int id;
};

}

#endif // LOGIC_PRINT22_H
