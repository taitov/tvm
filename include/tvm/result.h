// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include "type.h"

namespace nVirtualMachine
{

enum class eResult : uint32_t
{
	success,
	errorAllocationMemory,
	errorCreateFile,
	alreadyExist,
	fileNotFound,
	invalidName,
	invalidFormat,
	invalidRootModule,
	invalidRootSignalExitId,
	invalidRootMemoryExitId,
	invalidSignalEntryId,
	invalidSignalExitId,
	invalidMemoryEntryId,
	invalidMemoryExitId,
	invalidMemoryEntryExitId,
	invalidModuleId,
	invalidModuleType,
	invalidMemoryPosition,
	invalidMemoryEntry,
	invalidMemoryExit,
	invalidMemoryEntryExit,
	invalidTargetType,
	unknownModuleName,
	unknownRootSignalExit,
	unknownRootMemoryExit,
	unknownSignalExit,
	unknownSignalEntry,
	unknownMemoryModuleType,
	unknownType,
	notImplemented,
};

}
