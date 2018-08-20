#include <iostream>

#include "logic_print1.h"

using namespace nVirtualMachine;

cLogicPrint1::cLogicPrint1(const std::string& string) :
        string4(string)
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_8_" << std::endl;
}

tFlowId cLogicPrint1::signalPrint1()
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_1_"
	          << *string1
	          << *string2
	          << *string3
	          << string4
	          << std::endl;
	return flow;
}

tFlowId cLogicPrint1::signalPrint2()
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_2_"
	          << *string1
	          << *string2
	          << *string3
	          << string4
	          << std::endl;
	return flowNowhere;
}
