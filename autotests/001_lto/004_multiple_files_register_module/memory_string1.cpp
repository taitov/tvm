#include <iostream>

#include <tvm/engine.h>

#include "memory_string1.h"

using namespace nVirtualMachine;

bool cMemoryString1::restore(cStreamIn& stream)
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_7_" << std::endl;
	stream.pop(*(std::string*)this);
	return true;
}

void cMemoryString1::save(cStreamOut& stream) const
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_8_" << std::endl;
	stream.push(*(std::string*)this);
}

std::string cMemoryString1::functionOnlyForEngine() const
{
	return "_TEXT_ONLY_FOR_ENGINE_9_";
}
