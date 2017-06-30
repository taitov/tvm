// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_VM_H
#define TFVM_VM_H

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

public:
	using tBoolean = bool;
	const tMemoryTypeName memoryBooleanTypeName = "boolean";

	using tInteger = int64_t;
	const tMemoryTypeName memoryIntegerTypeName = "integer";

	using tBuffer = std::vector<uint8_t>;
	const tMemoryTypeName memoryBufferTypeName = "buffer";

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
	bool loadFromMemory(const std::vector<uint8_t>& buffer);
	bool reload();
	void unload();

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
	inline bool rootSignalFlow(tRootSignalExitId rootSignalExitId);

	template<typename TType>
	inline void rootSetMemory(tRootMemoryExitId rootMemoryExitId, const TType& value)
	{
		std::lock_guard<std::mutex> guard(mutex);
		if (!currentScheme)
		{
			return;
		}
		currentScheme->rootSetMemory(rootMemoryExitId, value);
	}

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

		if (!registerMemoryModule(memoryTypeNameArray, new cLogicConvert<tBuffer,
		                                                                 tArray>("fromBuffer",
		                                                                         memoryBufferTypeName,
		                                                                         memoryTypeNameArray,
			[](tBuffer* from, tArray* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
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

		if (!registerMemoryModule(memoryTypeNameVector, new cLogicConvert<tBuffer,
		                                                                 tVector>("fromBuffer",
		                                                                          memoryBufferTypeName,
		                                                                          memoryTypeNameVector,
			[](tBuffer* from, tVector* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
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

		if (!registerMemoryModule(memoryTypeNameMap, new cLogicConvert<tBuffer,
		                                                               tMap>("fromBuffer",
		                                                                     memoryBufferTypeName,
		                                                                     memoryTypeNameMap,
			[](tBuffer* from, tMap* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
			})))
		{
			return false;
		}

		memoryTypes[memoryTypeNameMap] = new cMemoryVariable<tMap>();
		return true;
	}

	template<typename ... TTypes>
	bool registerMemoryTuple(const std::vector<tMemoryName>& memoryNames,
	                         const std::vector<tMemoryTypeName>& memoryTypeNames)
	{
		tMemoryTypeName memoryTypeNameTuple = "tuple<";
		if (memoryNames.size())
		{
			for (unsigned int memoryTypeName_i = 0; memoryTypeName_i < memoryNames.size() - 1; memoryTypeName_i++)
			{
				memoryTypeNameTuple.value += memoryNames[memoryTypeName_i].value + ",";
			}
			for (unsigned int memoryTypeName_i = memoryNames.size() - 1; memoryTypeName_i < memoryNames.size(); memoryTypeName_i++)
			{
				memoryTypeNameTuple.value += memoryNames[memoryTypeName_i].value;
			}
		}
		memoryTypeNameTuple.value += ">";
		return registerMemoryTuple<TTypes ...>(memoryTypeNameTuple,
		                                       memoryNames,
		                                       memoryTypeNames);
	}

	template<typename ... TTypes>
	bool registerMemoryTuple(const tMemoryTypeName& memoryTypeNameTuple,
	                         const std::vector<tMemoryName>& memoryNames,
	                         const std::vector<tMemoryTypeName>& memoryTypeNames)
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

		if (!registerMemoryModule(memoryTypeNameTuple, new cLogicConvert<tBuffer,
		                                                                 tTuple>("fromBuffer",
		                                                                         memoryBufferTypeName,
		                                                                         memoryTypeNameTuple,
			[](tBuffer* from, tTuple* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
			})))
		{
			return false;
		}

		memoryTypes[memoryTypeNameTuple] = new cMemoryVariable<tTuple>();
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

		if (!registerMemoryModule(memoryTypeName, new cLogicConvert<tBuffer,
		                                                            TType>("fromBuffer",
		                                                                   memoryBufferTypeName,
		                                                                   memoryTypeName,
			[](tBuffer* from, TType* to)
			{
				cStreamIn stream(*from);
				stream.pop(*to);
			})))
		{
			return false;
		}

		return true;
	}

	void registerBuildInLibrary();

private: /** load */
	bool readScheme(cStreamIn& stream);

private:
	using tMemoryTypes = std::map<tMemoryTypeName,
	                              cMemory*>;

	using tModules = std::map<std::tuple<tLibraryName,
	                                     tModuleName>,
	                          cModule*>;

	using tSchemes = std::map<tSchemeName,
	                          cScheme*>;

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
	const tSchemes getSchemes() const;
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

private: /** load */
	tSchemes schemes;

private: /** exec */
	volatile bool stopped;
	cScheme* currentScheme;
	std::mutex mutex;
	tRootSignalExitId rootSignalSchemeLoaded;
	tRootSignalExitId rootSignalSchemeUnload;
};

cVirtualMachine::cVirtualMachine()
{
	stopped = false;
	currentScheme = nullptr;
	registerBuildInLibrary();
}

cVirtualMachine::~cVirtualMachine()
{
	stop();
	wait();
	unload();
	unregisterLibraries();
}

void cVirtualMachine::unregisterLibraries()
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

	registerBuildInLibrary();
}

bool cVirtualMachine::init()
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

bool cVirtualMachine::loadFromFile(const std::string& filePath)
{
	std::ifstream fileStream(filePath, std::ifstream::binary);
	if (!fileStream.is_open())
	{
		return false;
	}

	std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(fileStream)),
	                            std::istreambuf_iterator<char>());

	return loadFromMemory(buffer);
}

bool cVirtualMachine::loadFromMemory(const std::vector<uint8_t>& buffer)
{
	unload();

	cStreamIn stream(buffer);

	uint32_t magic = 0;
	stream.pop(magic);
	if (magic != fileHeaderMagic)
	{
		return false;
	}

	uint32_t schemesCount = 0;
	stream.pop(schemesCount);
	for (uint64_t scheme_i = 0; scheme_i < schemesCount; scheme_i++)
	{
		if (!readScheme(stream))
		{
			return false;
		}
	}

	if (stream.isFailed())
	{
		return false;
	}

	if (schemes.find("main") == schemes.end())
	{
		return false;
	}

	{
		std::lock_guard<std::mutex> guard(mutex);
		currentScheme = schemes["main"]->clone();
		if (!currentScheme->init(nullptr, 0))
		{
			delete currentScheme;
			currentScheme = nullptr;
			return false;
		}

		currentScheme->rootSignalFlow(rootSignalSchemeLoaded);
	}

	return true;
}

bool cVirtualMachine::reload()
{
	std::lock_guard<std::mutex> guard(mutex);

	if (currentScheme)
	{
		currentScheme->rootSignalFlow(rootSignalSchemeUnload);

		delete currentScheme;
		currentScheme = nullptr;
	}

	if (schemes.find("main") == schemes.end())
	{
		return false;
	}

	currentScheme = schemes["main"]->clone();
	if (!currentScheme->init(nullptr, 0))
	{
		delete currentScheme;
		currentScheme = nullptr;
		return false;
	}

	currentScheme->rootSignalFlow(rootSignalSchemeLoaded);

	return true;
}

void cVirtualMachine::unload()
{
	std::lock_guard<std::mutex> guard(mutex);

	if (currentScheme)
	{
		currentScheme->rootSignalFlow(rootSignalSchemeUnload);

		delete currentScheme;
		currentScheme = nullptr;
	}

	for (auto& iter : schemes)
	{
		delete iter.second;
	}
	schemes.clear();
}

void cVirtualMachine::run()
{
	stopped = false;

	for (auto& iter : libraries)
	{
		iter.second->doRun();
	}
}

void cVirtualMachine::wait()
{
	for (auto& iter : libraries)
	{
		iter.second->wait();
	}
}

void cVirtualMachine::stop()
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

const cVirtualMachine::tGuiMemoryTypes cVirtualMachine::getGuiMemoryTypes() const
{
	tGuiMemoryTypes guiMemoryTypes;

	for (auto& iter : memoryTypes)
	{
		guiMemoryTypes[iter.first] = iter.second;
	}

	return guiMemoryTypes;
}

const cVirtualMachine::tGuiRootModules cVirtualMachine::getGuiRootModules() const
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

const cVirtualMachine::tGuiModules cVirtualMachine::getGuiModules() const
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

const cVirtualMachine::tGuiMemoryModules cVirtualMachine::getGuiMemoryModules() const
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

inline bool cVirtualMachine::rootSignalFlow(tRootSignalExitId rootSignalExitId)
{
	std::lock_guard<std::mutex> guard(mutex);
	if (!currentScheme)
	{
		return false;
	}
	return currentScheme->rootSignalFlow(rootSignalExitId);
}

inline bool cVirtualMachine::isStopped() const
{
	return stopped;
}

bool cVirtualMachine::registerLibrary(cLibrary* library)
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

bool cVirtualMachine::registerMemoryModule(const tMemoryTypeName& memoryTypeName,
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

bool cVirtualMachine::registerRootSignalExit(const tLibraryName& libraryName,
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

bool cVirtualMachine::registerRootMemoryExit(const tLibraryName& libraryName,
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

void cVirtualMachine::registerBuildInLibrary()
{
	registerRootSignalExit("tfvm", "schemeLoaded", "signal", rootSignalSchemeLoaded);
	registerRootSignalExit("tfvm", "schemeUnload", "signal", rootSignalSchemeUnload);
}

bool cVirtualMachine::readScheme(cStreamIn& stream)
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

const cVirtualMachine::tMemoryTypes cVirtualMachine::getMemoryTypes() const
{
	return memoryTypes;
}

const cVirtualMachine::tModules cVirtualMachine::getModules() const
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

const cVirtualMachine::tSchemes cVirtualMachine::getSchemes() const
{
	return schemes;
}

const cVirtualMachine::tRootSignalExits cVirtualMachine::getRootSignalExits() const
{
	return rootSignalExits;
}

const cVirtualMachine::tRootMemoryExits cVirtualMachine::getRootMemoryExits() const
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

bool cLibrary::registerMemoryModule(const tMemoryTypeName& memoryTypeName, cModule* module)
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

template<typename ... TTypes>
bool cLibrary::registerMemoryTuple(tMemoryTypeName memoryTypeNameTuple,
                                   const std::vector<tMemoryName>& memoryNames,
                                   const std::vector<tMemoryTypeName>& memoryTypeNames)
{
	return virtualMachine->registerMemoryTuple<TTypes ...>(memoryTypeNameTuple,
	                                                       memoryNames,
	                                                       memoryTypeNames);
}

bool cLibrary::registerRootSignalExit(const tRootModuleName& rootModuleName,
                                      const tSignalExitName& signalExitName,
                                      tRootSignalExitId& rootSignalExitId)
{
	return virtualMachine->registerRootSignalExit(getLibraryName(),
	                                              rootModuleName,
	                                              signalExitName,
	                                              rootSignalExitId);
}

bool cLibrary::registerRootMemoryExit(const tRootModuleName& rootModuleName,
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

inline bool cLibrary::rootSignalFlow(tRootSignalExitId rootSignalExitId)
{
	return virtualMachine->rootSignalFlow(rootSignalExitId);
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

bool cScheme::init(cScheme* parentScheme, tModuleId parentModuleId)
{
#define CHECK_MAP(map, key) \
do { \
	if ((map).find(key) == (map).end()) \
	{ \
		return false; \
	} \
} while (0)

	this->parentScheme = parentScheme;
	this->parentModuleId = parentModuleId;

	const auto virtualMachineModules = virtualMachine->getModules();
	const auto virtualMachineSchemes = virtualMachine->getSchemes();

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
		CHECK_MAP(virtualMachineSchemes, schemeName);

		cScheme* scheme = virtualMachineSchemes.find(schemeName)->second->clone();
		if (!scheme->init(this, moduleId))
		{
			delete scheme;
			return false;
		}

		customModules[moduleId] = scheme;
	}

	for (const auto& iter : loadRootSignalFlows)
	{
		const auto& map = virtualMachine->getRootSignalExits();
		auto key = iter.first;
		CHECK_MAP(map, key);

		const tModuleId entryModuleId = std::get<0>(iter.second);
		const tSignalEntryName& signalEntryName = std::get<1>(iter.second);

		cModule* registerModule;
		cModule* clonedModule;
		if (!findEntryPathModule(entryModuleId,
		                         signalEntryName,
		                         registerModule,
		                         clonedModule))
		{
			return false;
		}

		if ((!registerModule) ||
		    (!clonedModule))
		{
			continue;
		}

		auto rootSignalFlowValue = std::make_tuple(std::get<1>(registerModule->getSignalEntries().find(signalEntryName)->second),
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
			if (!findEntryPathModule(entryModuleId,
			                         signalEntryName,
			                         entryRegisterModule,
			                         entryClonedModule))
			{
				return false;
			}

			if ((!entryRegisterModule) ||
			    (!entryClonedModule))
			{
				continue;
			}

			CHECK_MAP(modules, iter.first);
			CHECK_MAP(entryRegisterModule->getSignalEntries(), signalEntryName);

			const auto signalFlowKey = std::make_tuple(modules.find(iter.first)->second,
			                                           signalExit.second);
			const auto signalFlowValue = std::make_tuple(std::get<1>(entryRegisterModule->getSignalEntries().find(signalEntryName)->second),
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

	for (const auto& iter : loadMemoryModuleVariables)
	{
		const tModuleId moduleId = iter.first;

		CHECK_MAP(memories, moduleId);

		if (!memories.find(moduleId)->second->setVariables(iter.second))
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

bool cScheme::findEntryPathModule(const tModuleId entryModuleId,
                                  const tSignalEntryName& signalEntryName,
                                  cModule*& registerModule,
                                  cModule*& clonedModule) const
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
			return true;
		}

		return scheme->findEntryPathModule(std::get<0>(signalFlows.find(signalFlowsKey)->second),
		                                   std::get<1>(signalFlows.find(signalFlowsKey)->second),
		                                   registerModule,
		                                   clonedModule);
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
			return true;
		}

		return parentScheme->findEntryPathModule(std::get<0>(signalFlows.find(signalFlowsKey)->second),
		                                         std::get<1>(signalFlows.find(signalFlowsKey)->second),
		                                         registerModule,
		                                         clonedModule);
	}

	return false;

#undef CHECK_MAP
}

bool cScheme::findMemoryEntryPath(const tModuleId entryModuleId,
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

		pointer = memories.find(entryModuleId)->second->getValue();

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

bool cScheme::findMemoryExitPath(const tModuleId moduleId,
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

		pointer = memories.find(moduleId)->second->getValue();

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

bool cModule::setVariables(const std::vector<uint8_t>& buffer)
{
	cStreamIn stream(buffer);

	/** @todo */

	if (stream.isFailed())
	{
		return false;
	}

	return true;
}

}

#endif // TFVM_VM_H
