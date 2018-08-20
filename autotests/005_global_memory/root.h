#ifndef ROOT_H
#define ROOT_H

#include <tvm/register.h>

namespace nVirtualMachine
{

class cRootStart
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("start");

		registerModule.insertRootSignalExit("exit_1",
		                                    &cRootStart::signalId1);
	}

public:
	tRootSignalExitId signalId1;
	tRootSignalExitId signalId2;
	tRootSignalExitId signalId3;
	tRootSignalExitId signalId4;
	tRootSignalExitId signalId5;
	tRootSignalExitId signalId6;
	tRootSignalExitId signalId7;
	tRootSignalExitId signalId8;
	tRootSignalExitId signalId9;
	tRootSignalExitId signalId10;
	tRootSignalExitId signalId11;
	tRootSignalExitId signalId12;
	tRootSignalExitId signalId13;
	tRootSignalExitId signalId14;
	tRootSignalExitId signalId15;
	tRootSignalExitId signalId16;
};

class cRootPrintInteger
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("printInteger");

		registerModule.insertRootSignalExit("exit_1",
		                                    &cRootPrintInteger::signalId);
	}

public:
	tRootSignalExitId signalId;
};

}

#endif // ROOT_H
