#include <iostream>

#include <tvm/converter/json.h>

#include "vm.h"

using namespace tvm;

int main(int argc, char** argv)
{
	if (argc != 2 &&
	    argc != 3)
	{
		std::cout << "usage: " << argv[0] << " <from.json> <to.tvm>" << std::endl;
		std::cout << "       " << argv[0] << " getSpecification" << std::endl;
		return 1;
	}

	cConverterJson converter;

	if (registerModules(converter) != eResult::success)
	{
		return 2;
	}

	if (argc == 2)
	{
		const auto specification = converter.getSpecification();
		std::string specificationString(specification.begin(), specification.end());
		std::cout << specificationString << std::endl;
	}
	else if (argc == 3)
	{
		std::cout << "convert from '" << argv[1] << "' to '" << argv[2] << "'" << std::endl;

		if (converter.convert(argv[1], argv[2]) != eResult::success)
		{
			return 3;
		}

		std::cout << "success" << std::endl;
	}
	else
	{
		return 1;
	}

	return 0;
}
