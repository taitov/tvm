// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#ifdef TVM_ENGINE
#error "include 'engine' or 'converter'"
#endif

#define TVM_CONVERTER

#include <vector>

#include "register.h"

namespace nVirtualMachine
{

class cConverter : public cRegister
{
public:
	virtual std::vector<uint8_t> getSpecification() const = 0;

	virtual eResult convertToBinary(const std::vector<uint8_t>& from,
	                                std::vector<uint8_t>& to) const = 0;

	virtual eResult convert(const std::string& fromFilePath,
	                        const std::string& toFilePath) const;
};

eResult cConverter::convert(TVM_UNUSED const std::string& fromFilePath,
                            TVM_UNUSED const std::string& toFilePath) const
{
	/// @todo
	return eResult::notImplemented;
}

}
