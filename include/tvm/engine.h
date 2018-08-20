// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_ENGINE_H
#define TVM_ENGINE_H

#ifdef TVM_CONVERTER
#error "include 'engine' or 'converter'"
#endif

#define TVM_ENGINE

#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <fstream>
#include <mutex>
#include <shared_mutex>

#include "concurrentqueue/blockingconcurrentqueue.h"

#include "register.h"
#include "stream.h"
#include "scheme.h"
#include "locker.h"
#include "fileformat.h"

namespace nVirtualMachine
{

class cEngine : public cRegister
{
public:
	cEngine();
	~cEngine();

	eResult loadProjectFromFile(const std::string& filePath);
	eResult loadProjectFromMemory(const std::vector<uint8_t>& buffer);

	void unloadProject();

	void run();
	void stop();
	void join();

	/// thread-safe
	template<typename TType>
	eResult getRootModule(const TType*& rootModule) const;

	/// thread-safe
	template<typename TType>
	eResult getRootModule(const TType*& rootModule,
	                      const cLocker*& locker) const;

	/// thread-safe
	/// moodycamel queue is not linearizable
	void rootSignal(const tRootSignalExitId& rootSignalExitId) const;

	/// thread-safe
	void waitAllSchemes() const;

protected:
	eResult loadRootModules(const std::vector<nFileFormat::tRootModule>& fileRootModules);
	eResult loadGlobalMemories(const std::vector<nFileFormat::tMemoryModule>& fileGlobalMemories);
	eResult loadConstMemories(const std::vector<nFileFormat::tMemoryModule>& fileConstMemories);

protected:
	friend class cScheme;

	eResult createMemoryModule(const tMemoryModuleType& memoryModuleType,
	                           const tSchemeId& schemeId,
	                           const tMemoryModuleId& moduleId,
	                           const std::vector<uint8_t>& value,
	                           void** modulePointer);

	void destroyMemoryModule(const tMemoryModuleType& memoryModuleType,
	                         void* modulePointer);

	eResult createLogicModule(const tLogicModuleType& logicModuleType,
	                          const tSchemeId& schemeId,
	                          const tLogicModuleId& moduleId,
	                          void** modulePointer);

	void destroyLogicModule(const tLogicModuleType& logicModuleType,
	                        void* modulePointer);

	eResult getLogicModuleManager(const tLogicModuleType& logicModuleType,
	                              cLogicModuleManager** logicModuleManager) const;

	/// thread-safe
	void barrierSyncEnqueue();

protected:
	using tRootMemory = std::tuple<tMemoryModuleType,
	                               void*, ///< @todo: locker?
	                               void*>;

	using tGlobalMemory = std::tuple<tMemoryModuleType,
	                                 std::shared_mutex*, ///< @todo: locker?
	                                 void*>;

	using tConstMemory = std::tuple<tMemoryModuleType,
	                                void*>;

protected:
	mutable std::mutex barrierMutex;
	mutable moodycamel::BlockingConcurrentQueue<bool> barrierSync;

	std::vector<tRootMemory> rootMemories;
	std::vector<tGlobalMemory> globalMemories;
	std::vector<tConstMemory> constMemories;
	std::vector<cScheme*> schemes;
};

//

inline cEngine::cEngine()
{
}

inline cEngine::~cEngine()
{
	unloadProject();
}

inline eResult cEngine::loadProjectFromFile(const std::string& filePath)
{
	std::ifstream fileStream(filePath, std::ifstream::binary);
	if (!fileStream.is_open())
	{
		return eResult::fileNotFound;
	}

	std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(fileStream)),
	                            std::istreambuf_iterator<char>());

	return loadProjectFromMemory(buffer);
}

inline eResult cEngine::loadProjectFromMemory(const std::vector<uint8_t>& buffer)
{
	unloadProject();

	cStreamIn stream(buffer);

	uint32_t fileHeader;
	stream.pop(fileHeader);

	if (stream.isFailed() ||
	    fileHeader != nFileFormat::headerMagic)
	{
		unloadProject();
		return eResult::invalidFormat;
	}

	for (cRootModuleManager* rootModuleManager : rootModuleManagers)
	{
		rootModuleManager->initRootMemory();
	}

	nFileFormat::tProject fileProject;
	stream.pop(fileProject);

	if (stream.isFailed())
	{
		unloadProject();
		return eResult::invalidFormat;
	}

	eResult result = loadRootModules(std::get<0>(fileProject));
	if (result != eResult::success)
	{
		unloadProject();
		return result;
	}

	result = loadGlobalMemories(std::get<1>(fileProject));
	if (result != eResult::success)
	{
		unloadProject();
		return result;
	}

	result = loadConstMemories(std::get<2>(fileProject));
	if (result != eResult::success)
	{
		unloadProject();
		return result;
	}

	for (const nFileFormat::tScheme& fileScheme : std::get<3>(fileProject))
	{
		tSchemeId schemeId = schemes.size();
		cScheme* scheme = new cScheme(this, schemeId);

		result = scheme->load(fileScheme);
		if (result != eResult::success)
		{
			delete scheme;
			unloadProject();
			return result;
		}

		schemes.emplace_back(scheme);
	}

	/// @todo: check available size in stream

	return result;
}

inline void cEngine::unloadProject()
{
	stop();
	join();

	for (tRootMemory& rootMemory : rootMemories)
	{
		const tMemoryModuleType& memoryModuleType = std::get<0>(rootMemory);
		/// @todo
		void* modulePointer = std::get<2>(rootMemory);

		destroyMemoryModule(memoryModuleType,
		                    modulePointer);
	}
	rootMemories.clear();

	for (tGlobalMemory& globalMemory : globalMemories)
	{
		const tMemoryModuleType& memoryModuleType = std::get<0>(globalMemory);
		std::shared_mutex* mutex = std::get<1>(globalMemory);
		void* modulePointer = std::get<2>(globalMemory);

		delete mutex;

		destroyMemoryModule(memoryModuleType,
		                    modulePointer);
	}
	globalMemories.clear();

	for (tConstMemory& constMemory : constMemories)
	{
		const tMemoryModuleType& memoryModuleType = std::get<0>(constMemory);
		void* modulePointer = std::get<1>(constMemory);

		destroyMemoryModule(memoryModuleType,
		                    modulePointer);
	}
	constMemories.clear();

	for (auto& iter : schemes)
	{
		delete iter;
	}
	schemes.clear();
}

inline void cEngine::run()
{
	for (cScheme* scheme : schemes)
	{
		scheme->run();
	}
}

inline void cEngine::stop()
{
	for (cScheme* scheme : schemes)
	{
		scheme->stop();
	}
}

inline void cEngine::join()
{
	for (cScheme* scheme : schemes)
	{
		scheme->join();
	}
}

template<typename TType>
inline eResult cEngine::getRootModule(const TType*& rootModule) const
{
	tUniqueId uniqueId = getUniqueId<TType>(); ///< @todo: constexpr

	for (const cRootModuleManager* iter : rootModuleManagers)
	{
		if (iter->getRootModuleUniqueId() == uniqueId)
		{
			rootModule = (const TType*)iter->getRootModule();

			return eResult::success;
		}
	}

	return eResult::invalidRootModule;
}

template<typename TType>
inline eResult cEngine::getRootModule(const TType*& rootModule,
                                      const cLocker*& locker) const
{
	tUniqueId uniqueId = getUniqueId<TType>(); ///< @todo: constexpr

	for (const cRootModuleManager* iter : rootModuleManagers)
	{
		if (iter->getRootModuleUniqueId() == uniqueId)
		{
			rootModule = (const TType*)iter->getRootModule();
			locker = iter->getLocker();

			return eResult::success;
		}
	}

	return eResult::invalidRootModule;
}

inline void cEngine::rootSignal(const tRootSignalExitId& rootSignalExitId) const
{
	for (const cScheme* scheme : schemes)
	{
		scheme->rootSignal(rootSignalExitId);
	}
}

inline void cEngine::waitAllSchemes() const
{
	constexpr unsigned int itemsMax = 32;

	std::lock_guard<std::mutex> guard(barrierMutex);

	unsigned int schemesCount = schemes.size();
	for (const cScheme* scheme : schemes)
	{
		scheme->sync();
	}

	bool items[itemsMax];
	while (schemesCount > 0)
	{
		schemesCount -= barrierSync.wait_dequeue_bulk(items, itemsMax);
	}
}

inline eResult cEngine::loadRootModules(const std::vector<nFileFormat::tRootModule>& fileRootModules)
{
	eResult result = eResult::success;

	for (const auto& fileRootModule : fileRootModules)
	{
		tRootModuleType rootModuleType = std::get<0>(fileRootModule);

		for (const auto& fileRootMemoryModule : std::get<1>(fileRootModule))
		{
			tMemoryModuleId memoryModuleId = rootMemories.size();
			const tRootMemoryExitId& rootMemoryExitId = std::get<0>(fileRootMemoryModule);
			const tMemoryModuleType& memoryModuleType = std::get<0>(std::get<1>(fileRootMemoryModule));
			const std::vector<uint8_t>& value = std::get<1>(std::get<1>(fileRootMemoryModule));

			void* memoryModulePointer;
			result = createMemoryModule(memoryModuleType,
			                            schemeRootMemories,
			                            memoryModuleId,
			                            value,
			                            &memoryModulePointer);
			if (result != eResult::success)
			{
				return result;
			}

			rootMemories.emplace_back(memoryModuleType,
			                          nullptr, ///< @todo: locker?
			                          memoryModulePointer);

			if (rootModuleManagers.size() <= rootModuleType)
			{
				return eResult::invalidModuleType;
			}

			result = rootModuleManagers[rootModuleType]->setRootMemoryExit(rootMemoryExitId,
			                                                               memoryModulePointer);
			if (result != eResult::success)
			{
				return result;
			}
		}
	}

	return result;
}

inline eResult cEngine::loadGlobalMemories(const std::vector<nFileFormat::tMemoryModule>& fileGlobalMemories)
{
	eResult result = eResult::success;

	for (const nFileFormat::tMemoryModule& fileGlobalMemory : fileGlobalMemories)
	{
		tMemoryModuleId memoryModuleId = globalMemories.size();
		const tMemoryModuleType& memoryModuleType = std::get<0>(fileGlobalMemory);
		const std::vector<uint8_t>& value = std::get<1>(fileGlobalMemory);

		std::shared_mutex* mutex = new std::shared_mutex();
		if (mutex == nullptr)
		{
			return eResult::errorAllocationMemory;
		}

		void* modulePointer;
		result = createMemoryModule(memoryModuleType,
		                            schemeGlobalMemories,
		                            memoryModuleId,
		                            value,
		                            &modulePointer);
		if (result != eResult::success)
		{
			delete mutex;
			return result;
		}

		globalMemories.emplace_back(memoryModuleType,
		                            mutex,
		                            modulePointer);
	}

	return result;
}

inline eResult cEngine::loadConstMemories(const std::vector<nFileFormat::tMemoryModule>& fileConstMemories)
{
	eResult result = eResult::success;

	for (const nFileFormat::tMemoryModule& fileConstMemory : fileConstMemories)
	{
		tMemoryModuleId memoryModuleId = constMemories.size();
		const tMemoryModuleType& memoryModuleType = std::get<0>(fileConstMemory);
		const std::vector<uint8_t>& value = std::get<1>(fileConstMemory);

		void* modulePointer;
		result = createMemoryModule(memoryModuleType,
		                            schemeConstMemories,
		                            memoryModuleId,
		                            value,
		                            &modulePointer);
		if (result != eResult::success)
		{
			return result;
		}

		constMemories.emplace_back(memoryModuleType,
		                           modulePointer);
	}

	return result;
}

inline eResult cEngine::createMemoryModule(const tMemoryModuleType& memoryModuleType,
                                           TVM_UNUSED const tSchemeId& schemeId,
                                           TVM_UNUSED const tMemoryModuleId& moduleId,
                                           const std::vector<uint8_t>& value,
                                           void** modulePointer)
{
	/// @todo: custom allocator

	if (memoryModuleManagers.size() <= memoryModuleType)
	{
		return eResult::invalidModuleType;
	}

	cMemoryModuleManager* memoryModuleManager = memoryModuleManagers[memoryModuleType];
	*modulePointer = memoryModuleManager->create(value);

	return eResult::success;
}

inline void cEngine::destroyMemoryModule(const tMemoryModuleType& memoryModuleType,
                                         void* modulePointer)
{
	/// @todo: custom allocator

	if (memoryModuleManagers.size() <= memoryModuleType)
	{
		TVM_LOG_ERROR("unknown memory module type: '%" PRIu32 "'\n", memoryModuleType);
		return;
	}

	cMemoryModuleManager* memoryModuleManager = memoryModuleManagers[memoryModuleType];
	memoryModuleManager->destroy(modulePointer);
}

inline eResult cEngine::createLogicModule(const tLogicModuleType& logicModuleType,
                                          TVM_UNUSED const tSchemeId& schemeId,
                                          TVM_UNUSED const tLogicModuleId& moduleId,
                                          void** modulePointer)
{
	/// @todo: custom allocator

	if (logicModuleType == logicModuleTypeLock)
	{
		/// @todo
	}
	else if (logicModuleType == logicModuleTypeUnlock)
	{
		/// @todo
	}

	if (logicModuleManagers.size() <= logicModuleType)
	{
		return eResult::invalidModuleType;
	}

	cLogicModuleManager* logicModuleManager = logicModuleManagers[logicModuleType];
	*modulePointer = logicModuleManager->create();

	return eResult::success;
}

inline void cEngine::destroyLogicModule(const tLogicModuleType& logicModuleType,
                                        void* modulePointer)
{
	/// @todo: custom allocator

	if (logicModuleManagers.size() <= logicModuleType)
	{
		TVM_LOG_ERROR("unknown module type: '%" PRIu32 "'\n", logicModuleType);
		return;
	}

	cLogicModuleManager* logicModuleManager = logicModuleManagers[logicModuleType];
	logicModuleManager->destroy(modulePointer);
}

inline eResult cEngine::getLogicModuleManager(const tLogicModuleType& logicModuleType,
                                              cLogicModuleManager** logicModuleManager) const
{
	if (logicModuleManagers.size() <= logicModuleType)
	{
		TVM_LOG_ERROR("invalid logicModuleType: '%" PRIu32 "'\n", logicModuleType);
		return eResult::invalidModuleType;
	}

	*logicModuleManager = logicModuleManagers[logicModuleType];
	return eResult::success;
}

inline void cEngine::barrierSyncEnqueue()
{
	/// @todo: check errors
	barrierSync.enqueue(true);
}

//

inline cScheme::cScheme(cEngine* engine,
                        const tSchemeId& schemeId) :
        engine(engine),
        schemeId(schemeId)
{
}

inline eResult cScheme::load(const nFileFormat::tScheme& fileScheme)
{
	eResult result = eResult::success;

	callbacks.resize(engine->rootSignalExitsCount, nullptr);

	result = loadMemoryModules(std::get<0>(fileScheme));
	if (result != eResult::success)
	{
		return result;
	}

	result = loadLogicModules(std::get<1>(fileScheme));
	if (result != eResult::success)
	{
		return result;
	}

	result = loadRootModuleSignalExits(std::get<2>(fileScheme));
	if (result != eResult::success)
	{
		return result;
	}

	result = loadLogicModuleSignalExits(std::get<3>(fileScheme));
	if (result != eResult::success)
	{
		return result;
	}

	result = loadLogicModuleMemories(std::get<4>(fileScheme));
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

inline void cScheme::unload()
{
	for (tMemoryModule& module : memoryModules)
	{
		const tMemoryModuleType& memoryModuleType = std::get<0>(module);
		void* modulePointer = std::get<1>(module);

		engine->destroyMemoryModule(memoryModuleType,
		                            modulePointer);
	}
	memoryModules.clear();

	for (tLogicModule& module : logicModules)
	{
		const tLogicModuleType& logicModuleType = std::get<0>(module);
		void* modulePointer = std::get<1>(module);

		engine->destroyLogicModule(logicModuleType,
		                           modulePointer);
	}
	logicModules.clear();

	for (cCallbackBase* callback : callbacks)
	{
		if (callback != nullptr)
		{
			delete callback;
		}
	}
	callbacks.clear();
}

inline bool cScheme::handleFlow(tFlowId& flowId) const
{
	if (flowId >> flowShift)
	{
		if (flowId == flowStop)
		{
			return false;
		}
		else if (flowId == flowSync)
		{
			engine->barrierSyncEnqueue();
		}

		return true;
	}

	while (flowId != flowNowhere)
	{
		flowId = callbacks[flowId]->call();
	}

	return true;
}

inline eResult cScheme::loadMemoryModules(const std::vector<nFileFormat::tMemoryModule>& fileMemoryModules)
{
	eResult result = eResult::success;

	for (const nFileFormat::tMemoryModule& fileMemoryModule : fileMemoryModules)
	{
		tMemoryModuleId memoryModuleId = memoryModules.size();
		const tMemoryModuleType& memoryModuleType = std::get<0>(fileMemoryModule);
		const std::vector<uint8_t>& value = std::get<1>(fileMemoryModule);

		void* modulePointer;
		result = engine->createMemoryModule(memoryModuleType,
		                                    schemeId,
		                                    memoryModuleId,
		                                    value,
		                                    &modulePointer);
		if (result != eResult::success)
		{
			return result;
		}

		memoryModules.emplace_back(tMemoryModule(memoryModuleType, modulePointer));
	}

	return result;
}

inline eResult cScheme::loadLogicModules(const std::vector<nFileFormat::tLogicModule>& filelogicModules)
{
	eResult result = eResult::success;

	for (const nFileFormat::tLogicModule& logicModule : filelogicModules)
	{
		tLogicModuleId logicModuleId = logicModules.size();
		const tLogicModuleType& logicModuleType = std::get<0>(logicModule);

		void* modulePointer;
		result = engine->createLogicModule(logicModuleType,
		                                   schemeId,
		                                   logicModuleId,
		                                   &modulePointer);
		if (result != eResult::success)
		{
			return result;
		}

		logicModules.emplace_back(tLogicModule(logicModuleType, modulePointer));
	}

	return result;
}

inline eResult cScheme::loadRootModuleSignalExits(const std::vector<nFileFormat::tRootModuleSignalExits>& fileRootModuleSignalExits)
{
	eResult result = eResult::success;

	for (const nFileFormat::tRootModuleSignalExits& rootSignalExit : fileRootModuleSignalExits)
	{
		const tRootSignalExitId& rootSignalExitId = std::get<0>(rootSignalExit);
		const tLogicModuleId& targetModuleId = std::get<1>(rootSignalExit);
		const tSignalEntryId& targetSignalEntryId = std::get<2>(rootSignalExit);

		if (engine->rootSignalExitsCount <= rootSignalExitId)
		{
			TVM_LOG_ERROR("invalid rootSignalExitId: '%" PRIu32 "'\n", rootSignalExitId);
			return eResult::invalidRootSignalExitId;
		}

		if (logicModules.size() <= targetModuleId)
		{
			TVM_LOG_ERROR("invalid targetModuleId: '%" PRIu32 "'\n", targetModuleId);
			return eResult::invalidModuleId;
		}

		const tLogicModuleType& targetLogicModuleType = std::get<0>(logicModules[targetModuleId]);
		void* targetModulePointer = std::get<1>(logicModules[targetModuleId]);

		cLogicModuleManager* targetLogicModuleManager;
		result = engine->getLogicModuleManager(targetLogicModuleType,
		                                       &targetLogicModuleManager);
		if (result != eResult::success)
		{
			return result;
		}

		cCallbackBase* callback;
		result = targetLogicModuleManager->getCallback(targetSignalEntryId,
		                                               targetModulePointer,
		                                               &callback);
		if (result != eResult::success)
		{
			return result;
		}

		callbacks[rootSignalExitId] = callback;
	}

	return result;
}

inline eResult cScheme::loadLogicModuleSignalExits(const std::vector<nFileFormat::tLogicModuleSignalExits>& fileLogicModuleSignalExits)
{
	eResult result = eResult::success;

	for (const nFileFormat::tLogicModuleSignalExits& logicSignalExits : fileLogicModuleSignalExits)
	{
		const tLogicModuleId& moduleId = std::get<0>(logicSignalExits);

		if (logicModules.size() <= moduleId)
		{
			TVM_LOG_ERROR("invalid module id: '%" PRIu32 "'\n", moduleId);
			return eResult::invalidModuleId;
		}

		const tLogicModuleType& logicModuleType = std::get<0>(logicModules[moduleId]);
		void* modulePointer = std::get<1>(logicModules[moduleId]);

		cLogicModuleManager* logicModuleManager;
		result = engine->getLogicModuleManager(logicModuleType,
		                                       &logicModuleManager);
		if (result != eResult::success)
		{
			return result;
		}

		for (const auto& signalExit : std::get<1>(logicSignalExits))
		{
			const tSignalExitId& signalExitId = std::get<0>(signalExit);
			const tLogicModuleId& targetModuleId = std::get<1>(signalExit);
			const tSignalEntryId& targetSignalEntryId = std::get<2>(signalExit);

			if (logicModules.size() <= targetModuleId)
			{
				TVM_LOG_ERROR("invalid targetModuleId: '%" PRIu32 "'\n", targetModuleId);
				return eResult::invalidModuleId;
			}

			tFlowId* flowId;
			result = logicModuleManager->getFlowId(signalExitId,
			                                       modulePointer,
			                                       &flowId);
			if (result != eResult::success)
			{
				return result;
			}

			const tLogicModuleType& targetLogicModuleType = std::get<0>(logicModules[targetModuleId]);
			void* targetModulePointer = std::get<1>(logicModules[targetModuleId]);

			cLogicModuleManager* targetLogicModuleManager;
			result = engine->getLogicModuleManager(targetLogicModuleType,
			                                       &targetLogicModuleManager);
			if (result != eResult::success)
			{
				return result;
			}

			cCallbackBase* callback;
			result = targetLogicModuleManager->getCallback(targetSignalEntryId,
			                                               targetModulePointer,
			                                               &callback);
			if (result != eResult::success)
			{
				return result;
			}

			*flowId = callbacks.size();

			callbacks.emplace_back(callback);
		}
	}

	return result;
}

inline eResult cScheme::loadLogicModuleMemories(const std::vector<nFileFormat::tLogicModuleMemories>& fileLogicModuleMemories)
{
	eResult result = eResult::success;

	for (const nFileFormat::tLogicModuleMemories& logicMemories : fileLogicModuleMemories)
	{
		const tLogicModuleId& moduleId = std::get<0>(logicMemories);

		if (logicModules.size() <= moduleId)
		{
			TVM_LOG_ERROR("invalid module id: '%" PRIu32 "'\n", moduleId);
			return eResult::invalidModuleId;
		}

		const tLogicModuleType& logicModuleType = std::get<0>(logicModules[moduleId]);
		void* modulePointer = std::get<1>(logicModules[moduleId]);

		cLogicModuleManager* logicModuleManager;
		result = engine->getLogicModuleManager(logicModuleType,
		                                       &logicModuleManager);
		if (result != eResult::success)
		{
			return result;
		}

		/// memoryEntries
		for (const auto& memoryEntry : std::get<1>(logicMemories))
		{
			const tMemoryEntryId& memoryEntryId = std::get<0>(memoryEntry);
			const eMemoryPosition& memoryPosition = std::get<1>(memoryEntry);
			const tMemoryModuleId& memoryModuleId = std::get<2>(memoryEntry);

			void* memoryModulePointer;
			if (memoryPosition == eMemoryPosition::scheme)
			{
				if (memoryModules.size() <= memoryModuleId)
				{
					return eResult::invalidModuleId;
				}

				memoryModulePointer = std::get<1>(memoryModules[memoryModuleId]);
			}
			else if (memoryPosition == eMemoryPosition::global)
			{
				if (engine->globalMemories.size() <= memoryModuleId)
				{
					return eResult::invalidModuleId;
				}

				memoryModulePointer = std::get<2>(engine->globalMemories[memoryModuleId]);
			}
			else if (memoryPosition == eMemoryPosition::root)
			{
				if (engine->rootMemories.size() <= memoryModuleId)
				{
					return eResult::invalidModuleId;
				}

				memoryModulePointer = std::get<2>(engine->rootMemories[memoryModuleId]);
			}
			else if (memoryPosition == eMemoryPosition::constant)
			{
				if (engine->constMemories.size() <= memoryModuleId)
				{
					return eResult::invalidModuleId;
				}

				memoryModulePointer = std::get<1>(engine->constMemories[memoryModuleId]);
			}
			else
			{
				return eResult::invalidMemoryPosition;
			}

			const void** memoryPointer;
			result = logicModuleManager->getMemoryEntry(memoryEntryId,
			                                            modulePointer,
			                                            &memoryPointer);
			if (result != eResult::success)
			{
				return result;
			}

			*memoryPointer = memoryModulePointer;
		}

		/// memoryExits
		for (const auto& memoryExit : std::get<2>(logicMemories))
		{
			const tMemoryExitId& memoryExitId = std::get<0>(memoryExit);
			const eMemoryPosition& memoryPosition = std::get<1>(memoryExit);
			const tMemoryModuleId& memoryModuleId = std::get<2>(memoryExit);

			void* memoryModulePointer;
			if (memoryPosition == eMemoryPosition::scheme)
			{
				if (memoryModules.size() <= memoryModuleId)
				{
					return eResult::invalidModuleId;
				}

				memoryModulePointer = std::get<1>(memoryModules[memoryModuleId]);
			}
			else if (memoryPosition == eMemoryPosition::global)
			{
				if (engine->globalMemories.size() <= memoryModuleId)
				{
					return eResult::invalidModuleId;
				}

				memoryModulePointer = std::get<2>(engine->globalMemories[memoryModuleId]);
			}
			else
			{
				return eResult::invalidMemoryPosition;
			}

			void** memoryPointer;
			result = logicModuleManager->getMemoryExit(memoryExitId,
			                                           modulePointer,
			                                           &memoryPointer);
			if (result != eResult::success)
			{
				return result;
			}

			*memoryPointer = memoryModulePointer;
		}

		/// memoryEntryExits
		for (const auto& memoryEntryExit : std::get<3>(logicMemories))
		{
			const tMemoryEntryExitId& memoryEntryExitId = std::get<0>(memoryEntryExit);
			const eMemoryPosition& memoryPosition = std::get<1>(memoryEntryExit);
			const tMemoryModuleId& memoryModuleId = std::get<2>(memoryEntryExit);

			void* memoryModulePointer;
			if (memoryPosition == eMemoryPosition::scheme)
			{
				if (memoryModules.size() <= memoryModuleId)
				{
					return eResult::invalidModuleId;
				}

				memoryModulePointer = std::get<1>(memoryModules[memoryModuleId]);
			}
			else if (memoryPosition == eMemoryPosition::global)
			{
				if (engine->globalMemories.size() <= memoryModuleId)
				{
					return eResult::invalidModuleId;
				}

				memoryModulePointer = std::get<2>(engine->globalMemories[memoryModuleId]);
			}
			else
			{
				return eResult::invalidMemoryPosition;
			}

			void** memoryPointer;
			result = logicModuleManager->getMemoryEntryExit(memoryEntryExitId,
			                                                modulePointer,
			                                                &memoryPointer);
			if (result != eResult::success)
			{
				return result;
			}

			*memoryPointer = memoryModulePointer;
		}
	}

	return result;
}

}

#endif // TVM_ENGINE_H
