#include <iostream>

#include <tvm/engine.h>
#include <tvm/converter/nlohmann/json.hpp>

#include "vm.h"

using namespace nVirtualMachine;

std::map<std::tuple<std::string,
                    std::string>,
         tRootSignalExitId> rootSignalIds;

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

	const cRootStart1* rootStart1;
	const cRootStart2* rootStart2;
	const cRootSecondStart1* rootSecondStart1;
	const cRootSecondStart2* rootSecondStart2;
	if (engine.getRootModule(rootStart1) != eResult::success ||
	    engine.getRootModule(rootStart2) != eResult::success ||
	    engine.getRootModule(rootSecondStart1) != eResult::success ||
	    engine.getRootModule(rootSecondStart2) != eResult::success)
	{
		return 4;
	}

	rootSignalIds[{"start_1", "exit_1"}] = rootStart1->signalId;
	rootSignalIds[{"start_2", "exit_1"}] = rootStart2->signalId1;
	rootSignalIds[{"start_2", "exit_2"}] = rootStart2->signalId2;
	rootSignalIds[{"second_start_1", "exit_1"}] = rootSecondStart1->signalId;
	rootSignalIds[{"second_start_2", "exit_1"}] = rootSecondStart2->signalId1;
	rootSignalIds[{"second_start_2", "exit_2"}] = rootSecondStart2->signalId2;

	{
		std::ifstream autotestFileStream(argv[2]);
		if (!autotestFileStream.is_open())
		{
			std::cerr << "can't open file '" << argv[2] << "'" << std::endl;
			return 3;
		}

		jsonRoot = nlohmann::json::parse(autotestFileStream);
	}

	if (jsonRoot.find("autotest") == jsonRoot.end())
	{
		std::cerr << "empty autotest" << std::endl;
		return 7;
	}

	if (jsonRoot["autotest"].find("steps") == jsonRoot["autotest"].end())
	{
		std::cerr << "empty autotest" << std::endl;
		return 7;
	}

	engine.run();

	for (const nlohmann::json& jsonAutotestStep : jsonRoot["autotest"]["steps"])
	{
		std::string stepType = jsonAutotestStep["stepType"];
		if (stepType == "rootSignal")
		{
			std::string rootModuleName = jsonAutotestStep["rootModuleName"];
			std::string signalExitName = jsonAutotestStep["signalExitName"];
			std::string expectedString = jsonAutotestStep["expectedString"];

			const auto iter = rootSignalIds.find({rootModuleName, signalExitName});
			if (iter == rootSignalIds.end())
			{
				std::cerr << "unknown root signal '" << rootModuleName << "':'" << signalExitName << "'" << std::endl;
				return 4;
			}

			checkString.clear();
			engine.rootSignal(iter->second);
			engine.waitAllSchemes();

			std::cout << checkString << std::endl;

			if (expectedString != checkString)
			{
				std::cerr << "expected string: '" << expectedString << "'" << std::endl;
				return 6;
			}
		}
		else
		{
			std::cerr << "unknown step type '" << stepType << "'" << std::endl;
			return 5;
		}
	}

	return 0;
}
