// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include "result.h"
#include "type.h"

namespace tvm
{

class cCallbackBase
{
public:
	virtual ~cCallbackBase();

	virtual tFlowId call() = 0;
};

class cLogicModuleManager
{
public:
	virtual ~cLogicModuleManager();

	virtual void* create() = 0;

	virtual void destroy(void* modulePointer) = 0;

	virtual eResult getCallback(const tSignalEntryId& signalEntryId,
	                            void* modulePointer,
	                            cCallbackBase** callback) = 0;

	virtual eResult getFlowId(const tSignalExitId& signalExitId,
	                          void* modulePointer,
	                          tFlowId** flowId) = 0;

	virtual eResult getMemoryEntry(const tMemoryEntryId& memoryEntryId,
	                               void* modulePointer,
	                               const void*** memoryPointer) = 0;

	virtual eResult getMemoryExit(const tMemoryExitId& memoryExitId,
	                              void* modulePointer,
	                              void*** memoryPointer) = 0;

	virtual eResult getMemoryEntryExit(const tMemoryEntryExitId& memoryEntryExitId,
	                                   void* modulePointer,
	                                   void*** memoryPointer) = 0;
};

//

inline cLogicModuleManager::~cLogicModuleManager()
{
}

inline cCallbackBase::~cCallbackBase()
{
}

}
