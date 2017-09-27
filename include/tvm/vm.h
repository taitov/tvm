// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_VM_H
#define TVM_VM_H

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <iostream>
#include <fstream>
#include <mutex>

#include <string.h>

#include "type.h"
#include "root.h"
#include "action.h"
#include "logic.h"
#include "memory.h"
#include "signal.h"
#include "scheme.h"
#include "stream.h"
#include "library.h"

namespace nVirtualMachine
{

constexpr uint32_t fileHeaderMagic = 0x6d766674;

class cVirtualMachine
{
	friend class cLibrary;
	friend class cScheme;
	friend class cActionModule;

	template<typename TObject>
	friend class cSignalEntryObject;

public:
	using tBoolean = bool;
	const tMemoryTypeName memoryBooleanTypeName = "boolean";

	using tInteger = int64_t;
	const tMemoryTypeName memoryIntegerTypeName = "integer";

	using tBuffer = std::vector<uint8_t>;
	const tMemoryTypeName memoryBufferTypeName = "buffer";

	using tString = std::string;
	const tMemoryTypeName memoryStringTypeName = "string";

public:
	cVirtualMachine();
	~cVirtualMachine();

	template<typename TType>
	bool registerLibraries(TType* library)
	{
		return registerLibrary(library);
	}

	template<typename THeadType,
	         typename ... TTypes>
	bool registerLibraries(THeadType* library, TTypes* ... libraries)
	{
		if (!registerLibrary(library))
		{
			return false;
		}
		return registerLibraries<TTypes ...>(libraries ...);
	}

	bool registerLibrary(cLibrary* library);

	bool registerMemoryModule(const tMemoryTypeName& memoryTypeName,
	                          cModule* module);

	void unregisterLibraries();

	bool init();

	bool loadFromFile(const std::string& filePath);
	bool loadFromFile(const tProjectName& projectName,
	                  const std::string& filePath);
	bool loadFromMemory(const tProjectName& projectName,
	                    const std::vector<uint8_t>& buffer);
	void unload(const tProjectName& projectName);
	void unloadAll();

	void run();
	void wait();
	void stop();

public: /** gui */
	using tGuiMemoryTypes = std::map<tMemoryTypeName,
	                                 cMemory*>;

	using tGuiRootModules = std::map<std::tuple<tLibraryName,
	                                            tRootModuleName>,
	                                 std::tuple<std::vector<tSignalExitName>,
	                                            std::vector<std::tuple<tMemoryExitName,
	                                                                   tMemoryTypeName>>>>;

	using tGuiModules = std::map<std::tuple<tLibraryName,
	                                        tModuleName>,
	                             cModule*>;

	using tGuiMemoryModules = std::map<std::tuple<tMemoryTypeName,
	                                              tModuleName>,
	                                   cModule*>;


	const tGuiMemoryTypes getGuiMemoryTypes() const;
	const tGuiRootModules getGuiRootModules() const;
	const tGuiModules getGuiModules() const;
	const tGuiMemoryModules getGuiMemoryModules() const;

public: /** exec */
	inline void rootSignalFlow(tRootSignalExitId rootSignalExitId);

	template<typename TType>
	inline void rootSetMemory(tRootMemoryExitId rootMemoryExitId, const TType& value);

	inline bool isStopped() const;

public:
	template<typename TType>
	bool registerMemory(const tMemoryTypeName& memoryTypeName)
	{
		if (memoryTypes.find(memoryTypeName) != memoryTypes.end())
		{
			return false;
		}

		memoryTypes[memoryTypeName] = new cMemoryVariable<TType>();
		return true;
	}

	template<typename TType>
	bool registerMemory(const tMemoryTypeName& memoryTypeName, const TType& value)
	{
		if (memoryTypes.find(memoryTypeName) != memoryTypes.end())
		{
			return false;
		}

		memoryTypes[memoryTypeName] = new cMemoryVariable<TType>(value);
		return true;
	}

	template<typename TType>
	bool registerMemoryStandart(const tMemoryTypeName& memoryTypeName)
	{
		if (memoryTypes.find(memoryTypeName) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModuleStandart<TType,
		                                  tBoolean>(memoryTypeName,
		                                            memoryBooleanTypeName))
		{
			return false;
		}

		memoryTypes[memoryTypeName] = new cMemoryVariable<TType>();
		return true;
	}

	template<typename TType>
	bool registerMemoryStandart(const tMemoryTypeName& memoryTypeName,
	                            const TType& value)
	{
		if (memoryTypes.find(memoryTypeName) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModuleStandart<TType,
		                                  tBoolean>(memoryTypeName,
		                                            memoryBooleanTypeName))
		{
			return false;
		}

		memoryTypes[memoryTypeName] = new cMemoryVariable<TType>(value);
		return true;
	}

	template<typename TType,
	         std::size_t TSize>
	bool registerMemoryArray(const tMemoryTypeName& memoryTypeName)
	{
		tMemoryTypeName memoryTypeNameArray = "array<" + memoryTypeName.value + "," + std::to_string(TSize) + ">";
		return registerMemoryArray<TType, TSize>(memoryTypeNameArray, memoryTypeName);
	}

	template<typename TType,
	         std::size_t TSize>
	bool registerMemoryArray(const tMemoryTypeName& memoryTypeNameArray,
	                         const tMemoryTypeName& memoryTypeName)
	{
		using tArray = std::array<TType, TSize>;

		if (memoryTypes.find(memoryTypeNameArray) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameArray,
		                          new cLogicCopy<tArray>(memoryTypeNameArray)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameArray,
		                          new cLogicSize<tArray,
		                                         tInteger>("getCount",
		                                                   memoryTypeNameArray,
		                                                   memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameArray,
		                          new cLogicArrayGet<TType,
		                                             TSize,
		                                             tInteger>(memoryTypeNameArray,
		                                                       memoryTypeName,
		                                                       memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameArray,
		                          new cLogicArraySet<TType,
		                                             TSize,
		                                             tInteger>(memoryTypeNameArray,
		                                                       memoryTypeName,
		                                                       memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameArray,
		                          new cLogicArrayForEach<TType,
		                                                 TSize>(memoryTypeNameArray,
		                                                        memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameArray,
		                          new cLogicIfEqual<tArray,
		                                            tBoolean>(memoryTypeNameArray,
		                                                          memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameArray, new cLogicConvert<tArray,
		                                                                 tBuffer>("toBuffer",
		                                                                          memoryTypeNameArray,
		                                                                          memoryBufferTypeName,
			[](tArray* from, tBuffer* to)
			{
				cStreamOut stream;
				stream.push(*from);
				*to = stream.getBuffer();
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameArray, new cLogicConvertBool<tBuffer,
		                                                                     tArray>("fromBuffer",
		                                                                             memoryBufferTypeName,
		                                                                             memoryTypeNameArray,
			[](tBuffer* from, tArray* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
				if (stream.isFailed())
				{
					return false;
				}
				return true;
			})))
		{
			return false;
		}

		memoryTypes[memoryTypeNameArray] = new cMemoryVariable<tArray>();
		return true;
	}

	template<typename TType>
	bool registerMemoryVector(const tMemoryTypeName& memoryTypeName)
	{
		using tVector = std::vector<TType>;
		tMemoryTypeName memoryTypeNameVector = "vector<" + memoryTypeName.value + ">";

		if (memoryTypes.find(memoryTypeNameVector) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicCopy<tVector>(memoryTypeNameVector)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicIsEmpty<tVector,
		                                            tBoolean>(memoryTypeNameVector,
		                                                      memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicSetClear<tVector>(memoryTypeNameVector)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicVectorPushBack<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicVectorPopBack<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicSize<tVector,
		                                         tInteger>("getCount",
		                                                   memoryTypeNameVector,
		                                                   memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicVectorGet<TType,
		                                              tInteger>(memoryTypeName,
		                                                        memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicVectorSet<TType,
		                                              tInteger>(memoryTypeName,
		                                                        memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicVectorForEach<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector, new cLogicConvert<tVector,
		                                                                  tBuffer>("toBuffer",
		                                                                           memoryTypeNameVector,
		                                                                           memoryBufferTypeName,
			[](tVector* from, tBuffer* to)
			{
				cStreamOut stream;
				stream.push(*from);
				*to = stream.getBuffer();
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector, new cLogicConvertBool<tBuffer,
		                                                                      tVector>("fromBuffer",
		                                                                               memoryBufferTypeName,
		                                                                               memoryTypeNameVector,
			[](tBuffer* from, tVector* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
				if (stream.isFailed())
				{
					return false;
				}
				return true;
			})))
		{
			return false;
		}

		memoryTypes[memoryTypeNameVector] = new cMemoryVariable<tVector>();
		return true;
	}

	template<typename TKeyType,
	         typename TValueType>
	bool registerMemoryMap(const tMemoryTypeName& memoryKeyTypeName,
	                       const tMemoryTypeName& memoryValueTypeName)
	{
		using tMap = std::map<TKeyType, TValueType>;
		tMemoryTypeName memoryTypeNameMap = "map<" + memoryKeyTypeName.value + "," + memoryValueTypeName.value + ">";

		if (memoryTypes.find(memoryTypeNameMap) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicCopy<tMap>(memoryTypeNameMap)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicSetClear<tMap>(memoryTypeNameMap)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicIsEmpty<tMap,
		                                            tBoolean>(memoryTypeNameMap,
		                                                      memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicMapInsertOrUpdate<TKeyType,
		                                                      TValueType>(memoryKeyTypeName,
		                                                                  memoryValueTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicMapFind<TKeyType,
		                                            TValueType>(memoryKeyTypeName,
		                                                        memoryValueTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicMapSelectOrCreate<TKeyType,
		                                                      TValueType>(memoryKeyTypeName,
		                                                                  memoryValueTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicSize<tMap,
		                                         tInteger>("getCount",
		                                                   memoryTypeNameMap,
		                                                   memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicMapGet<TKeyType,
		                                           TValueType>(memoryKeyTypeName,
		                                                       memoryValueTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap,
		                          new cLogicMapForEach<TKeyType,
		                                               TValueType>(memoryKeyTypeName,
		                                                           memoryValueTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap, new cLogicConvert<tMap,
		                                                               tBuffer>("toBuffer",
		                                                                        memoryTypeNameMap,
		                                                                        memoryBufferTypeName,
			[](tMap* from, tBuffer* to)
			{
				cStreamOut stream;
				stream.push(*from);
				*to = stream.getBuffer();
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameMap, new cLogicConvertBool<tBuffer,
		                                                                   tMap>("fromBuffer",
		                                                                         memoryBufferTypeName,
		                                                                         memoryTypeNameMap,
			[](tBuffer* from, tMap* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
				if (stream.isFailed())
				{
					return false;
				}
				return true;
			})))
		{
			return false;
		}

		memoryTypes[memoryTypeNameMap] = new cMemoryVariable<tMap>();
		return true;
	}

	template<typename TTuple>
	bool registerMemoryTuple(const std::vector<std::pair<tMemoryName, tMemoryTypeName>>& memories)
	{
		tMemoryTypeName memoryTypeNameTuple = "tuple<";
		if (memories.size())
		{
			for (unsigned int memoryTypeName_i = 0; memoryTypeName_i < memories.size() - 1; memoryTypeName_i++)
			{
				memoryTypeNameTuple.value += memories[memoryTypeName_i].first.value + ",";
			}
			for (unsigned int memoryTypeName_i = memories.size() - 1; memoryTypeName_i < memories.size(); memoryTypeName_i++)
			{
				memoryTypeNameTuple.value += memories[memoryTypeName_i].first.value;
			}
		}
		memoryTypeNameTuple.value += ">";
		return registerMemoryTuple<TTuple>(memoryTypeNameTuple,
		                                   memories);
	}

	template<typename TTuple>
	bool registerMemoryTuple(const tMemoryTypeName& memoryTypeNameTuple,
	                         const std::vector<std::pair<tMemoryName, tMemoryTypeName>>& memories)
	{
		TTuple* helper = nullptr;

		std::vector<tMemoryName> memoryNames;
		std::vector<tMemoryTypeName> memoryTypeNames;

		for (const auto& pair : memories)
		{
			memoryNames.push_back(pair.first);
			memoryTypeNames.push_back(pair.second);
		}

		return registerMemoryTupleHelper(memoryTypeNameTuple,
		                                 memoryNames,
		                                 memoryTypeNames,
		                                 helper);
	}

	template<typename TEnum>
	bool registerMemoryEnum(tMemoryTypeName memoryTypeNameEnum,
	                        const std::vector<std::pair<tMemoryName, TEnum>>& items)
	{
		if (memoryTypes.find(memoryTypeNameEnum) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModuleStandart<TEnum,
		                                  tBoolean>(memoryTypeNameEnum,
		                                            memoryBooleanTypeName))
		{
			return false;
		}

		std::map<tMemoryName, int64_t>& mapEnums = enums[memoryTypeNameEnum];
		for (const auto& iter : items)
		{
			mapEnums[iter.first] = (int64_t)iter.second;
		}

		if (!registerMemoryModule(memoryTypeNameEnum, new cLogicConvert<TEnum,
		                                                                tString>("toString",
		                                                                         memoryTypeNameEnum,
		                                                                         memoryStringTypeName,
			[this, memoryTypeNameEnum](TEnum* from, tString* to)
			{
				const std::map<tMemoryName, int64_t>& mapEnums = enums[memoryTypeNameEnum];
				for (const auto& iter : mapEnums)
				{
					if (iter.second == (int64_t)*from)
					{
						*to = iter.first;
						return;
					}
				}
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameEnum, new cLogicConvertBool<tString,
		                                                                    TEnum>("fromString",
		                                                                           memoryStringTypeName,
		                                                                           memoryTypeNameEnum,
			[this, memoryTypeNameEnum](tString* from, TEnum* to)
			{
				const std::map<tMemoryName, int64_t>& mapEnums = enums[memoryTypeNameEnum];
				if (mapEnums.find(*from) == mapEnums.end())
				{
					return false;
				}
				*to = (TEnum)mapEnums.find(*from)->second;
				return true;
			})))
		{
			return false;
		}

		memoryTypes[memoryTypeNameEnum] = new cMemoryVariable<TEnum>();
		return true;
	}

	bool registerRootSignalExit(const tLibraryName& libraryName,
	                            const tRootModuleName& rootModuleName,
	                            const tSignalExitName& signalExitName,
	                            tRootSignalExitId& rootSignalExitId);

	bool registerRootMemoryExit(const tLibraryName& libraryName,
	                            const tRootModuleName& rootModuleName,
	                            const tMemoryExitName& memoryExitName,
	                            const tMemoryTypeName& memoryTypeName,
	                            tRootMemoryExitId& rootMemoryExitId);

private:
	template<typename TType,
	         typename TBooleanType>
	bool registerMemoryModuleStandart(const tMemoryTypeName& memoryTypeName,
	                                  const tMemoryTypeName& memoryBooleanTypeName)
	{
		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicCopy<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicAddition<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicSubtraction<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicMultiplication<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicDivision<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicModulo<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicIncrement<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicDecrement<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicIfEqual<TType,
		                                            TBooleanType>(memoryTypeName,
		                                                          memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicIfGreater<TType,
		                                              TBooleanType>(memoryTypeName,
		                                                            memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicIfLess<TType,
		                                           TBooleanType>(memoryTypeName,
		                                                         memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicIfGreaterOrEqual<TType,
		                                                     TBooleanType>(memoryTypeName,
		                                                                   memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicIfLessOrEqual<TType,
		                                                  TBooleanType>(memoryTypeName,
		                                                                memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicBitwiseNot<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicBitwiseAnd<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicBitwiseOr<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicBitwiseXor<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicBitwiseLeftShift<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName,
		                          new cLogicBitwiseRightShift<TType>(memoryTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName, new cLogicConvert<TType,
		                                                            tBuffer>("toBuffer",
		                                                                     memoryTypeName,
		                                                                     memoryBufferTypeName,
			[](TType* from, tBuffer* to)
			{
				cStreamOut stream;
				stream.push(*from);
				*to = stream.getBuffer();
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeName, new cLogicConvertBool<tBuffer,
		                                                                TType>("fromBuffer",
		                                                                       memoryBufferTypeName,
		                                                                       memoryTypeName,
			[](tBuffer* from, TType* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
				if (stream.isFailed())
				{
					return false;
				}
				return true;
			})))
		{
			return false;
		}

		return true;
	}

	template<typename ... TTypes>
	bool registerMemoryTupleHelper(const tMemoryTypeName& memoryTypeNameTuple,
	                               const std::vector<tMemoryName>& memoryNames,
	                               const std::vector<tMemoryTypeName>& memoryTypeNames,
	                               std::tuple<TTypes ...>*)
	{
		using tTuple = std::tuple<TTypes ...>;

		if (sizeof...(TTypes) != memoryNames.size())
		{
			return false;
		}

		if (sizeof...(TTypes) != memoryTypeNames.size())
		{
			return false;
		}

		if (memoryTypes.find(memoryTypeNameTuple) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameTuple,
		                          new cLogicCopy<tTuple>(memoryTypeNameTuple)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameTuple,
		                          new cLogicIfEqual<tTuple,
		                                            tBoolean>(memoryTypeNameTuple,
		                                                      memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameTuple,
		                          new cLogicTupleGet<TTypes ...>(memoryTypeNameTuple,
		                                                         memoryNames,
		                                                         memoryTypeNames)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameTuple,
		                          new cLogicTupleSet<TTypes ...>(memoryTypeNameTuple,
		                                                         memoryNames,
		                                                         memoryTypeNames)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameTuple, new cLogicConvert<tTuple,
		                                                                 tBuffer>("toBuffer",
		                                                                          memoryTypeNameTuple,
		                                                                          memoryBufferTypeName,
			[](tTuple* from, tBuffer* to)
			{
				cStreamOut stream;
				stream.push(*from);
				*to = stream.getBuffer();
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameTuple, new cLogicConvertBool<tBuffer,
		                                                                     tTuple>("fromBuffer",
		                                                                             memoryBufferTypeName,
		                                                                             memoryTypeNameTuple,
			[](tBuffer* from, tTuple* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
				if (stream.isFailed())
				{
					return false;
				}
				return true;
			})))
		{
			return false;
		}

		memoryTypes[memoryTypeNameTuple] = new cMemoryVariable<tTuple>();
		return true;
	}

	void registerBuildInLibrary();

private: /** load */
	using tSchemes = std::map<tSchemeName,
	                          cScheme*>;

	bool readScheme(cStreamIn& stream,
	                tSchemes& schemes);

	void freeSchemes(tSchemes& schemes);

private:
	using tMemoryTypes = std::map<tMemoryTypeName,
	                              cMemory*>;

	using tEnums = std::map<tMemoryTypeName,
	                        std::map<tMemoryName, int64_t>>;

	using tModules = std::map<std::tuple<tLibraryName,
	                                     tModuleName>,
	                          cModule*>;

	using tRootSignalExits = std::map<std::tuple<tLibraryName,
	                                             tRootModuleName,
	                                             tSignalExitName>,
	                                  tRootSignalExitId>;

	using tRootMemoryExits = std::map<std::tuple<tLibraryName,
	                                             tRootModuleName,
	                                             tMemoryExitName>,
	                                  std::tuple<tMemoryTypeName,
	                                             tRootMemoryExitId>>;

	const tMemoryTypes getMemoryTypes() const;
	const tModules getModules() const;
	const tRootSignalExits getRootSignalExits() const;
	const tRootMemoryExits getRootMemoryExits() const;

private:
	using tLibraries = std::map<tLibraryName,
	                            cLibrary*>;

	using tMemoryModules = std::map<std::tuple<tMemoryTypeName,
	                                           tModuleName>,
	                                cModule*>;

	tLibraries libraries;
	tMemoryTypes memoryTypes;
	tMemoryModules memoryModules;
	tRootSignalExits rootSignalExits;
	tRootMemoryExits rootMemoryExits;
	tEnums enums;

private: /** load */
	std::map<tProjectName,
	         std::tuple<tProjectId, ///< projectId
	                    cScheme* ///< mainScheme
	                   >> projects;

private: /** exec */
	volatile bool stopped;
	std::map<tProjectId, cScheme*> currentSchemes;
	std::mutex mutex; /**< @todo: mutex perProject */
	tRootSignalExitId rootSignalSchemeLoaded;
	tRootSignalExitId rootSignalSchemeUnload;
};

inline cVirtualMachine::cVirtualMachine()
{
	stopped = false;
	registerBuildInLibrary();
}

inline cVirtualMachine::~cVirtualMachine()
{
	stop();
	wait();
	unloadAll();
	unregisterLibraries();
}

inline void cVirtualMachine::unregisterLibraries()
{
	for (auto& iter : libraries)
	{
		delete iter.second;
	}
	libraries.clear();
	rootSignalExits.clear();
	rootMemoryExits.clear();
	for (auto& iter : memoryTypes)
	{
		delete iter.second;
	}
	memoryTypes.clear();
	for (auto& memoryModule : memoryModules)
	{
		delete memoryModule.second;
	}
	memoryModules.clear();
	enums.clear();

	registerBuildInLibrary();
}

inline bool cVirtualMachine::init()
{
	for (auto& iter : libraries)
	{
		if (!iter.second->init())
		{
			return false;
		}
	}

	return true;
}

inline bool cVirtualMachine::loadFromFile(const std::string& filePath)
{
	return loadFromFile(basename(filePath.c_str()),
	                    filePath);
}

inline bool cVirtualMachine::loadFromFile(const tProjectName& projectName,
                                          const std::string& filePath)
{
	std::ifstream fileStream(filePath, std::ifstream::binary);
	if (!fileStream.is_open())
	{
		return false;
	}

	std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(fileStream)),
	                            std::istreambuf_iterator<char>());

	return loadFromMemory(projectName, buffer);
}

inline bool cVirtualMachine::loadFromMemory(const tProjectName& projectName,
                                            const std::vector<uint8_t>& buffer)
{
	std::lock_guard<std::mutex> guard(mutex);

	if (projects.find(projectName) != projects.end())
	{
		return false;
	}

	cStreamIn stream(buffer);

	uint32_t magic = 0;
	stream.pop(magic);
	if (magic != fileHeaderMagic)
	{
		return false;
	}

	tSchemes schemes; /**< @todo: rewrite code */
	uint32_t schemesCount = 0;
	stream.pop(schemesCount);
	for (uint64_t scheme_i = 0; scheme_i < schemesCount; scheme_i++)
	{
		if (!readScheme(stream, schemes))
		{
			freeSchemes(schemes);
			return false;
		}
	}

	if (stream.isFailed())
	{
		freeSchemes(schemes);
		return false;
	}

	if (schemes.find("main") == schemes.end())
	{
		freeSchemes(schemes);
		return false;
	}

	tProjectId projectId = currentSchemes.size() + 1;

	cScheme* mainScheme = schemes["main"]->clone();
	if (!mainScheme->init(schemes, projectId))
	{
		delete mainScheme;
		freeSchemes(schemes);
		return false;
	}

	freeSchemes(schemes);

	projects[projectName] = std::make_tuple(projectId,
	                                        mainScheme);

	currentSchemes[projectId] = mainScheme;

	mainScheme->rootSignalFlow(rootSignalSchemeLoaded);

	return true;
}

inline void cVirtualMachine::unload(const tProjectName& projectName)
{
	std::lock_guard<std::mutex> guard(mutex);

	if (projects.find(projectName) == projects.end())
	{
		return;
	}

	const tProjectId& projectId = std::get<0>(projects[projectName]);
	cScheme* mainScheme = std::get<1>(projects[projectName]);

	currentSchemes[projectId]->rootSignalFlow(rootSignalSchemeUnload);

	delete mainScheme;

	currentSchemes.erase(projectId);
	projects.erase(projectName);
}

inline void cVirtualMachine::unloadAll()
{
	std::lock_guard<std::mutex> guard(mutex);

	for (auto& currentSchemeIter : currentSchemes)
	{
		cScheme* currentScheme = currentSchemeIter.second;
		currentScheme->rootSignalFlow(rootSignalSchemeUnload);
	}

	for (auto& project : projects)
	{
		cScheme* mainScheme = std::get<1>(project.second);
		delete mainScheme;
	}

	currentSchemes.clear();
	projects.clear();
}

inline void cVirtualMachine::run()
{
	stopped = false;

	for (auto& iter : libraries)
	{
		iter.second->doRun();
	}
}

inline void cVirtualMachine::wait()
{
	for (auto& iter : libraries)
	{
		iter.second->wait();
	}
}

inline void cVirtualMachine::stop()
{
	if (!stopped)
	{
		stopped = true;

		{
			std::lock_guard<std::mutex> guard(mutex);
			for (auto& iter : libraries)
			{
				iter.second->stop();
			}
		}
	}
}

inline const cVirtualMachine::tGuiMemoryTypes cVirtualMachine::getGuiMemoryTypes() const
{
	tGuiMemoryTypes guiMemoryTypes;

	for (auto& iter : memoryTypes)
	{
		guiMemoryTypes[iter.first] = iter.second;
	}

	return guiMemoryTypes;
}

inline const cVirtualMachine::tGuiRootModules cVirtualMachine::getGuiRootModules() const
{
	tGuiRootModules guiRootModules;

	for (auto& iter : rootSignalExits)
	{
		auto key = std::make_tuple(std::get<0>(iter.first), std::get<1>(iter.first));
		auto value = std::get<2>(iter.first);

		std::get<0>(guiRootModules[key]).push_back(value);
	}

	for (auto& iter : rootMemoryExits)
	{
		auto key = std::make_tuple(std::get<0>(iter.first), std::get<1>(iter.first));
		auto value = std::make_tuple(std::get<2>(iter.first), std::get<0>(iter.second));

		std::get<1>(guiRootModules[key]).push_back(value);
	}

	return guiRootModules;
}

inline const cVirtualMachine::tGuiModules cVirtualMachine::getGuiModules() const
{
	tGuiModules guiModules;

	for (const auto& iterLibrary : libraries)
	{
		for (const auto& iterModule : iterLibrary.second->getModules())
		{
			const auto key = std::make_tuple(iterLibrary.first,
			                                 iterModule.first);
			guiModules[key] = iterModule.second;
		}
	}

	return guiModules;
}

inline const cVirtualMachine::tGuiMemoryModules cVirtualMachine::getGuiMemoryModules() const
{
	tGuiMemoryModules guiMemoryModules;

	for (const auto& iterModule : memoryModules)
	{
		const auto key = std::make_tuple(std::get<0>(iterModule.first).value,
		                                 std::get<1>(iterModule.first));
		guiMemoryModules[key] = iterModule.second;
	}

	return guiMemoryModules;
}

inline void cVirtualMachine::rootSignalFlow(tRootSignalExitId rootSignalExitId)
{
	std::lock_guard<std::mutex> guard(mutex);
	for (const auto& currentSchemeIter : currentSchemes)
	{
		cScheme* currentScheme = currentSchemeIter.second;
		currentScheme->rootSignalFlow(rootSignalExitId);
	}
}

inline bool cVirtualMachine::isStopped() const
{
	return stopped;
}

inline bool cVirtualMachine::registerLibrary(cLibrary* library)
{
	if (!library->doRegisterLibrary(this))
	{
		delete library;
		return false;
	}

	if (library->getLibraryName().value.find(':') != std::string::npos)
	{
		delete library;
		return false;
	}

	if (libraries.find(library->getLibraryName()) != libraries.end())
	{
		delete library;
		return false;
	}

	libraries[library->getLibraryName()] = library;

	return true;
}

inline bool cVirtualMachine::registerMemoryModule(const tMemoryTypeName& memoryTypeName,
                                                  cModule* module)
{
	if (!module->doRegisterModule(this))
	{
		delete module;
		return false;
	}

	if (module->getModuleName().value == ":null")
	{
		delete module;
		return true;
	}

	const auto key = std::make_tuple(memoryTypeName,
	                                 module->getModuleName());
	if (memoryModules.find(key) != memoryModules.end())
	{
		delete module;
		return false;
	}

	memoryModules[key] = module;

	return true;
}

inline bool cVirtualMachine::registerRootSignalExit(const tLibraryName& libraryName,
                                                    const tRootModuleName& rootModuleName,
                                                    const tSignalExitName& signalExitName,
                                                    tRootSignalExitId& rootSignalExitId)
{
	auto key = std::make_tuple(libraryName,
	                           rootModuleName,
	                           signalExitName);
	if (rootSignalExits.find(key) != rootSignalExits.end())
	{
		return false;
	}

	rootSignalExitId = rootSignalExits.size() + 1;
	rootSignalExits[key] = rootSignalExitId;
	return true;
}

inline bool cVirtualMachine::registerRootMemoryExit(const tLibraryName& libraryName,
                                                    const tRootModuleName& rootModuleName,
                                                    const tMemoryExitName& memoryExitName,
                                                    const tMemoryTypeName& memoryTypeName,
                                                    tRootMemoryExitId& rootMemoryExitId)
{
	auto key = std::make_tuple(libraryName,
	                           rootModuleName,
	                           memoryExitName);
	if (rootMemoryExits.find(key) != rootMemoryExits.end())
	{
		return false;
	}

	rootMemoryExitId = rootMemoryExits.size() + 1;
	rootMemoryExits[key] = std::make_tuple(memoryTypeName,
	                                       rootMemoryExitId);
	return true;
}

inline void cVirtualMachine::registerBuildInLibrary()
{
	registerRootSignalExit("tvm", "schemeLoaded", "signal", rootSignalSchemeLoaded);
	registerRootSignalExit("tvm", "schemeUnload", "signal", rootSignalSchemeUnload);
}

inline bool cVirtualMachine::readScheme(cStreamIn& stream,
                                        tSchemes& schemes)
{
	tSchemeName schemeName;
	stream.pop(schemeName);
	if (!schemeName.value.length())
	{
		return false;
	}

	cScheme* scheme = new cScheme(this);
	if (!scheme->read(stream))
	{
		delete scheme;
		return false;
	}

	if (stream.isFailed())
	{
		delete scheme;
		return false;
	}

	if (schemes.find(schemeName) != schemes.end())
	{
		delete scheme;
		return false;
	}

	schemes[schemeName] = scheme;
	return true;
}

inline void cVirtualMachine::freeSchemes(cVirtualMachine::tSchemes& schemes)
{
	for (auto& schemeIter : schemes)
	{
		cScheme* scheme = schemeIter.second;
		delete scheme;
	}
}

inline const cVirtualMachine::tMemoryTypes cVirtualMachine::getMemoryTypes() const
{
	return memoryTypes;
}

inline const cVirtualMachine::tModules cVirtualMachine::getModules() const
{
	tModules modules;

	for (const auto& iterLibrary : libraries)
	{
		for (const auto& iterModule : iterLibrary.second->getModules())
		{
			const auto key = std::make_tuple(iterLibrary.first,
			                                 iterModule.first);
			modules[key] = iterModule.second;
		}
	}

	for (const auto& iterModule : memoryModules)
	{
		const auto key = std::make_tuple(":memory:" + std::get<0>(iterModule.first).value,
		                                 std::get<1>(iterModule.first));
		modules[key] = iterModule.second;
	}

	return modules;
}

inline const cVirtualMachine::tRootSignalExits cVirtualMachine::getRootSignalExits() const
{
	return rootSignalExits;
}

inline const cVirtualMachine::tRootMemoryExits cVirtualMachine::getRootMemoryExits() const
{
	return rootMemoryExits;
}

template<typename TType>
bool cLibrary::registerMemory(const tMemoryTypeName& memoryTypeName)
{
	return virtualMachine->registerMemory<TType>(memoryTypeName);
}

template<typename TType>
bool cLibrary::registerMemory(const tMemoryTypeName& memoryTypeName, const TType& value)
{
	return virtualMachine->registerMemory<TType>(memoryTypeName, value);
}

inline bool cLibrary::registerMemoryModule(const tMemoryTypeName& memoryTypeName, cModule* module)
{
	return virtualMachine->registerMemoryModule(memoryTypeName,
	                                            module);
}

template<typename TType>
bool cLibrary::registerMemoryStandart(const tMemoryTypeName& memoryTypeName)
{
	return virtualMachine->registerMemoryStandart<TType>(memoryTypeName);
}

template<typename TType>
bool cLibrary::registerMemoryStandart(const tMemoryTypeName& memoryTypeName,
                                      const TType& value)
{
	return virtualMachine->registerMemoryStandart<TType>(memoryTypeName,
	                                                     value);
}

template<typename TType,
         std::size_t TSize>
bool cLibrary::registerMemoryArray(tMemoryTypeName memoryTypeNameArray,
                                   const tMemoryTypeName& memoryTypeName)
{
	return virtualMachine->registerMemoryArray<TType,
	                                           TSize>(memoryTypeNameArray,
	                                                  memoryTypeName);
}

template<typename TType>
bool cLibrary::registerMemoryVector(const tMemoryTypeName& memoryTypeName)
{
	return virtualMachine->registerMemoryVector<TType>(memoryTypeName);
}

template<typename TKeyType,
         typename TValueType>
bool cLibrary::registerMemoryMap(const tMemoryTypeName& memoryKeyTypeName,
                                 const tMemoryTypeName& memoryValueTypeName)
{
	return virtualMachine->registerMemoryMap<TKeyType,
	                                         TValueType>(memoryKeyTypeName,
	                                                     memoryValueTypeName);
}

template<typename TTuple>
bool cLibrary::registerMemoryTuple(tMemoryTypeName memoryTypeNameTuple,
                                   const std::vector<std::pair<tMemoryName, tMemoryTypeName>>& memories)
{
	return virtualMachine->registerMemoryTuple<TTuple>(memoryTypeNameTuple,
	                                                   memories);
}

template<typename TEnum>
bool cLibrary::registerMemoryEnum(tMemoryTypeName memoryTypeNameEnum,
                                  const std::vector<std::pair<tMemoryName, TEnum>>& items)
{
	return virtualMachine->registerMemoryEnum<TEnum>(memoryTypeNameEnum,
	                                                 items);
}

inline bool cLibrary::registerRootSignalExit(const tRootModuleName& rootModuleName,
                                             const tSignalExitName& signalExitName,
                                             tRootSignalExitId& rootSignalExitId)
{
	return virtualMachine->registerRootSignalExit(getLibraryName(),
	                                              rootModuleName,
	                                              signalExitName,
	                                              rootSignalExitId);
}

inline bool cLibrary::registerRootMemoryExit(const tRootModuleName& rootModuleName,
                                             const tMemoryExitName& memoryExitName,
                                             const tMemoryTypeName& memoryTypeName,
                                             tRootMemoryExitId& rootMemoryExitId)
{
	return virtualMachine->registerRootMemoryExit(getLibraryName(),
	                                              rootModuleName,
	                                              memoryExitName,
	                                              memoryTypeName,
	                                              rootMemoryExitId);
}

inline void cLibrary::stopVirtualMachine()
{
	virtualMachine->stop();
}

inline void cLibrary::rootSignalFlow(tRootSignalExitId rootSignalExitId)
{
	virtualMachine->rootSignalFlow(rootSignalExitId);
}

template<typename TType>
inline void cLibrary::rootSetMemory(tRootMemoryExitId rootMemoryExitId, const TType& value)
{
	virtualMachine->rootSetMemory(rootMemoryExitId, value);
}

inline bool cLibrary::isStopped() const
{
	return virtualMachine->isStopped();
}

inline bool cScheme::init(const tSchemes& schemes,
                          const tProjectId& projectId)
{
	this->parentScheme = nullptr;
	this->parentModuleId = 0;
	this->projectId = projectId;

	if (!initModules(schemes))
	{
		return false;
	}

	if (!initFlows())
	{
		return false;
	}

	return true;
}

inline bool cScheme::initModules(const tSchemes& schemes)
{
#define CHECK_MAP(map, key) \
do { \
	if ((map).find(key) == (map).end()) \
	{ \
		return false; \
	} \
} while (0)

	const auto virtualMachineModules = virtualMachine->getModules();

	for (const auto& iter : loadMemories)
	{
		const auto& map = virtualMachine->getMemoryTypes();
		auto key = iter.second;
		CHECK_MAP(map, key);

		memories[iter.first] = map.find(key)->second->clone();
	}

	for (const auto& iter : loadModules)
	{
		const auto& map = virtualMachineModules;
		auto key = iter.second;
		CHECK_MAP(map, key);

		cModule* module = map.find(key)->second->clone();

		const auto& memoryEntries = map.find(key)->second->getMemoryEntries();
		for (const auto& iter : memoryEntries)
		{
			const std::ptrdiff_t memoryOffset = std::get<1>(iter.second);

			std::ptrdiff_t moduleMemoryPointer = (std::ptrdiff_t)module;
			moduleMemoryPointer += memoryOffset;

			*(void**)moduleMemoryPointer = nullptr;
		}

		const auto& memoryExits = map.find(key)->second->getMemoryExits();
		for (const auto& iter : memoryExits)
		{
			const std::ptrdiff_t memoryOffset = std::get<1>(iter.second);

			std::ptrdiff_t moduleMemoryPointer = (std::ptrdiff_t)module;
			moduleMemoryPointer += memoryOffset;

			*(void**)moduleMemoryPointer = nullptr;
		}

		modules[iter.first] = module;
	}

	for (const auto& iter : loadCustomModules)
	{
		const tModuleId moduleId = iter.first;
		const tSchemeName& schemeName = iter.second;
		CHECK_MAP(schemes, schemeName);

		cScheme* scheme = schemes.find(schemeName)->second->clone();
		scheme->parentScheme = this;
		scheme->parentModuleId = moduleId;
		scheme->projectId = projectId;
		if (!scheme->initModules(schemes))
		{
			delete scheme;
			return false;
		}

		customModules[moduleId] = scheme;
	}

	for (const auto& iter : loadMemoryModuleVariables)
	{
		const tModuleId moduleId = iter.first;

		CHECK_MAP(memories, moduleId);

		if (!memories.find(moduleId)->second->write(iter.second))
		{
			return false;
		}
	}

	for (auto& module : modules)
	{
		if (!module.second->doInit(this))
		{
			return false;
		}
	}

	return true;

#undef CHECK_MAP
}

inline bool cScheme::initFlows()
{
#define CHECK_MAP(map, key) \
do { \
	if ((map).find(key) == (map).end()) \
	{ \
		return false; \
	} \
} while (0)

	const auto virtualMachineModules = virtualMachine->getModules();

	for (const auto& iter : loadRootSignalFlows)
	{
		const auto& map = virtualMachine->getRootSignalExits();
		auto key = iter.first;
		CHECK_MAP(map, key);

		const tModuleId entryModuleId = std::get<0>(iter.second);
		const tSignalEntryName& signalEntryName = std::get<1>(iter.second);

		cModule* registerModule;
		cModule* clonedModule;
		cSignalEntry* entrySignalEntry;
		if (!findEntryPathModule(entryModuleId,
		                         signalEntryName,
		                         registerModule,
		                         clonedModule,
		                         entrySignalEntry))
		{
			return false;
		}

		if ((!registerModule) ||
		    (!clonedModule) ||
		    (!entrySignalEntry))
		{
			continue;
		}

		auto rootSignalFlowValue = std::make_tuple(entrySignalEntry,
		                                           clonedModule);
		rootSignalFlows[map.find(key)->second] = rootSignalFlowValue;
	}

	for (const auto& iter : loadRootMemoryExitFlows)
	{
		CHECK_MAP(virtualMachine->getRootMemoryExits(), iter.first);

		const tModuleId entryModuleId = std::get<0>(iter.second);
		const tMemoryEntryName& memoryEntryName = std::get<1>(iter.second);

		void* pointer;
		if (!findMemoryEntryPath(entryModuleId,
		                         memoryEntryName,
		                         pointer))
		{
			return false;
		}

		if (!pointer)
		{
			continue;
		}

		const auto rootMemoryFlowsKey = std::get<1>(virtualMachine->getRootMemoryExits().find(iter.first)->second);
		rootMemoryFlows[rootMemoryFlowsKey] = pointer;
	}

	for (const auto& iter : loadModules)
	{
		CHECK_MAP(virtualMachineModules, iter.second);

		const cModule* exitRegisterModule = virtualMachineModules.find(iter.second)->second;

		for (const auto& signalExit : exitRegisterModule->getSignalExits())
		{
			const auto loadSignalFlowsKey = std::make_tuple(iter.first,
			                                                signalExit.first);
			if (loadSignalFlows.find(loadSignalFlowsKey) == loadSignalFlows.end())
			{
				continue;
			}

			const tModuleId entryModuleId = std::get<0>(loadSignalFlows.find(loadSignalFlowsKey)->second);
			const tSignalEntryName& signalEntryName = std::get<1>(loadSignalFlows.find(loadSignalFlowsKey)->second);

			cModule* entryRegisterModule;
			cModule* entryClonedModule;
			cSignalEntry* entrySignalEntry;
			if (!findEntryPathModule(entryModuleId,
			                         signalEntryName,
			                         entryRegisterModule,
			                         entryClonedModule,
			                         entrySignalEntry))
			{
				return false;
			}

			if ((!entryRegisterModule) ||
			    (!entryClonedModule) ||
			    (!entrySignalEntry))
			{
				continue;
			}

			CHECK_MAP(modules, iter.first);

			const auto signalFlowKey = std::make_tuple(modules.find(iter.first)->second,
			                                           signalExit.second);
			const auto signalFlowValue = std::make_tuple(entrySignalEntry,
			                                             entryClonedModule);
			signalFlows[signalFlowKey] = signalFlowValue;
		}

		for (const auto& memoryExit : exitRegisterModule->getMemoryExits())
		{
			tModuleId toModuleId;
			tMemoryEntryName toMemoryEntryName;

			if (!getMemoryModule(iter.first, memoryExit.first,
			                     toModuleId, toMemoryEntryName))
			{
				continue;
			}

			void* pointer;
			if (!findMemoryEntryPath(toModuleId,
			                         toMemoryEntryName,
			                         pointer))
			{
				return false;
			}

			if (!pointer)
			{
				continue;
			}

			CHECK_MAP(modules, iter.first);

			std::ptrdiff_t moduleMemoryPointer = (std::ptrdiff_t)modules.find(iter.first)->second;
			moduleMemoryPointer += std::get<1>(memoryExit.second);

			*(void**)moduleMemoryPointer = pointer;
		}

		for (const auto& memoryEntry : exitRegisterModule->getMemoryEntries())
		{
			tModuleId toModuleId;
			tMemoryExitName toMemoryExitName;

			if (!getMemoryModule(iter.first, memoryEntry.first,
			                     toModuleId, toMemoryExitName))
			{
				continue;
			}

			void* pointer;
			if (!findMemoryExitPath(toModuleId,
			                        toMemoryExitName,
			                        pointer))
			{
				return false;
			}

			if (!pointer)
			{
				continue;
			}

			CHECK_MAP(modules, iter.first);

			std::ptrdiff_t moduleMemoryPointer = (std::ptrdiff_t)modules.find(iter.first)->second;
			moduleMemoryPointer += std::get<1>(memoryEntry.second);

			*(void**)moduleMemoryPointer = pointer;
		}
	}

	for (const auto& iter : loadCustomModules)
	{
		const tModuleId moduleId = iter.first;
		CHECK_MAP(customModules, moduleId);
		if (!customModules[moduleId]->initFlows())
		{
			return false;
		}
	}

	return true;

#undef CHECK_MAP
}

inline bool cScheme::findEntryPathModule(const tModuleId entryModuleId,
                                         const tSignalEntryName& signalEntryName,
                                         cModule*& registerModule,
                                         cModule*& clonedModule,
                                         cSignalEntry*& signalEntry) const
{
#define CHECK_MAP(map, key) \
do { \
	if ((map).find(key) == (map).end()) \
	{ \
		return false; \
	} \
} while (0)

	if (loadModules.find(entryModuleId) != loadModules.end())
	{
		CHECK_MAP(modules, entryModuleId);

		const auto virtualMachineModules = virtualMachine->getModules();
		CHECK_MAP(virtualMachineModules, loadModules.find(entryModuleId)->second);

		registerModule = virtualMachineModules.find(loadModules.find(entryModuleId)->second)->second;
		clonedModule = modules.find(entryModuleId)->second;

		CHECK_MAP(registerModule->getSignalEntries(), signalEntryName);
		signalEntry = std::get<1>(registerModule->getSignalEntries().find(signalEntryName)->second);

		const auto& entryModuleSignalEntries = registerModule->getSignalEntries();
		CHECK_MAP(entryModuleSignalEntries, signalEntryName);

		return true;
	}
	else if (loadCustomModules.find(entryModuleId) != loadCustomModules.end())
	{
		CHECK_MAP(customModules, entryModuleId);

		const cScheme* scheme = customModules.find(entryModuleId)->second;

		tModuleId schemeEntryModuleId = scheme->findSchemeSignalEntryModule(signalEntryName.value);
		if (!schemeEntryModuleId)
		{
			return false;
		}

		const tLoadSignalFlows& signalFlows = scheme->loadSignalFlows;
		const auto signalFlowsKey = std::make_tuple(schemeEntryModuleId,
		                                            signalEntryName.value);

		if (signalFlows.find(signalFlowsKey) == signalFlows.end())
		{
			registerModule = nullptr;
			clonedModule = nullptr;
			signalEntry = nullptr;
			return true;
		}

		return scheme->findEntryPathModule(std::get<0>(signalFlows.find(signalFlowsKey)->second),
		                                   std::get<1>(signalFlows.find(signalFlowsKey)->second),
		                                   registerModule,
		                                   clonedModule,
		                                   signalEntry);
	}
	else if (loadSchemeSignalExitModules.find(entryModuleId) != loadSchemeSignalExitModules.end())
	{
		if (!parentScheme)
		{
			return false;
		}

		const tLoadSignalFlows& signalFlows = parentScheme->loadSignalFlows;
		const auto signalFlowsKey = std::make_tuple(parentModuleId,
		                                            signalEntryName.value);

		if (signalFlows.find(signalFlowsKey) == signalFlows.end())
		{
			registerModule = nullptr;
			clonedModule = nullptr;
			signalEntry = nullptr;
			return true;
		}

		return parentScheme->findEntryPathModule(std::get<0>(signalFlows.find(signalFlowsKey)->second),
		                                         std::get<1>(signalFlows.find(signalFlowsKey)->second),
		                                         registerModule,
		                                         clonedModule,
		                                         signalEntry);
	}

	return false;

#undef CHECK_MAP
}

inline bool cScheme::findMemoryEntryPath(const tModuleId entryModuleId,
                                         const tMemoryEntryName& memoryEntryName,
                                         void*& pointer) const
{
#define CHECK_MAP(map, key) \
do { \
	if ((map).find(key) == (map).end()) \
	{ \
		return false; \
	} \
} while (0)

	if (loadMemories.find(entryModuleId) != loadMemories.end())
	{
		CHECK_MAP(memories, entryModuleId);

		pointer = memories.find(entryModuleId)->second->getPointer();

		return true;
	}
	else if (loadCustomModules.find(entryModuleId) != loadCustomModules.end())
	{
		CHECK_MAP(customModules, entryModuleId);

		const cScheme* scheme = customModules.find(entryModuleId)->second;

		tModuleId schemeEntryModuleId = scheme->findSchemeMemoryEntryModule(memoryEntryName.value);
		if (!schemeEntryModuleId)
		{
			return false;
		}

		tModuleId toModuleId;
		tMemoryEntryName toMemoryEntryName;

		if (!scheme->getMemoryModule(schemeEntryModuleId, memoryEntryName.value,
		                             toModuleId, toMemoryEntryName))
		{
			pointer = nullptr;
			return true;
		}

		return scheme->findMemoryEntryPath(toModuleId,
		                                   toMemoryEntryName,
		                                   pointer);
	}
	else if (loadSchemeMemoryExitModules.find(entryModuleId) != loadSchemeMemoryExitModules.end())
	{
		if (!parentScheme)
		{
			return false;
		}

		tModuleId toModuleId;
		tMemoryEntryName toMemoryEntryName;

		if (!parentScheme->getMemoryModule(parentModuleId, memoryEntryName.value,
		                                   toModuleId, toMemoryEntryName))
		{
			pointer = nullptr;
			return true;
		}

		return parentScheme->findMemoryEntryPath(toModuleId,
		                                         toMemoryEntryName,
		                                         pointer);
	}

	return false;

#undef CHECK_MAP
}

inline bool cScheme::findMemoryExitPath(const tModuleId moduleId,
                                        const tMemoryExitName& memoryExitName,
                                        void*& pointer) const
{
#define CHECK_MAP(map, key) \
do { \
	if ((map).find(key) == (map).end()) \
	{ \
		return false; \
	} \
} while (0)

	if (loadMemories.find(moduleId) != loadMemories.end())
	{
		CHECK_MAP(memories, moduleId);

		pointer = memories.find(moduleId)->second->getPointer();

		return true;
	}
	else if (loadCustomModules.find(moduleId) != loadCustomModules.end())
	{
		CHECK_MAP(customModules, moduleId);

		const cScheme* scheme = customModules.find(moduleId)->second;

		tModuleId schemeModuleId = scheme->findSchemeMemoryExitModule(memoryExitName.value);
		if (!schemeModuleId)
		{
			return false;
		}

		tModuleId toModuleId;
		tMemoryExitName toMemoryExitName;
		if (!scheme->getMemoryModule(schemeModuleId, memoryExitName.value,
		                             toModuleId, toMemoryExitName))
		{
			pointer = nullptr;
			return true;
		}

		return scheme->findMemoryExitPath(toModuleId,
		                                  toMemoryExitName,
		                                  pointer);
	}
	else if (loadSchemeMemoryEntryModules.find(moduleId) != loadSchemeMemoryEntryModules.end())
	{
		if (!parentScheme)
		{
			return false;
		}

		tModuleId toModuleId;
		tMemoryExitName toMemoryExitName;
		if (!parentScheme->getMemoryModule(parentModuleId, memoryExitName.value,
		                                   toModuleId, toMemoryExitName))
		{
			pointer = nullptr;
			return true;
		}

		return parentScheme->findMemoryExitPath(toModuleId,
		                                        toMemoryExitName,
		                                        pointer);
	}

	return false;

#undef CHECK_MAP
}

template<typename TType>
void cVirtualMachine::rootSetMemory(tRootMemoryExitId rootMemoryExitId, const TType& value)
{
	std::lock_guard<std::mutex> guard(mutex);
	for (const auto& currentSchemeIter : currentSchemes)
	{
		cScheme* currentScheme = currentSchemeIter.second;
		currentScheme->rootSetMemory(rootMemoryExitId, value);
	}
}

inline bool cActionModule::signalFlow(tSignalExitId signalExitId)
{
	std::lock_guard<std::mutex> guard(scheme->virtualMachine->mutex);
	return scheme->signalFlow(this, signalExitId);
}

inline void* cActionModule::cSimpleThread::callHelper(void* args)
{
	cSimpleThread* simpleThread = (cSimpleThread*)args;
	if (!simpleThread->module->scheme->virtualMachine->isStopped())
	{
		simpleThread->module->run();
	}
	simpleThread->thread = 0;
	return nullptr;
}

}

#endif // TVM_VM_H
