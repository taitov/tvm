// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_LIBRARY_BASE_H
#define TVM_LIBRARY_BASE_H

#include <vector>
#include <map>
#include <algorithm>

#include <stdlib.h>
#include <unistd.h>

#include <tvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cBase : public cLibrary
{
public:
	using tBoolean = cVirtualMachine::tBoolean;
	const tMemoryTypeName memoryBooleanTypeName = "boolean";

	using tInteger = cVirtualMachine::tInteger;
	const tMemoryTypeName memoryIntegerTypeName = "integer";

	using tBuffer = cVirtualMachine::tBuffer;
	const tMemoryTypeName memoryBufferTypeName = "buffer";

	using tByte = uint8_t;
	using tString = std::string;
	using tFloat = double;

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

		if (!registerMemory<tBoolean>(memoryBooleanTypeName,
		                              false))
		{
			return false;
		}

		if (!registerMemoryModule(memoryBooleanTypeName,
		                          new cLogicCopy<tBoolean>(memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryBooleanTypeName,
		                          new cLogicIf<tBoolean>(memoryBooleanTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryBooleanTypeName,
		                          new cLogicBooleanSwitch()))
		{
			return false;
		}

		if (!registerMemoryModule(memoryBooleanTypeName,
		                          new cLogicBooleanSetTrue()))
		{
			return false;
		}

		if (!registerMemoryModule(memoryBooleanTypeName,
		                          new cLogicBooleanSetFalse()))
		{
			return false;
		}

		if (!registerMemoryModule(memoryBooleanTypeName,
		                          new cLogicConvert<tBoolean,
		                                            tString>("toString",
		                                                     memoryBooleanTypeName,
		                                                     "string",
			[](tBoolean* from, tString* to)
			{
				*to = ((*from) ? "true" : "false");
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryBooleanTypeName,
		                          new cLogicConvertBool<tString,
		                                                tBoolean>("fromString",
		                                                          "string",
		                                                          memoryBooleanTypeName,
			[](tString* from, tBoolean* to)
			{
				if (*from == "true")
				{
					*to = true;
					return true;
				}
				else if (*from == "false")
				{
					*to = false;
					return true;
				}
				return false;
			})))
		{
			return false;
		}

		if (!registerMemoryStandart<tByte>("byte",
		                                   0))
		{
			return false;
		}

		if (!registerMemoryStandart<tString>("string"))
		{
			return false;
		}

		if (!registerMemoryModule("string",
		                          new cLogicIsEmpty<tString,
		                                            tBoolean>("string",
		                                                      memoryBooleanTypeName)))
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
		                          new cLogicSize<tString,
		                                         tInteger>("getLength",
		                                                   "string",
		                                                   memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryStandart<tInteger>(memoryIntegerTypeName,
		                                      0))
		{
			return false;
		}

		if (!registerMemoryVector<tString>("string"))
		{
			return false;
		}

		if (!registerMemoryMap<tString,
		                       tString>("string",
		                                "string"))
		{
			return false;
		}

		if (!registerMemory<tBuffer>(memoryBufferTypeName))
		{
			return false;
		}

		/*
		if (!registerMemoryModule(memoryBufferTypeName,
		                          new cLogicConvert<tBuffer,
		                                            tString>("toString",
		                                                     memoryBufferTypeName,
		                                                     "string",
			[](tBuffer* from, tString* to)
			{
				to->resize(from->size());
				memcpy(&(*to)[0], &(*from)[0], from->size());
				to->resize(strnlen(to->c_str(), to->size()));
			})))
		{
			return false;
		}
		*/

		if (!registerMemoryModule(memoryBufferTypeName,
		                          new cLogicSize<tBuffer,
		                                         tInteger>("getSize",
		                                                   memoryBufferTypeName,
		                                                   memoryIntegerTypeName)))
		{
			return false;
		}

		if (!registerMemoryModule(memoryIntegerTypeName,
		                          new cLogicConvert<tInteger,
		                                            tString>("toString",
		                                                     memoryIntegerTypeName,
		                                                     "string",
			[](tInteger* from, tString* to)
			{
				*to = std::to_string(*from);
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryIntegerTypeName,
		                          new cLogicConvertBool<tString,
		                                                tInteger>("fromString",
		                                                          "string",
		                                                          memoryIntegerTypeName,
			[](tString* from, tInteger* to)
			{
				char* endptr;
				*to = strtol(from->c_str(), &endptr, 0);
				if (endptr == from->c_str())
				{
					return false;
				}
				return true;
			})))
		{
			return false;
		}

		if (!registerMemoryStandart<tFloat>("float",
		                                    0.0))
		{
			return false;
		}

		if (!registerMemoryModule("float",
		                          new cLogicConvert<tFloat,
		                                            tString>("toString",
		                                                     "float",
		                                                     "string",
			[](tFloat* from, tString* to)
			{
				*to = std::to_string(*from);
			})))
		{
			return false;
		}

		if (!registerMemoryModule("float",
		                          new cLogicConvertBool<tString,
		                                                tFloat>("fromString",
		                                                        "string",
		                                                        "float",
			[](tString* from, tFloat* to)
			{
				char* endptr;
				*to = strtod(from->c_str(), &endptr);
				if (endptr == from->c_str())
				{
					return false;
				}
				return true;
			})))
		{
			return false;
		}

		if (!registerMemoryModule("float",
		                          new cLogicConvert<tFloat,
		                                            tInteger>("toInteger",
		                                                      "float",
		                                                      memoryIntegerTypeName,
			[](tFloat* from, tInteger* to)
			{
				*to = *from;
			})))
		{
			return false;
		}

		if (!registerMemoryModule(memoryIntegerTypeName,
		                          new cLogicConvert<tInteger,
		                                            tFloat>("toFloat",
		                                                     memoryIntegerTypeName,
		                                                     "float",
			[](tInteger* from, tFloat* to)
			{
				*to = *from;
			})))
		{
			return false;
		}

		if (!registerModules(new cLogicGetArguments(this),
		                     new cLogicGetEnvironments(this),
		                     new cLogicTrue(),
		                     new cLogicFalse(),
		                     new cActionTouch(),
		                     new cActionExit(this),
		                     new cActionWait()))
		{
			return false;
		}

		return true;
	}

private:
	std::vector<tString> arguments;
	std::map<tString, tString> environments;

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

	class cLogicBooleanSetTrue : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicBooleanSetTrue();
		}

		bool registerModule() override
		{
			setModuleName("setTrue");
			setCaptionName("setTrue");

			if (!registerSignalEntry("signal", &cLogicBooleanSetTrue::signalEntry))
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
				*to = true;
			}
			return signalFlow(signalExit);
		}

	private:
		const tSignalExitId signalExit = 1;

	private:
		tBoolean* to;
	};

	class cLogicBooleanSetFalse : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicBooleanSetFalse();
		}

		bool registerModule() override
		{
			setModuleName("setFalse");
			setCaptionName("setFalse");

			if (!registerSignalEntry("signal", &cLogicBooleanSetFalse::signalEntry))
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
				*to = false;
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

	class cActionTouch : public cActionModule
	{
	public:
		cModule* clone() const override
		{
			return new cActionTouch();
		}

		bool registerModule() override
		{
			setModuleName("touch");

			if (!registerSignalEntry("signal", signalEntryTouch))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry(const tSignalEntryId& signalEntryId) override
		{
			thread.run(this);
			return true;
		}

		void run() override
		{
			signalFlow(signalExit);
		}

	private:
		const tSignalEntryId signalEntryTouch = 1;

		const tSignalExitId signalExit = 1;
	};

	class cActionExit : public cActionModule
	{
	public:
		cActionExit(cBase* library) :
		        library(library)
		{
		}

		cModule* clone() const override
		{
			return new cActionExit(library);
		}

		bool registerModule() override
		{
			setModuleName("exit");

			if (!registerSignalEntry("signal", signalEntryExit))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry(const tSignalEntryId& signalEntryId) override
		{
			thread.run(this);
			return true;
		}

		void run() override
		{
			library->stopVirtualMachine();
		}

	private:
		cBase* library;

	private:
		const tSignalEntryId signalEntryExit = 1;
	};

	class cActionWait : public cActionModule
	{
	public:
		cModule* clone() const override
		{
			return new cActionWait();
		}

		bool registerModule() override
		{
			setModuleName("wait");

			if (!registerSignalEntry("startOrRestart", signalEntryStart))
			{
				return false;
			}

			if (!registerSignalEntry("startOrContinue", signalEntryContinue))
			{
				return false;
			}

			if (!registerMemoryEntry("milliseconds", "integer", milliseconds))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry(const tSignalEntryId& signalEntryId) override
		{
			if (!milliseconds)
			{
				return false;
			}

			if (signalEntryId == signalEntryStart)
			{
				if (thread.isRunning())
				{
					thread.stop();
				}
				thread.run(this);
				return true;
			}
			else if (signalEntryId == signalEntryContinue)
			{
				if (!thread.isRunning())
				{
					thread.run(this);
				}
				return true;
			}

			return false;
		}

		void run() override
		{
			usleep((*milliseconds) * 1000); ///< @todo: getMemory (!) */
			signalFlow(signalExit);
		}

	private:
		const tSignalEntryId signalEntryStart = 1;
		const tSignalEntryId signalEntryContinue = 2;

		const tSignalExitId signalExit = 1;

	private:
		tInteger* milliseconds; ///< @todo: getMemory (!) */
	};
};

}

}

#endif // TVM_LIBRARY_BASE_H
