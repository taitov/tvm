// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include <stdio.h>
#include <endian.h>

#include <tuple>
#include <vector>

#include "type.h"

namespace tvm
{

namespace nFileFormat
{

constexpr uint32_t headerMagic = 0x74766d62;

using tMemoryModule = std::tuple<tMemoryModuleType,
                                 std::vector<uint8_t>>;

using tRootModule = std::tuple<tRootModuleType,
                               std::vector<std::tuple<tRootMemoryExitId,
                                                      tMemoryModule>>>;

using tLogicModule = std::tuple<tLogicModuleType>;

using tRootModuleSignalExits = std::tuple<tRootSignalExitId,
                                          tLogicModuleId,
                                          tSignalEntryId>;

using tLogicModuleSignalExits = std::tuple<tLogicModuleId,
                                           std::vector<std::tuple<tSignalExitId,
                                                                  tLogicModuleId,
                                                                  tSignalEntryId>>>;

using tLogicModuleMemories = std::tuple<tLogicModuleId,
                                        std::vector<std::tuple<tMemoryEntryId,
                                                               eMemoryPosition,
                                                               tMemoryModuleId>>, ///< memoryEntries
                                        std::vector<std::tuple<tMemoryExitId,
                                                               eMemoryPosition,
                                                               tMemoryModuleId>>, ///< memoryExits
                                        std::vector<std::tuple<tMemoryEntryExitId,
                                                               eMemoryPosition,
                                                               tMemoryModuleId>>>; ///< memoryEntryExits

using tScheme = std::tuple<std::vector<tMemoryModule>,
                           std::vector<tLogicModule>,
                           std::vector<tRootModuleSignalExits>,
                           std::vector<tLogicModuleSignalExits>,
                           std::vector<tLogicModuleMemories>>;

using tDebugInformation = std::vector<uint8_t>;

using tProject = std::tuple<std::vector<tRootModule>, ///< rootModules
                            std::vector<tMemoryModule>, ///< globalMemories
                            std::vector<tMemoryModule>, ///< constMemories
                            std::vector<tScheme>, ///< schemes
                            tDebugInformation>; ///< debugInformation

}

}
