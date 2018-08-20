// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include <vector>
#include <shared_mutex>

#include <blockingconcurrentqueue.h>

#include "result.h"
#include "type.h"
#include "common.h"
#include "engine.h"
#include "fileformat.h"

namespace nVirtualMachine
{

class cEngine;

class cScheme
{
public:
	cScheme(cEngine* engine,
	        const tSchemeId& schemeId);
	~cScheme();

	eResult load(const nFileFormat::tScheme& fileScheme);
	void unload();

	void run();
	void stop();
	void join();

	/// thread-safe
	void rootSignal(const tRootSignalExitId& rootSignalExitId) const;

	/// thread-safe
	void sync() const;

protected:
	eResult loadMemoryModules(const std::vector<nFileFormat::tMemoryModule>& fileMemoryModules);
	eResult loadLogicModules(const std::vector<nFileFormat::tLogicModule>& fileLogicModules);
	eResult loadRootModuleSignalExits(const std::vector<nFileFormat::tRootModuleSignalExits>& fileRootModuleSignalExits);
	eResult loadLogicModuleSignalExits(const std::vector<nFileFormat::tLogicModuleSignalExits>& fileLogicModuleSignalExits);
	eResult loadLogicModuleMemories(const std::vector<nFileFormat::tLogicModuleMemories>& fileLogicModuleMemories);

protected: /// fast-path
	void startThread();
	inline bool handleFlow(tFlowId& flowId) const;

protected:
	using tMemoryModule = std::tuple<tMemoryModuleType,
	                                 void*>;

	using tLogicModule = std::tuple<tLogicModuleType,
	                                void*>;

protected:
	cEngine* engine;
	tSchemeId schemeId;
	std::thread thread;

	std::vector<tMemoryModule> memoryModules;
	std::vector<tLogicModule> logicModules;

protected: /// fast-path
	mutable moodycamel::BlockingConcurrentQueue<tFlowId> flowsQueue;

	std::vector<cCallbackBase*> callbacks;
};

//

inline cScheme::~cScheme()
{
	unload();
}

inline void cScheme::run()
{
	thread = std::thread([this]{startThread();});
}

inline void cScheme::stop()
{
	/// @todo: check errors
	flowsQueue.enqueue(flowStop);
}

inline void cScheme::join()
{
	if (thread.joinable())
	{
		thread.join();
	}
}

inline void cScheme::rootSignal(const tRootSignalExitId& rootSignalExitId) const
{
	tFlowId flowId = rootSignalExitId;
	if (callbacks[flowId] == nullptr)
	{
		return;
	}

	/// @todo: check errors
	flowsQueue.enqueue(flowId);
}

inline void cScheme::sync() const
{
	/// @todo: check errors
	flowsQueue.enqueue(flowSync);
}

inline void cScheme::startThread()
{
	constexpr unsigned int flowsMax = 8;

	tFlowId flowIds[flowsMax];

	for (;;)
	{
		unsigned int flowsCount = flowsQueue.wait_dequeue_bulk(flowIds, flowsMax);
		for (unsigned int flow_i = 0; flow_i < flowsCount; flow_i++)
		{
			if (!handleFlow(flowIds[flow_i]))
			{
				return;
			}
		}
	}
}

}
