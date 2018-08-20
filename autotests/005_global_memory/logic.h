#ifndef LOGIC_H
#define LOGIC_H

#include <iostream>

#include <tvm/register.h>

namespace nVirtualMachine
{

std::string checkString;

class cLogicPrintInteger
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("printInteger");

		registerModule.insertSignalEntry("entry_1",
		                                 &cLogicPrintInteger::signal);

		registerModule.insertMemoryEntry("entry_1",
		                                 "integer",
		                                 &cLogicPrintInteger::integer);
	}

protected:
	tFlowId signal()
	{
		if (integer)
		{
			checkString += std::to_string(*integer);
		}
		return flowNowhere;
	}

protected:
	const int32_t* integer;
};

class cLogicIncrement
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("increment");

		registerModule.insertSignalEntry("entry_1",
		                                 &cLogicIncrement::signal);

		registerModule.insertSignalExit("exit_1",
		                                &cLogicIncrement::flow);

		registerModule.insertMemoryEntryExit("entryExit_1",
		                                     "integer",
		                                     &cLogicIncrement::integer);
	}

protected:
	tFlowId signal()
	{
		if (integer)
		{
			for (volatile unsigned int i = 0; i < 16; i++)
			{
				(*integer)++;
			}
		}
		return flow;
	}

protected:
	int32_t* integer;

protected:
	tFlowId flow;
};

}

#endif // LOGIC_H
