// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include <inttypes.h>

namespace tvm
{

using tFlowId = uint32_t;
using tMemoryModuleId = uint32_t;
using tLogicModuleId = uint32_t;
using tMemoryModuleType = uint32_t;
using tRootModuleType = uint32_t;
using tLogicModuleType = uint32_t;
using tRootSignalExitId = uint32_t;
using tRootMemoryExitId = uint32_t;
using tSignalEntryId = uint32_t;
using tSignalExitId = uint32_t;
using tMemoryEntryId = uint32_t;
using tMemoryExitId = uint32_t;
using tMemoryEntryExitId = uint32_t;
using tSchemeId = uint32_t;
using tUniqueId = size_t;

class cConverterStringHelper;
using tCaptionName = cConverterStringHelper;
using tModuleName = cConverterStringHelper;
using tSignalEntryName = cConverterStringHelper;
using tMemoryEntryName = cConverterStringHelper;
using tSignalExitName = cConverterStringHelper;
using tMemoryExitName = cConverterStringHelper;
using tMemoryEntryExitName = cConverterStringHelper;
using tMemoryTypeName = cConverterStringHelper;

enum class eMemoryPosition : uint8_t
{
	scheme,
	global,
	root,
	constant,
};

}
