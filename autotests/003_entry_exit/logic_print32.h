#ifndef LOGIC_PRINT32_H
#define LOGIC_PRINT32_H

#include <iostream>

#include <tvm/register.h>

namespace tvm
{

class cLogicPrint32
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print_3_2");

		registerModule.insertSignalEntry("entry_1",
		                                 &cLogicPrint32::signalEntryPrint1);

		registerModule.insertSignalEntry("entry_2",
		                                 &cLogicPrint32::signalEntryPrint2);

		registerModule.insertSignalEntry("entry_3",
		                                 &cLogicPrint32::signalEntryPrint3);

		registerModule.insertSignalExit("exit_1",
		                                &cLogicPrint32::flow1);

		registerModule.insertSignalExit("exit_2",
		                                &cLogicPrint32::flow2);
	}

public:
	cLogicPrint32()
	{
		id = staticId++;
	}

protected:
	tFlowId signalEntryPrint1()
	{
		checkString += std::to_string(id) + ":print_3_2:entry_1\n";
		return flow1;
	}

	tFlowId signalEntryPrint2()
	{
		checkString += std::to_string(id) + ":print_3_2:entry_2\n";
		return flow2;
	}

	tFlowId signalEntryPrint3()
	{
		checkString += std::to_string(id) + ":print_3_2:entry_3\n";
		return flowNowhere;
	}

protected:
	tFlowId flow1;
	tFlowId flow2;

protected:
	unsigned int id;
};

}

#endif // LOGIC_PRINT32_H
