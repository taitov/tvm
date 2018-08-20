#ifndef ROOT_SECOND_START1_H
#define ROOT_SECOND_START1_H

#include <tvm/register.h>

namespace nVirtualMachine
{

class cRootSecondStart1
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("second_start_1");

		registerModule.insertRootSignalExit("exit_1",
		                                    &cRootSecondStart1::signalId);
	}

public:
	tRootSignalExitId signalId;
};

}

#endif // ROOT_SECOND_START1_H
