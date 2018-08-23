#include <iostream>

#include <tvm/engine.h>

#include "vm.h"

using namespace tvm;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << " <program.tvm>" << std::endl;
		return 1;
	}

	cEngine engine;

	if (registerModules(engine) != eResult::success)
	{
		return 2;
	}

	if (engine.loadProjectFromFile(argv[1]) != eResult::success)
	{
		return 3;
	}

	const cRootStart* rootStart;
	if (engine.getRootModule(rootStart) != eResult::success)
	{
		return 4;
	}

	engine.run();

	engine.rootSignal(rootStart->signal);

	return 0;
}
