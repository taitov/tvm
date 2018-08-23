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
	const cLocker* rootStartLocker;
	if (engine.getRootModule(rootStart, rootStartLocker) != eResult::success)
	{
		return 4;
	}

	engine.run();

	if (rootStart->memoryExit) ///< @todo: redesign fileFormat?
	{
		engine.rootSignal(rootStart->rootSignalExit);
		engine.waitAllSchemes();

		rootStartLocker->lock();
		*rootStart->memoryExit = "Hello from main()";
		rootStartLocker->unlock();
		engine.rootSignal(rootStart->rootSignalExit);
		engine.waitAllSchemes();

		rootStartLocker->lock();
		*rootStart->memoryExit = "This is another string";
		rootStartLocker->unlock();
		engine.rootSignal(rootStart->rootSignalExit);
		engine.waitAllSchemes();
	}
	else
	{
		return 5;
	}

	return 0;
}
