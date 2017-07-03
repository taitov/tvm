#include <tvm/vm.h>

#include "virtualmachine.h"

using namespace nVirtualMachine;

int main(int argc, char** argv, char** envp)
{
	cVirtualMachine virtualMachine;

	if (!registerVirtualMachine(virtualMachine,
	                            argc,
	                            argv,
	                            envp))
	{
		return 1;
	}

	if (!virtualMachine.init())
	{
		return 2;
	}

	if (!virtualMachine.loadFromFile("prog.tvm"))
	{
		return 3;
	}

	virtualMachine.run();
	virtualMachine.wait();

	return 0;
}
