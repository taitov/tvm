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
	bool loadFromMemory(const std::vector<char>& buffer);
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

	template<typename TType,
	         typename TBooleanType>
	bool registerMemoryStandart(const tMemoryTypeName& memoryTypeName,
	                            const tMemoryTypeName& memoryBooleanTypeName)
	{
		if (memoryTypes.find(memoryTypeName) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModuleStandart<TType,
		                                  TBooleanType>(memoryTypeName,
		                                                memoryBooleanTypeName))
		{
			return false;
		}

		memoryTypes[memoryTypeName] = new cMemoryVariable<TType>();
		return true;
	}

	template<typename TType,
	         typename TBooleanType>
	bool registerMemoryStandart(const tMemoryTypeName& memoryTypeName,
	                            const tMemoryTypeName& memoryBooleanTypeName,
	                            const TType& value)
	{
		if (memoryTypes.find(memoryTypeName) != memoryTypes.end())
		{
			return false;
		}

		if (!registerMemoryModuleStandart<TType,
		                                  TBooleanType>(memoryTypeName,
		                                                memoryBooleanTypeName))
		{
			return false;
		}

		memoryTypes[memoryTypeName] = new cMemoryVariable<TType>(value);
		return true;
	}

	template<typename TType,
	         typename TIntegerType,
	         typename TBooleanType>
	bool registerMemoryVector(const tMemoryTypeName& memoryTypeName,
	                          const tMemoryTypeName& memoryIntegerTypeName,
	                          const tMemoryTypeName& memoryBooleanTypeName)
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
		                                            TBooleanType>(memoryTypeNameVector,
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
		                                         TIntegerType>("getCount",
		                                                       memoryTypeNameVector,
		                                                       memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicVectorGet<TType,
		                                              TIntegerType>(memoryTypeName,
		                                                            memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameVector,
		                          new cLogicVectorForEach<TType>(memoryTypeName)))
		{
			return false;
		}

		memoryTypes[memoryTypeNameVector] = new cMemoryVariable<tVector>();
		return true;
	}

	template<typename TKeyType,
	         typename TValueType,
	         typename TIntegerType,
	         typename TBooleanType>
	bool registerMemoryMap(const tMemoryTypeName& memoryKeyTypeName,
	                       const tMemoryTypeName& memoryValueTypeName,
	                       const tMemoryTypeName& memoryIntegerTypeName,
	                       const tMemoryTypeName& memoryBooleanTypeName)
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
		                                            TBooleanType>(memoryTypeNameMap,
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
		                                         TIntegerType>("getCount",
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

		memoryTypes[memoryTypeNameMap] = new cMemoryVariable<tMap>();
		return true;
	}

	template<typename TBooleanType,
	         typename ... TTypes>
	bool registerMemoryTuple(const tMemoryTypeName& memoryBooleanTypeName,
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
		                                            TBooleanType>(memoryTypeNameTuple,
		                                                          memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameTuple,
		                          new cLogicTupleGet<TTypes ...>(memoryNames,
		                                                         memoryTypeNames)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryTypeNameTuple,
		                          new cLogicTupleSet<TTypes ...>(memoryNames,
		                                                         memoryTypeNames)))
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

private: /** load */
	using tSchemes = std::map<tSchemeName,
	                          cScheme*>;

	tSchemes schemes;

private: /** exec */
	volatile bool stopped;
	cScheme* currentScheme;
	std::mutex mutex;
	tRootSignalExitId rootSignalSchemeLoaded;
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

	std::vector<char> buffer((std::istreambuf_iterator<char>(fileStream)),
	                          std::istreambuf_iterator<char>());

	return loadFromMemory(buffer);
}

bool cVirtualMachine::loadFromMemory(const std::vector<char>& buffer)
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
		if (!currentScheme->init(nullptr))
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
		delete currentScheme;
		currentScheme = nullptr;
	}

	if (schemes.find("main") == schemes.end())
	{
		return false;
	}

	currentScheme = schemes["main"]->clone();
	if (!currentScheme->init(nullptr))
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
	for (auto& iter : schemes)
	{
		delete iter.second;
	}
	schemes.clear();

	if (currentScheme)
	{
		delete currentScheme;
		currentScheme = nullptr;
	}
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

template<typename TType,
         typename TBooleanType>
bool cLibrary::registerMemoryStandart(const tMemoryTypeName& memoryTypeName,
                                      const tMemoryTypeName& memoryBooleanTypeName)
{
	return virtualMachine->registerMemoryStandart<TType,
	                                              TBooleanType>(memoryTypeName,
	                                                            memoryBooleanTypeName);
}

template<typename TType,
         typename TBooleanType>
bool cLibrary::registerMemoryStandart(const tMemoryTypeName& memoryTypeName,
                                      const tMemoryTypeName& memoryBooleanTypeName,
                                      const TType& value)
{
	return virtualMachine->registerMemoryStandart<TType,
	                                              TBooleanType>(memoryTypeName,
	                                                            memoryBooleanTypeName,
	                                                            value);
}

template<typename TType,
         typename TIntegerType,
         typename TBooleanType>
bool cLibrary::registerMemoryVector(const tMemoryTypeName& memoryTypeName,
                                    const tMemoryTypeName& memoryIntegerTypeName,
                                    const tMemoryTypeName& memoryBooleanTypeName)
{
	return virtualMachine->registerMemoryVector<TType,
	                                            TIntegerType,
	                                            TBooleanType>(memoryTypeName,
	                                                          memoryIntegerTypeName,
	                                                          memoryBooleanTypeName);
}

template<typename TKeyType,
         typename TValueType,
         typename TIntegerType,
         typename TBooleanType>
bool cLibrary::registerMemoryMap(const tMemoryTypeName& memoryKeyTypeName,
                                 const tMemoryTypeName& memoryValueTypeName,
                                 const tMemoryTypeName& memoryIntegerTypeName,
                                 const tMemoryTypeName& memoryBooleanTypeName)
{
	return virtualMachine->registerMemoryMap<TKeyType,
	                                         TValueType,
	                                         TIntegerType,
	                                         TBooleanType>(memoryKeyTypeName,
	                                                       memoryValueTypeName,
	                                                       memoryIntegerTypeName,
	                                                       memoryBooleanTypeName);
}

template<typename TBooleanType,
         typename ... TTypes>
bool cLibrary::registerMemoryTuple(const tMemoryTypeName& memoryBooleanTypeName,
                                   const std::vector<tMemoryName>& memoryNames,
                                   const std::vector<tMemoryTypeName>& memoryTypeNames)
{
	return virtualMachine->registerMemoryTuple<TBooleanType,
	                                           TTypes ...>(memoryBooleanTypeName,
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

bool cScheme::init(cScheme* parentScheme)
{
#define CHECK_MAP(map, key) \
do { \
	if ((map).find(key) == (map).end()) \
	{ \
		return false; \
	} \
} while (0)

	this->parentScheme = parentScheme;

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

	for (const auto& iter : loadRootSignalFlows)
	{
		const auto& map = virtualMachine->getRootSignalExits();
		auto key = iter.first;
		CHECK_MAP(map, key);

		tModuleId entryModuleId = std::get<0>(iter.second);
		CHECK_MAP(loadModules, entryModuleId);

		tSignalEntryName signalEntryName = std::get<1>(iter.second);

		const auto& modules = virtualMachineModules;
		auto moduleKey = loadModules.find(entryModuleId)->second;
		CHECK_MAP(modules, moduleKey);

		const auto entryModuleSignalEntries = modules.find(moduleKey)->second->getSignalEntries();
		CHECK_MAP(entryModuleSignalEntries, signalEntryName);

		const tModules& schemeModules = this->modules;
		CHECK_MAP(schemeModules, entryModuleId);

		cModule* entryModule = schemeModules.find(entryModuleId)->second;

		auto rootSignalFlowValue = std::make_tuple(std::get<1>(entryModuleSignalEntries.find(signalEntryName)->second),
		                                           entryModule);
		rootSignalFlows[map.find(key)->second] = rootSignalFlowValue;
	}

	for (const auto& iter : loadRootMemoryExitFlows)
	{
		const auto rootMemoryExitKey = iter.first;
		const tModuleId entryModuleId = iter.second;

		CHECK_MAP(loadMemories, entryModuleId);

		CHECK_MAP(virtualMachine->getRootMemoryExits(), rootMemoryExitKey);
		CHECK_MAP(virtualMachine->getMemoryTypes(), loadMemories.find(entryModuleId)->second);

		CHECK_MAP(memories, entryModuleId);

		const auto rootMemoryFlowKey = std::get<1>(virtualMachine->getRootMemoryExits().find(rootMemoryExitKey)->second);
		const auto rootMemoryFlowValue = memories.find(entryModuleId)->second->getValue();

		rootMemoryFlows[rootMemoryFlowKey] = rootMemoryFlowValue;
	}

	for (const auto& iter : loadSignalFlows)
	{
		const tModuleId exitModuleId = std::get<0>(iter.first);
		const tModuleId entryModuleId = std::get<0>(iter.second);

		CHECK_MAP(loadModules, exitModuleId);
		CHECK_MAP(loadModules, entryModuleId);

		CHECK_MAP(virtualMachineModules, loadModules.find(exitModuleId)->second);
		CHECK_MAP(virtualMachineModules, loadModules.find(entryModuleId)->second);

		const auto& signalExits = virtualMachineModules.find(loadModules.find(exitModuleId)->second)->second->getSignalExits();
		const auto& signalEntries = virtualMachineModules.find(loadModules.find(entryModuleId)->second)->second->getSignalEntries();

		CHECK_MAP(signalExits, std::get<1>(iter.first));
		CHECK_MAP(signalEntries, std::get<1>(iter.second));

		const tSignalExitId signalExitId = signalExits.find(std::get<1>(iter.first))->second;
		const std::tuple<tSignalEntryId,
		                 cSignalEntry*> signalEntry = signalEntries.find(std::get<1>(iter.second))->second;

		CHECK_MAP(modules, exitModuleId);
		CHECK_MAP(modules, entryModuleId);

		const auto signalFlowKey = std::make_tuple(modules.find(exitModuleId)->second, signalExitId);
		const auto signalFlowValue = std::make_tuple(std::get<1>(signalEntry), modules.find(entryModuleId)->second);

		signalFlows[signalFlowKey] = signalFlowValue;
	}

	for (const auto& iter : loadMemoryEntryFlows)
	{
		const tModuleId exitModuleId = std::get<0>(iter.first);
		const tModuleId entryModuleId = iter.second;

		CHECK_MAP(loadModules, exitModuleId);
		CHECK_MAP(loadMemories, entryModuleId);

		CHECK_MAP(virtualMachineModules, loadModules.find(exitModuleId)->second);
		CHECK_MAP(virtualMachine->getMemoryTypes(), loadMemories.find(entryModuleId)->second);

		const auto& memoryEntries = virtualMachineModules.find(loadModules.find(exitModuleId)->second)->second->getMemoryEntries();

		CHECK_MAP(memoryEntries, std::get<1>(iter.first));

		const std::ptrdiff_t memoryOffset = std::get<1>(memoryEntries.find(std::get<1>(iter.first))->second);

		CHECK_MAP(modules, exitModuleId);
		CHECK_MAP(memories, entryModuleId);

		std::ptrdiff_t moduleMemoryPointer = (std::ptrdiff_t)modules.find(exitModuleId)->second;
		moduleMemoryPointer += memoryOffset;

		*(void**)moduleMemoryPointer = memories.find(entryModuleId)->second->getValue();
	}

	for (const auto& iter : loadMemoryExitFlows)
	{
		const tModuleId exitModuleId = std::get<0>(iter.first);
		const tModuleId entryModuleId = iter.second;

		CHECK_MAP(loadModules, exitModuleId);
		CHECK_MAP(loadMemories, entryModuleId);

		CHECK_MAP(virtualMachineModules, loadModules.find(exitModuleId)->second);
		CHECK_MAP(virtualMachine->getMemoryTypes(), loadMemories.find(entryModuleId)->second);

		const auto& memoryExits = virtualMachineModules.find(loadModules.find(exitModuleId)->second)->second->getMemoryExits();

		CHECK_MAP(memoryExits, std::get<1>(iter.first));

		const std::ptrdiff_t memoryOffset = std::get<1>(memoryExits.find(std::get<1>(iter.first))->second);

		CHECK_MAP(modules, exitModuleId);
		CHECK_MAP(memories, entryModuleId);

		std::ptrdiff_t moduleMemoryPointer = (std::ptrdiff_t)modules.find(exitModuleId)->second;
		moduleMemoryPointer += memoryOffset;

		*(void**)moduleMemoryPointer = memories.find(entryModuleId)->second->getValue();
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

bool cModule::setVariables(const std::vector<char>& buffer)
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
