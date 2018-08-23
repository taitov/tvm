// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include "result.h"
#include "type.h"

namespace tvm
{

class cMemoryModuleManager
{
public:
	virtual ~cMemoryModuleManager();

	virtual void* create(const std::vector<uint8_t>& value) = 0;

	virtual void destroy(void* pointer) = 0;
};

//

inline cMemoryModuleManager::~cMemoryModuleManager()
{
}

}
