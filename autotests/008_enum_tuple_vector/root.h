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
		moduleName = "start";

		rootSignalExits.insert("signal1",
		                       &cRootStart::rootSignalExit1);

		rootSignalExits.insert("signal2",
		                       &cRootStart::rootSignalExit2);

		rootSignalExits.insert("signal3",
		                       &cRootStart::rootSignalExit3);

		rootSignalExits.insert("signal4",
		                       &cRootStart::rootSignalExit4);
	}

public:
	tRootSignalExitId rootSignalExit1;
	tRootSignalExitId rootSignalExit2;
	tRootSignalExitId rootSignalExit3;
	tRootSignalExitId rootSignalExit4;
};

}

#endif // ROOT_H
