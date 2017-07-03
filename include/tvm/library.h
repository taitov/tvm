// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_LIBRARY_H
#define TVM_LIBRARY_H

#include "vm.h"

namespace nVirtualMachine
{

class cVirtualMachine;

class cLibrary
{
	friend class cVirtualMachine;
	friend class cRootModule;

public:
	using tModules = std::map<tModuleName,
	                          cModule*>;

public:
	cLibrary();

	virtual ~cLibrary();

	const tLibraryName& getLibraryName() const;

	const tModules& getModules() const;

protected:
	void setLibraryName(const tLibraryName& libraryName);

	template<typename TType>
	bool registerMemory(const tMemoryTypeName& memoryTypeName);

	template<typename TType>
	bool registerMemory(const tMemoryTypeName& memoryTypeName, const TType& value);

	bool registerMemoryModule(const tMemoryTypeName& memoryTypeName,
	                          cModule* module);

	template<typename TType>
	bool registerMemoryStandart(const tMemoryTypeName& memoryTypeName);

	template<typename TType>
	bool registerMemoryStandart(const tMemoryTypeName& memoryTypeName,
	                            const TType& value);

	template<typename TType,
	         std::size_t TSize>
	bool registerMemoryArray(tMemoryTypeName memoryTypeNameArray,
	                         const tMemoryTypeName& memoryTypeName);

	template<typename TType>
	bool registerMemoryVector(const tMemoryTypeName& memoryTypeName);

	template<typename TKeyType,
	         typename TValueType>
	bool registerMemoryMap(const tMemoryTypeName& memoryKeyTypeName,
	                       const tMemoryTypeName& memoryValueTypeName);

	template<typename ... TTypes>
	bool registerMemoryTuple(tMemoryTypeName memoryTypeNameTuple,
	                         const std::vector<tMemoryName>& memoryNames,
	                         const std::vector<tMemoryTypeName>& memoryTypeNames);

	bool registerRootSignalExit(const tRootModuleName& rootModuleName,
	                            const tSignalExitName& signalExitName,
	                            tRootSignalExitId& rootSignalExitId);

	bool registerRootMemoryExit(const tRootModuleName& rootModuleName,
	                            const tMemoryExitName& memoryExitName,
	                            const tMemoryTypeName& memoryTypeName,
	                            tRootMemoryExitId& rootMemoryExitId);

	template<typename TType>
	bool registerModules(TType* module)
	{
		return registerModule(module);
	}

	template<typename THeadType,
	         typename ... TTypes>
	bool registerModules(THeadType* module, TTypes* ... modules)
	{
		if (!registerModule(module))
		{
			return false;
		}
		return registerModules<TTypes ...>(modules ...);
	}

	template<typename TType>
	bool registerRootModules(TType& module)
	{
		return registerRootModule(module);
	}

	template<typename THeadType,
	         typename ... TTypes>
	bool registerRootModules(THeadType& module, TTypes& ... modules)
	{
		if (!registerRootModule(module))
		{
			return false;
		}
		return registerRootModules<TTypes ...>(modules ...);
	}

	bool registerModule(cModule* module);
	bool registerRootModule(cRootModule& rootModule);

protected: /** exec */
	inline bool rootSignalFlow(tRootSignalExitId rootSignalExitId);

	template<typename TType>
	inline void rootSetMemory(tRootMemoryExitId rootMemoryExitId, const TType& value);

	inline bool isStopped() const;

private:
	bool doRegisterLibrary(cVirtualMachine* virtualMachine);
	virtual bool registerLibrary() = 0;
	virtual bool init();
	void doRun();
	virtual void run();
	void wait();
	inline virtual void stop();

private:
	class cSimpleThread
	{
	public:
		cSimpleThread()
		{
			thread = 0;
		}

		void run(cLibrary* object)
		{
			if (thread)
			{
				return;
			}

			this->object = object;

			pthread_attr_t attr;

			if (pthread_attr_init(&attr) != 0)
			{
				return;
			}
			if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0)
			{
				return;
			}

			if (pthread_create(&thread, &attr, &callHelper, this) != 0)
			{
				pthread_attr_destroy(&attr);
				return;
			}

			pthread_attr_destroy(&attr);
		}

		void wait()
		{
			if (thread != 0)
			{
				void* status;
				pthread_join(thread, &status);
				thread = 0;
			}
		}

		bool isDone()
		{
			return (thread == 0);
		}

	private:
		static void* callHelper(void* args)
		{
			cSimpleThread* simpleThread = (cSimpleThread*)args;
			simpleThread->object->run();
			return nullptr;
		}

		pthread_t thread;
		cLibrary* object;
	};

private:
	using tRootModules = std::map<tRootModuleName,
	                              cRootModule*>;

	cVirtualMachine* virtualMachine;

	tLibraryName libraryName;
	tModules modules;
	tRootModules rootModules;

	cSimpleThread thread;
};

inline cLibrary::cLibrary()
{
	virtualMachine = nullptr;
}

inline cLibrary::~cLibrary()
{
	for (auto& iter : modules)
	{
		delete iter.second;
	}
}

inline void cLibrary::setLibraryName(const tLibraryName& libraryName)
{
	this->libraryName = libraryName;
}

inline const tLibraryName& cLibrary::getLibraryName() const
{
	return libraryName;
}

inline const cLibrary::tModules& cLibrary::getModules() const
{
	return modules;
}

inline bool cLibrary::init()
{
	return true;
}

inline void cLibrary::doRun()
{
	thread.run(this);
}

inline void cLibrary::run()
{
}

inline void cLibrary::wait()
{
	thread.wait();
}

inline bool cLibrary::doRegisterLibrary(cVirtualMachine* virtualMachine)
{
	this->virtualMachine = virtualMachine;
	return registerLibrary();
}

inline bool cLibrary::registerModule(cModule* module)
{
	if (!module->doRegisterModule(virtualMachine))
	{
		delete module;
		return false;
	}

	if (module->getModuleName().value == ":null")
	{
		delete module;
		return true;
	}

	if (modules.find(module->getModuleName()) != modules.end())
	{
		delete module;
		return false;
	}

	modules[module->getModuleName()] = module;

	return true;
}

inline bool cLibrary::registerRootModule(cRootModule& rootModule)
{
	if (!rootModule.doRegisterModule(this))
	{
		return false;
	}

	if (rootModules.find(rootModule.getModuleName()) != rootModules.end())
	{
		return false;
	}

	rootModules[rootModule.getModuleName()] = &rootModule;

	return true;
}

inline void cLibrary::stop()
{
}

inline bool cRootModule::registerSignalExit(const tSignalExitName& signalExitName,
                                            tRootSignalExitId& rootSignalExitId)
{
	return library->registerRootSignalExit(getModuleName(),
	                                       signalExitName,
	                                       rootSignalExitId);
}

inline bool cRootModule::registerMemoryExit(const tMemoryExitName& memoryExitName,
                                            const tMemoryTypeName& memoryTypeName,
                                            tRootMemoryExitId& rootMemoryExitId)
{
	return library->registerRootMemoryExit(getModuleName(),
	                                       memoryExitName,
	                                       memoryTypeName,
	                                       rootMemoryExitId);
}

}

#endif // TVM_LIBRARY_H
