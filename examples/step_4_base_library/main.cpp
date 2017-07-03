#include <tvm/vm.h>
#include <tvm/library/base.h>
#include <tvm/library/console.h>

using namespace nVirtualMachine;

int main(int argc, char** argv, char** envp)
{
	cVirtualMachine virtualMachine;

	if (!virtualMachine.registerLibraries(new nLibrary::cBase(argc,
	                                                          argv,
	                                                          envp),
	                                      new nLibrary::cConsole()))
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
