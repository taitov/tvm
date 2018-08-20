#include <iostream>

#include <tvm/engine.h>

#include "memory_string2.h"

using namespace nVirtualMachine;

bool cMemoryString2::restore(cStreamIn& stream)
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_10_" << std::endl;
	stream.pop(*(std::string*)this);
	return true;
}

void cMemoryString2::save(cStreamOut& stream) const
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_11_" << std::endl;
	stream.push(*(std::string*)this);
}

std::string cMemoryString2::functionOnlyForEngine() const
{
	return "_TEXT_ONLY_FOR_ENGINE_12_";
}
