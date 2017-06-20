// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_LIBRARY_BASE_H
#define TFVM_LIBRARY_BASE_H

#include <vector>
#include <map>
#include <algorithm>

#include <stdlib.h>
#include <unistd.h>

#include <tfvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cBase : public cLibrary
{
public:
	using tBoolean = bool;
	using tString = std::string;
	using tInteger = int64_t;
	using tBuffer = std::vector<uint8_t>;

public:
	cBase()
	{
	}

	cBase(int argc, char** argv, char** envp)
	{
		for (int arg_i = 0; arg_i < argc; arg_i++)
		{
			arguments.push_back(argv[arg_i]);
		}

		if (envp)
		{
			while (*envp)
			{
				const std::string env(*envp);
				envp++;

				const std::string envName = env.substr(0, env.find('='));
				const std::string envValue = env.substr(env.find('=') + 1);

				environments[envName] = envValue;
			}
		}
	}

	bool registerLibrary() override
	{
		setLibraryName("base");

		if (!registerMemory<tBoolean>("boolean",
		                              false))
		{
			return false;
		}

		if (!registerMemoryModule("boolean",
		                          new cLogicCopy<tBoolean>("boolean")))
		{
			return false;
		}

		if (!registerMemoryModule("boolean",
		                          new cLogicIf<tBoolean>("boolean")))
		{
			return false;
		}

		if (!registerMemoryModule("boolean",
		                          new cLogicBooleanSwitch()))
		{
			return false;
		}

		if (!registerMemoryModule("boolean",
		                          new cLogicConvert<tBoolean,
		                                            tString>("toString",
		                                                     "boolean",
		                                                     "string",
			[](tBoolean* from, tString* to)
			{
				*to = ((*from) ? "true" : "false");
			})))
		{
			return false;
		}

		if (!registerMemory<tString>("string"))
		{
			return false;
		}

		if (!registerMemoryModule("string",
		                          new cLogicCopy<tString>("string")))
		{
			return false;
		}

		if (!registerMemoryModule("string",
		                          new cLogicIsEmpty<tString,
		                                            tBoolean>("string",
		                                                      "boolean")))
		{
			return false;
		}

		if (!registerMemoryModule("string",
		                          new cLogicAppend<tString>("string")))
		{
			return false;
		}

		if (!registerMemoryModule("string",
		                          new cLogicSetClear<tString>("string")))
		{
			return false;
		}

		if (!registerMemoryModule("string",
		                          new cLogicIfEqual<tString,
		                                            tBoolean>("string",
		                                                    "boolean")))
		{
			return false;
		}

		if (!registerMemoryModule("string",
		                          new cLogicSize<tString,
		                                         tInteger>("getLength",
		                                                   "string",
		                                                   "integer")))
		{
			return false;
		}

		if (!registerMemoryStandart<tInteger,
		                            tBoolean>("integer",
		                                      "boolean",
		                                      0))
		{
			return false;
		}

		if (!registerMemoryVector<tString,
		                          tInteger,
		                          tBoolean>("string",
		                                    "integer",
		                                    "boolean"))
		{
			return false;
		}

		if (!registerMemoryMap<tString,
		                       tString,
		                       tInteger,
		                       tBoolean>("string",
		                                 "string",
		                                 "integer",
		                                 "boolean"))
		{
			return false;
		}

		if (!registerMemory<tBuffer>("buffer"))
		{
			return false;
		}

		if (!registerMemoryModule("buffer",
		                          new cLogicSize<tBuffer,
		                                         tInteger>("getSize",
		                                                   "buffer",
		                                                   "integer")))
		{
			return false;
		}

		if (!registerMemoryModule("integer",
		                          new cLogicConvert<tInteger,
		                                            tString>("toString",
		                                                     "integer",
		                                                     "string",
			[](tInteger* from, tString* to)
			{
				*to = std::to_string(*from);
			})))
		{
			return false;
		}

		if (!registerRootModules(rootRun))
		{
			return false;
		}

		if (!registerModules(new cLogicGetArguments(this),
		                     new cLogicGetEnvironments(this),
		                     new cLogicTrue(),
		                     new cLogicFalse()))
		{
			return false;
		}

		return true;
	}

	void run() override
	{
		rootSetMemory(rootRun.memoryArguments, arguments);
		rootSetMemory(rootRun.memoryEnvironments, environments);
		rootSignalFlow(rootRun.signal);
	}

private:
	std::vector<tString> arguments;
	std::map<tString, tString> environments;

private: /** rootModules */
	class cRootRun : public cRootModule
	{
	public:
		bool registerModule() override
		{
			setModuleName("run");

			if (!registerSignalExit("signal", signal))
			{
				return false;
			}

			if (!registerMemoryExit("arguments", "vector<string>", memoryArguments))
			{
				return false;
			}

			if (!registerMemoryExit("environments", "map<string,string>", memoryEnvironments))
			{
				return false;
			}

			return true;
		}

		tRootSignalExitId signal;
		tRootMemoryExitId memoryArguments;
		tRootMemoryExitId memoryEnvironments;
	};

private:
	cRootRun rootRun;

private: /** modules */
	class cLogicGetArguments : public cLogicModule
	{
	public:
		cLogicGetArguments(cBase* library) :
		        library(library)
		{
		}

		cModule* clone() const override
		{
			return new cLogicGetArguments(library);
		}

		bool registerModule() override
		{
			setModuleName("getArguments");
			setCaptionName("getArguments");

			if (!registerSignalEntry("signal", &cLogicGetArguments::signalEntry))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			if (!registerMemoryExit("vector<string>", "vector<string>", vector))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (vector)
			{
				*vector = library->arguments;
			}
			return signalFlow(signalExit);
		}

	private:
		cBase* library;

	private:
		const tSignalExitId signalExit = 1;

	private:
		std::vector<tString>* vector;
	};

	class cLogicGetEnvironments : public cLogicModule
	{
	public:
		cLogicGetEnvironments(cBase* library) :
		        library(library)
		{
		}

		cModule* clone() const override
		{
			return new cLogicGetEnvironments(library);
		}

		bool registerModule() override
		{
			setModuleName("getEnvironments");
			setCaptionName("getEnvironments");

			if (!registerSignalEntry("signal", &cLogicGetEnvironments::signalEntry))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			if (!registerMemoryExit("map<string,string>", "map<string,string>", map))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (map)
			{
				*map = library->environments;
			}
			return signalFlow(signalExit);
		}

	private:
		cBase* library;

	private:
		const tSignalExitId signalExit = 1;

	private:
		std::map<tString, tString>* map;
	};

	class cLogicBooleanSwitch : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicBooleanSwitch();
		}

		bool registerModule() override
		{
			setModuleName("switch");
			setCaptionName("switch");

			if (!registerSignalEntry("signal", &cLogicBooleanSwitch::signalEntry))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			if (!registerMemoryExit("boolean", "boolean", to))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (to)
			{
				*to = !(*to);
			}
			return signalFlow(signalExit);
		}

	private:
		const tSignalExitId signalExit = 1;

	private:
		tBoolean* to;
	};

	class cLogicTrue : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicTrue();
		}

		bool registerModule() override
		{
			setModuleName("true");

			if (!registerSignalEntry("signal", &cLogicTrue::signalEntry))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			return true;
		}
	};

	class cLogicFalse : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicFalse();
		}

		bool registerModule() override
		{
			setModuleName("false");

			if (!registerSignalEntry("signal", &cLogicFalse::signalEntry))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			return false;
		}
	};
};

}

}

#endif // TFVM_LIBRARY_BASE_H
