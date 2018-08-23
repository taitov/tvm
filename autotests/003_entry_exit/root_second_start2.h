#ifndef ROOT_SECOND_START2_H
#define ROOT_SECOND_START2_H

#include <tvm/register.h>

namespace tvm
{

class cRootSecondStart2
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("second_start_2");

		registerModule.insertRootSignalExit("exit_1",
		                                    &cRootSecondStart2::signalId1);

		registerModule.insertRootSignalExit("exit_2",
		                                    &cRootSecondStart2::signalId2);
	}

public:
	tRootSignalExitId signalId1;
	tRootSignalExitId signalId2;
};

}

#endif // ROOT_SECOND_START2_H
