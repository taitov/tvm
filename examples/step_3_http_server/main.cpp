#include <tvm/vm.h>

#include "example.h"

using namespace nVirtualMachine;

int main(int argc, char** argv, char** envp)
{
	cVirtualMachine virtualMachine;

	if (!virtualMachine.registerLibraries(new nLibrary::cExample("0.0.0.0", 8080)))
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
