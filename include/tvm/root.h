// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include "result.h"
#include "type.h"
#include "locker.h"

namespace nVirtualMachine
{

class cRootModuleManager
{
public:
	virtual ~cRootModuleManager();

	const cLocker* getLocker() const;

	virtual void initRootMemory() = 0;

	virtual eResult setRootMemoryExit(const tRootMemoryExitId& rootMemoryExitId,
	                                  void* memoryModulePointer) = 0;

	virtual tUniqueId getRootModuleUniqueId() const = 0;

	virtual const void* getRootModule() const = 0;

protected:
	cLocker locker;
};

//

inline cRootModuleManager::~cRootModuleManager()
{
}

inline const cLocker* cRootModuleManager::getLocker() const
{
	return &locker;
}

}
