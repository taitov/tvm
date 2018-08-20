#ifndef ROOT_START1_H
#define ROOT_START1_H

#include <tvm/register.h>

namespace nVirtualMachine
{

class cRootStart1
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("start_1");

		registerModule.insertRootSignalExit("exit_1",
		                                    &cRootStart1::signalId);
	}

public:
	tRootSignalExitId signalId;
};

}

#endif // ROOT_START1_H
