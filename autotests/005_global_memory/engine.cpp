#include <iostream>

#include <tvm/engine.h>
#include <tvm/converter/nlohmann/json.hpp>

#include "vm.h"

using namespace nVirtualMachine;

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "usage: " << argv[0] << " <program.tvm>" << " <autotest.json>" << std::endl;
		return 1;
	}

	cEngine engine;
	nlohmann::json jsonRoot;

	if (registerModules(engine) != eResult::success)
	{
		return 2;
	}

	if (engine.loadProjectFromFile(argv[1]) != eResult::success)
	{
		return 3;
	}

	const cRootStart* rootStart;
	const cRootPrintInteger* rootPrintInteger;
	if (engine.getRootModule(rootStart) != eResult::success ||
	    engine.getRootModule(rootPrintInteger) != eResult::success)
	{
		return 4;
	}

	{
		std::ifstream autotestFileStream(argv[2]);
		if (!autotestFileStream.is_open())
		{
			std::cerr << "can't open file '" << argv[2] << "'" << std::endl;
			return 3;
		}

		jsonRoot = nlohmann::json::parse(autotestFileStream);
	}

	unsigned int threadsCount = std::stoll(std::string(jsonRoot["autotest"]["threadsCount"]), 0, 0);
	unsigned int rootSignalsCount = std::stoll(std::string(jsonRoot["autotest"]["rootSignalsCount"]), 0, 0);
	std::string expectedString = jsonRoot["autotest"]["expectedString"];

	engine.run();

	checkString.clear();

	std::vector<std::thread> threads(threadsCount);
	for (unsigned int thread_i = 0;
	     thread_i < threads.size();
	     thread_i++)
	{
		threads[thread_i] = std::thread([&engine, &rootSignalsCount, &rootStart]
		{
			for (unsigned int i = 0; i < rootSignalsCount; i++)
			{
				engine.rootSignal(rootStart->signalId1);
			}

			/// @todo: remove waitAllSchemes()
			/// moodycamel queue is not linearizable
			engine.waitAllSchemes();
		});
	}
	for (unsigned int thread_i = 0;
	     thread_i < threads.size();
	     thread_i++)
	{
		threads[thread_i].join();
	}
	engine.waitAllSchemes();

	engine.rootSignal(rootPrintInteger->signalId);
	engine.waitAllSchemes();

	std::cout << checkString << std::endl;

	if (expectedString != checkString)
	{
		std::cerr << "expected string: '" << expectedString << "'" << std::endl;
		return 6;
	}

	return 0;
}
