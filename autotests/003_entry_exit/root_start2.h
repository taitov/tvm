#ifndef ROOT_START2_H
#define ROOT_START2_H

#include <tvm/register.h>

namespace nVirtualMachine
{

class cRootStart2
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("start_2");

		registerModule.insertRootSignalExit("exit_1",
		                                    &cRootStart2::signalId1);

		registerModule.insertRootSignalExit("exit_2",
		                                    &cRootStart2::signalId2);
	}

public:
	tRootSignalExitId signalId1;
	tRootSignalExitId signalId2;
};

}

#endif // ROOT_START2_H
