#ifndef LOGIC_PRINT33_H
#define LOGIC_PRINT33_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cLogicPrint33
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print_3_3");

		registerModule.insertSignalEntry("entry_1",
		                                 &cLogicPrint33::signalEntryPrint1);

		registerModule.insertSignalEntry("entry_2",
		                                 &cLogicPrint33::signalEntryPrint2);

		registerModule.insertSignalEntry("entry_3",
		                                 &cLogicPrint33::signalEntryPrint3);

		registerModule.insertSignalExit("exit_1",
		                                &cLogicPrint33::flow1);

		registerModule.insertSignalExit("exit_2",
		                                &cLogicPrint33::flow2);

		registerModule.insertSignalExit("exit_3",
		                                &cLogicPrint33::flow3);
	}

public:
	cLogicPrint33()
	{
		id = staticId++;
	}

protected:
	tFlowId signalEntryPrint1()
	{
		checkString += std::to_string(id) + ":print_3_3:entry_1\n";
		return flow1;
	}

	tFlowId signalEntryPrint2()
	{
		checkString += std::to_string(id) + ":print_3_3:entry_2\n";
		return flow2;
	}

	tFlowId signalEntryPrint3()
	{
		checkString += std::to_string(id) + ":print_3_3:entry_3\n";
		return flow3;
	}

protected:
	tFlowId flow1;
	tFlowId flow2;
	tFlowId flow3;

protected:
	unsigned int id;
};

}

#endif // LOGIC_PRINT33_H
