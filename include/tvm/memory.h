// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_MEMORY_H
#define TVM_MEMORY_H

#include "result.h"
#include "type.h"

namespace nVirtualMachine
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

#endif // TVM_MEMORY_H
