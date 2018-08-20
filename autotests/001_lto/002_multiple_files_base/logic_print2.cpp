#include <iostream>

#include "logic_print2.h"

using namespace nVirtualMachine;

cLogicPrint2::cLogicPrint2(const std::string& string) :
        string4(string)
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_9_" << std::endl;
}

tFlowId cLogicPrint2::signalPrint1()
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_6_"
	          << *string1
	          << *string2
	          << *string3
	          << string4
	          << std::endl;
	return flow;
}

tFlowId cLogicPrint2::signalPrint2()
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_7_"
	          << *string1
	          << *string2
	          << *string3
	          << string4
	          << std::endl;
	return flowNowhere;
}
