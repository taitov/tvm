// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include <nlohmann/json.hpp>

#include "../converter.h"
#include "../stream.h"
#include "../fileformat.h"

namespace tvm
{

namespace
{

using tConverterModuleId = std::string;
using tConverterModuleName = std::string;
using tConverterMemoryModuleName = std::string;
using tConverterRootModuleName = std::string;
using tConverterLogicModuleName = std::string;
using tConverterModuleType = std::string;
using tConverterSignalExitName = std::string;
using tConverterSignalEntryName = std::string;
using tConverterMemoryEntryName = std::string;
using tConverterMemoryExitName = std::string;
using tConverterMemoryEntryExitName = std::string;
using tConverterTargetType = std::string;

using tConverterRootSignalExit = std::tuple<tConverterSignalExitName,
                                            tRootSignalExitId,
                                            tConverterModuleId,
                                            tConverterSignalEntryName>;

using tConverterSignalExit = std::tuple<tConverterSignalExitName,
                                        tConverterModuleId,
                                        tConverterSignalEntryName>;

using tConverterMemoryEntry = std::tuple<tConverterMemoryEntryName,
                                         tMemoryEntryId,
                                         eMemoryPosition,
                                         std::variant<tConverterModuleId,
                                                      std::tuple<tConverterRootModuleName,
                                                                 tConverterMemoryExitName>,
                                                      tMemoryModuleId>>;

using tConverterMemoryExit = std::tuple<tConverterMemoryExitName,
                                        tMemoryExitId,
                                        eMemoryPosition,
                                        tConverterModuleId>;

using tConverterMemoryEntryExit = std::tuple<tMemoryEntryExitName,
                                             tMemoryEntryExitId,
                                             eMemoryPosition,
                                             tConverterModuleId>;

}

//

class cProjectJson;
class cSchemeJson;

//

class cConverterJson : public cConverter
{
public:
	std::vector<uint8_t> getSpecification() const override;

	eResult convertToBinary(const std::vector<uint8_t>& from,
	                        std::vector<uint8_t>& to) const override;

	eResult convert(const std::string& fromFilePath,
	                const std::string& toFilePath) const override;

protected:
	friend class cProjectJson;
	friend class cSchemeJson;

	eResult convertValueToStream(cStreamOut& stream,
	                             const tConverterMemoryModuleName& converterMemoryModuleName,
	                             const nlohmann::json& jsonValue) const;

	eResult convertValue(const tConverterMemoryModuleName& converterMemoryModuleName,
	                     const nlohmann::json& jsonValue,
	                     std::vector<uint8_t>& data) const;

	tConverterModuleType getModuleType(const tConverterModuleName& converterModuleName) const;

	bool isMemoryModule(const tConverterMemoryModuleName& converterMemoryModuleName) const;
	bool isRootModule(const tConverterRootModuleName& converterRootModuleName) const;
	bool isLogicModule(const tConverterLogicModuleName& converterLogicModuleName) const;

	bool isRootSignalExit(const tConverterRootModuleName& converterRootModuleName,
	                      const tConverterSignalExitName& converterRootSignalExitName) const;

	bool isRootMemoryExit(const tConverterRootModuleName& converterRootModuleName,
	                      const tConverterMemoryExitName& converterRootMemoryExitName) const;

	bool isSignalEntry(const tConverterModuleName& converterLogicModuleName,
	                   const tConverterSignalEntryName& converterSignalEntryName) const;

	bool isSignalExit(const tConverterModuleName& converterLogicModuleName,
	                  const tConverterSignalExitName& converterSignalExitName) const;

	eResult getMemoryEntryId(const tConverterModuleName& converterLogicModuleName,
	                         const tConverterMemoryEntryName& converterMemoryEntryName,
	                         tMemoryEntryId& memoryEntryId) const;

	eResult getMemoryExitId(const tConverterModuleName& converterLogicModuleName,
	                        const tConverterMemoryExitName& converterMemoryExitName,
	                        tMemoryExitId& memoryExitId) const;

	eResult getMemoryEntryExitId(const tConverterModuleName& converterLogicModuleName,
	                             const tConverterMemoryEntryExitName& converterMemoryEntryExitName,
	                             tMemoryEntryExitId& memoryEntryExitId) const;

	/// @todo: eResult
	tRootSignalExitId getRootSignalExitId(const tConverterRootModuleName& converterRootModuleName,
	                                      const tConverterSignalExitName& converterRootSignalExitName) const;

	/// @todo: eResult
	tRootMemoryExitId getRootMemoryExitId(const tConverterRootModuleName& converterRootModuleName,
	                                      const tConverterMemoryExitName& converterRootMemoryExitName) const;

	/// @todo: eResult
	tConverterMemoryModuleName getRootMemoryExitModuleName(const tConverterRootModuleName& converterRootModuleName,
	                                                       const tConverterMemoryExitName& converterRootMemoryExitName) const;

	eResult getMemoryModuleName(const tConverterLogicModuleName& converterLogicModuleName,
	                            const tConverterMemoryEntryName& converterMemoryEntryName,
	                            tConverterMemoryModuleName& converterMemoryModuleName) const;

	tMemoryModuleType getMemoryModuleType(const tConverterMemoryModuleName& converterMemoryModuleName) const;
	tRootModuleType getRootModuleType(const tConverterRootModuleName& converterRootModuleName) const;
	tLogicModuleType getLogicModuleType(const tConverterLogicModuleName& converterLogicModuleName) const;

	tSignalExitId getSignalExitId(const tConverterModuleName& converterModuleName,
	                              const tConverterSignalExitName& converterSignalExitName) const;

	tSignalEntryId getSignalEntryId(const tConverterModuleName& converterModuleName,
	                                const tConverterSignalEntryName& converterSignalEntryName) const;
};

//

class cProjectJson
{
public:
	cProjectJson(const cConverterJson* converterJson);

	eResult convert(const nlohmann::json& fromJsonRoot,
	                nFileFormat::tProject& toFileProject);

protected:
	eResult parseRootMemories(const nlohmann::json& jsonRootMemories);
	eResult parseGlobalMemories(const nlohmann::json& jsonGlobalMemories);
	eResult parseCustomSchemes(const nlohmann::json& jsonCustomSchemes);
	eResult parseSchemes(const nlohmann::json& jsonSchemes);

	eResult fillFile(nFileFormat::tProject& fileProject) const;
	eResult fillFileRootModules(std::vector<nFileFormat::tRootModule>& fileRootModules) const;
	eResult fillFileGlobalMemories(std::vector<nFileFormat::tMemoryModule>& fileGlobalMemories) const;
	eResult fillFileConstMemories(std::vector<nFileFormat::tMemoryModule>& fileConstMemories) const;
	eResult fillFileSchemes(std::vector<nFileFormat::tScheme>& fileSchemes) const;
	eResult fillFileDebugInformation(nFileFormat::tDebugInformation& fileDebugInformation) const;

protected:
	friend class cSchemeJson;

	eResult createConstMemory(const tConverterMemoryModuleName& converterMemoryModuleName,
	                          const nlohmann::json& jsonValue,
	                          tMemoryModuleId& memoryModuleId);

	bool globalMemoryExist(const tConverterModuleId& converterMemoryModuleId) const;
	bool rootMemoryExist(const tConverterRootModuleName& converterRootModuleName,
	                     const tConverterMemoryExitName& converterRootMemoryExitName) const;
	bool constMemoryExist(const tMemoryModuleId& memoryModuleId) const;

	tMemoryEntryId getGlobalMemoryId(const tConverterModuleId& converterMemoryModuleId) const;
	tMemoryEntryId getRootMemoryId(const tConverterRootModuleName& converterRootModuleName,
	                               const tConverterMemoryExitName& converterRootMemoryExitName) const;

protected:
	const cConverterJson* converterJson;

	std::map<tConverterRootModuleName, ///< root module name
	         std::tuple<std::vector<std::tuple<tConverterMemoryExitName,
	                                           tConverterMemoryModuleName, ///< memory module name
	                                           tMemoryModuleType,
	                                           std::vector<uint8_t>>>>> rootModules;

	std::map<tConverterModuleId,
	         std::tuple<tConverterModuleName,
	                    tMemoryModuleType,
	                    std::vector<uint8_t>,
	                    tMemoryModuleId>> globalMemories;

	std::map<std::tuple<tConverterMemoryModuleName,
	                    nlohmann::json>,
	         std::tuple<tMemoryModuleType,
	                    std::vector<uint8_t>,
	                    tMemoryModuleId>> constMemories;

	std::vector<std::tuple<nFileFormat::tScheme>> schemes;
};

//

class cSchemeJson
{
public:
	cSchemeJson(cProjectJson* projectJson);

	eResult convert(const nlohmann::json& fromJsonScheme,
	                nFileFormat::tScheme& toFileFormatScheme);

protected:
	eResult parseModules(const nlohmann::json& jsonModules);
	eResult parseMemoryModule(const nlohmann::json& jsonMemoryModule);
	eResult parseRootModule(const nlohmann::json& jsonRootModule);
	eResult parseLogicModule(const nlohmann::json& jsonLogicModule);
	eResult parseCustomModule(const tConverterModuleId& converterModuleId,
	                          const nlohmann::json& jsonCustomModule);

	eResult fillFileScheme(nFileFormat::tScheme& fileScheme) const;
	eResult fillFileMemoryModules(std::vector<nFileFormat::tMemoryModule>& fileMemoryModules) const;
	eResult fillFileLogicModules(std::vector<nFileFormat::tLogicModule>& fileLogicModules) const;
	eResult fillFileRootModuleSignalExits(std::vector<nFileFormat::tRootModuleSignalExits>& fileRootModuleSignalExits) const;
	eResult fillFileLogicModuleSignalExits(std::vector<nFileFormat::tLogicModuleSignalExits>& fileLogicModuleSignalExits) const;
	eResult fillFileLogicModuleMemories(std::vector<nFileFormat::tLogicModuleMemories>& fileLogicModuleMemories) const;

protected:
	bool moduleExist(const tConverterModuleId& converterModuleId) const;
	bool memoryModuleExist(const tConverterModuleId& converterModuleId) const;
	bool logicModuleExist(const tConverterModuleId& converterModuleId) const;

	tLogicModuleId getLogicModuleId(const tConverterModuleId& converterModuleId) const;
	tMemoryEntryId getMemoryModuleId(const tConverterModuleId& converterModuleId) const;

	tConverterModuleName getConverterModuleName(const tConverterModuleId& converterModuleId) const;

protected:
	cProjectJson* projectJson;

	std::map<tConverterModuleId,
	         std::tuple<tConverterModuleName,
	                    tMemoryModuleType,
	                    std::vector<uint8_t>,
	                    tMemoryModuleId>> memoryModules;

	std::map<tConverterModuleId,
	         std::tuple<tConverterModuleName,
	                    std::vector<tConverterRootSignalExit>>> rootModules;

	std::map<tConverterModuleId,
	         std::tuple<tConverterModuleName,
	                    tLogicModuleType,
	                    tLogicModuleId,
	                    std::vector<tConverterSignalExit>,
	                    std::vector<tConverterMemoryEntry>,
	                    std::vector<tConverterMemoryExit>,
	                    std::vector<tConverterMemoryEntryExit>>> logicModules;
};

//

inline std::vector<uint8_t> cConverterJson::getSpecification() const
{
	nlohmann::json root;

	for (const auto& iter : converterMemoryModules)
	{
		nlohmann::json memoryModule;

		memoryModule["moduleName"] = std::get<0>(iter).getString();
		memoryModule["valueType"] = ""; ///< @todo: std::get<1>(iter).getString();

		root["memoryModules"].emplace_back(memoryModule);
	}

	for (const auto& iter : converterRootModules)
	{
		nlohmann::json rootModule;

		rootModule["moduleName"] = std::get<0>(iter).getString();
		rootModule["captionName"] = std::get<1>(iter).getString();

		for (const auto& iter : std::get<2>(iter))
		{
			rootModule["signalExitNames"].emplace_back(std::get<0>(iter).getString());
		}

		for (const auto& iter : std::get<3>(iter))
		{
			nlohmann::json memoryExit;

			memoryExit["memoryExitName"] = std::get<0>(iter).getString();
			memoryExit["moduleName"] = std::get<1>(iter).getString();

			rootModule["memoryExits"].emplace_back(memoryExit);
		}

		root["rootModules"].emplace_back(rootModule);
	}

	for (const auto& iter : converterLogicModules)
	{
		nlohmann::json logicModule;

		logicModule["moduleName"] = std::get<0>(iter).getString();
		logicModule["captionName"] = std::get<1>(iter).getString();

		for (const auto& iter : std::get<2>(iter))
		{
			logicModule["signalEntryNames"].emplace_back(iter.getString());
		}

		for (const auto& iter : std::get<3>(iter))
		{
			nlohmann::json memoryEntry;

			memoryEntry["memoryEntryName"] = std::get<0>(iter).getString();
			memoryEntry["memoryTypeName"] = std::get<1>(iter).getString();

			logicModule["memoryEntries"].emplace_back(memoryEntry);
		}

		for (const auto& iter : std::get<4>(iter))
		{
			logicModule["signalExitNames"].emplace_back(iter.getString());
		}

		for (const auto& iter : std::get<5>(iter))
		{
			nlohmann::json memoryExit;

			memoryExit["memoryExitName"] = std::get<0>(iter).getString();
			memoryExit["memoryTypeName"] = std::get<1>(iter).getString();

			logicModule["memoryExits"].emplace_back(memoryExit);
		}

		for (const auto& iter : std::get<6>(iter))
		{
			nlohmann::json memoryEntryExit;

			memoryEntryExit["memoryEntryExitName"] = std::get<0>(iter).getString();
			memoryEntryExit["memoryTypeName"] = std::get<1>(iter).getString();

			logicModule["memoryEntryExits"].emplace_back(memoryEntryExit);
		}

		root["logicModules"].emplace_back(logicModule);
	}

	std::string dump = root.dump();
	return std::vector<uint8_t>(dump.begin(), dump.end());
}

inline eResult cConverterJson::convertToBinary(const std::vector<uint8_t>& from,
                                               std::vector<uint8_t>& to) const
{
	/// @todo
	(void)from;
	(void)to;
	return eResult::notImplemented;
}

inline eResult cConverterJson::convert(const std::string& fromFilePath,
                                       const std::string& toFilePath) const
{
	cStreamOut stream;
	stream.push(nFileFormat::headerMagic);

	{
		std::ifstream fromFileStream(fromFilePath);
		if (!fromFileStream.is_open())
		{
			TVM_LOG_ERROR("can't open file '%s'\n", fromFilePath.data());
			return eResult::fileNotFound;
		}

		nFileFormat::tProject fileProject;

		{
			nlohmann::json root = nlohmann::json::parse(fromFileStream);

			cProjectJson projectJson(this);
			eResult result = projectJson.convert(root, fileProject);
			if (result != eResult::success)
			{
				return result;
			}
		}

		stream.push(fileProject);
	}

	std::ofstream toFileStream;
	toFileStream.open(toFilePath, std::ofstream::out | std::ofstream::binary);
	if (!toFileStream.is_open())
	{
		TVM_LOG_ERROR("can't open file '%s'\n", toFilePath.data());
		return eResult::errorCreateFile;
	}

	{
		const std::vector<uint8_t>& buffer = stream.getBuffer();
		toFileStream.write((const char*)buffer.data(), buffer.size());
	}

	return eResult::success;
}

template<typename TType>
inline static void convertIntegerToStream(cStreamOut& stream,
                                          const nlohmann::json& jsonValue)
{
	TType value = 0;
	if (!jsonValue.is_null())
	{
		value =  std::stoll(std::string(jsonValue), 0, 0);
	}
	stream.push(value);
}

template<typename TType>
inline static void convertUnsignedIntegerToStream(cStreamOut& stream,
                                                  const nlohmann::json& jsonValue)
{
	TType value = 0;
	if (!jsonValue.is_null())
	{
		value = std::stoull(std::string(jsonValue), 0, 0);
	}
	stream.push(value);
}

eResult cConverterJson::convertValueToStream(cStreamOut& stream,
                                             const tConverterMemoryModuleName& converterMemoryModuleName,
                                             const nlohmann::json& jsonValue) const
{
	eResult result = eResult::success;

	if (!isMemoryModule(converterMemoryModuleName))
	{
		TVM_LOG_ERROR("unknown converterMemoryModuleName: '%s'\n", converterMemoryModuleName.data());
		return eResult::unknownModuleName;
	}

	tMemoryModuleType memoryModuleType = getMemoryModuleType(converterMemoryModuleName);

	const cMemoryValueType& memoryValueType = std::get<1>(converterMemoryModules[memoryModuleType]);
	if (memoryValueType.isValue())
	{
		if (memoryValueType.valueGetType() == "string")
		{
			std::string value;
			if (!jsonValue.is_null())
			{
				value = jsonValue;
			}
			stream.push(value);
		}
		else if (memoryValueType.valueGetType() == "int8")
		{
			convertIntegerToStream<int8_t>(stream, jsonValue);
		}
		else if (memoryValueType.valueGetType() == "int16")
		{
			convertIntegerToStream<int16_t>(stream, jsonValue);
		}
		else if (memoryValueType.valueGetType() == "int32")
		{
			convertIntegerToStream<int32_t>(stream, jsonValue);
		}
		else if (memoryValueType.valueGetType() == "int64")
		{
			convertIntegerToStream<int64_t>(stream, jsonValue);
		}
		else if (memoryValueType.valueGetType() == "uint8")
		{
			convertUnsignedIntegerToStream<uint8_t>(stream, jsonValue);
		}
		else if (memoryValueType.valueGetType() == "uint16")
		{
			convertUnsignedIntegerToStream<uint16_t>(stream, jsonValue);
		}
		else if (memoryValueType.valueGetType() == "uint32")
		{
			convertUnsignedIntegerToStream<uint32_t>(stream, jsonValue);
		}
		else if (memoryValueType.valueGetType() == "uint64")
		{
			convertUnsignedIntegerToStream<uint64_t>(stream, jsonValue);
		}
		else
		{
			TVM_LOG_ERROR("unknown valueGetType: %s\n", memoryValueType.valueGetType().data());
			return eResult::unknownType;
		}
	}
	else if (memoryValueType.isVector())
	{
		/// is sensitive to cStreamOut implementation
		/// @todo: rewrite

		cStreamOut::tInteger size = jsonValue.size();
		stream.push(size);

		for (const nlohmann::json& jsonIter : jsonValue)
		{
			tConverterMemoryModuleName iterConverterMemoryModuleName = ":memory:";
			iterConverterMemoryModuleName += memoryValueType.vectorGetMemoryModule();

			result = convertValueToStream(stream,
			                              iterConverterMemoryModuleName,
			                              jsonIter["value"]);
			if (result != eResult::success)
			{
				return result;
			}
		}
	}
	else if (memoryValueType.isTuple())
	{
		/// is sensitive to cStreamOut implementation
		/// @todo: rewrite

		const auto& tupleMemoryModules = memoryValueType.tupleGetMemoryModules();
		for (const auto& iter : tupleMemoryModules)
		{
			const std::string& iterValueName = std::get<0>(iter);
			const std::string& iterMemoryModuleName = std::get<1>(iter);

			tConverterMemoryModuleName iterConverterMemoryModuleName = ":memory:";
			iterConverterMemoryModuleName += iterMemoryModuleName;

			bool found = false;
			for (const nlohmann::json& jsonIter : jsonValue)
			{
				if (jsonIter["valueName"] == iterValueName)
				{
					result = convertValueToStream(stream,
					                              iterConverterMemoryModuleName,
					                              jsonIter["value"]);
					if (result != eResult::success)
					{
						return result;
					}

					found = true;
					break;
				}
			}

			if (!found)
			{
				result = convertValueToStream(stream,
				                              iterConverterMemoryModuleName,
				                              nlohmann::json{});
				if (result != eResult::success)
				{
					return result;
				}
			}
		}
	}
	else if (memoryValueType.isEnum())
	{
		/// is sensitive to cStreamOut implementation
		/// @todo: rewrite

		if (!jsonValue.is_null())
		{
			std::string stringValue = jsonValue;

			for (const auto& iter : memoryValueType.enumGetValues())
			{
				if (std::get<0>(iter) == stringValue)
				{
					const size_t& enumValue = std::get<1>(iter);

					size_t enumSize = memoryValueType.enumGetSize();
					if (enumSize == 1)
					{
						uint8_t value = enumValue;
						stream.push(value);
					}
					else if (enumSize == 2)
					{
						uint16_t value = enumValue;
						stream.push(value);
					}
					else if (enumSize == 4)
					{
						uint32_t value = enumValue;
						stream.push(value);
					}
					else if (enumSize == 8)
					{
						uint64_t value = enumValue;
						stream.push(value);
					}
					else
					{
						TVM_LOG_ERROR("wrong enumSize\n");
						return eResult::unknownType;
					}

					return eResult::success;
				}
			}

			TVM_LOG_ERROR("unknown enum %s %s\n", converterMemoryModuleName.data(), stringValue.data());
			return eResult::unknownType;
		}
	}
	else
	{
		TVM_LOG_ERROR("unknown valueType\n");
		return eResult::unknownType;
	}

	return result;
}

eResult cConverterJson::convertValue(const tConverterMemoryModuleName& converterMemoryModuleName,
                                     const nlohmann::json& jsonValue,
                                     std::vector<uint8_t>& data) const
{
	eResult result = eResult::success;

	cStreamOut stream;

	result = convertValueToStream(stream,
	                              converterMemoryModuleName,
	                              jsonValue);
	if (result != eResult::success)
	{
		return result;
	}

	data = stream.getBuffer();

	return result;
}

tConverterModuleType cConverterJson::getModuleType(const tConverterModuleName& converterModuleName) const
{
	/// @todo: rewrite

	tConverterModuleType converterModuleType = converterModuleName.substr(converterModuleName.find(':') + 1);
	return converterModuleType.substr(0, converterModuleType.find(':'));
}

bool cConverterJson::isMemoryModule(const tConverterMemoryModuleName& converterMemoryModuleName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterMemoryModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterMemoryModuleName)
		{
			return true;
		}
	}

	return false;
}

bool cConverterJson::isRootModule(const tConverterRootModuleName& converterRootModuleName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterRootModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterRootModuleName)
		{
			return true;
		}
	}

	return false;
}

bool cConverterJson::isLogicModule(const tConverterLogicModuleName& converterLogicModuleName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterLogicModuleName)
		{
			return true;
		}
	}

	return false;
}

bool cConverterJson::isRootSignalExit(const tConverterRootModuleName& converterRootModuleName,
                                      const tConverterSignalExitName& converterRootSignalExitName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterRootModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterRootModuleName)
		{
			const std::vector<std::tuple<tSignalExitName,
			                             tRootSignalExitId>>& iterSignalExitNames = std::get<2>(iter);

			for (const auto& iter : iterSignalExitNames)
			{
				const tSignalExitName& signalExitName = std::get<0>(iter);

				if (signalExitName.getString() == converterRootSignalExitName)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool cConverterJson::isRootMemoryExit(const tConverterRootModuleName& converterRootModuleName,
                                      const tConverterMemoryExitName& converterRootMemoryExitName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterRootModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterRootModuleName)
		{
			const std::vector<std::tuple<tMemoryExitName,
			                             tModuleName>>& iterMemoryExitNames = std::get<3>(iter);

			for (const auto& iter : iterMemoryExitNames)
			{
				const tMemoryExitName& memoryExitName = std::get<0>(iter);

				if (memoryExitName.getString() == converterRootMemoryExitName)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool cConverterJson::isSignalEntry(const tConverterModuleName& converterLogicModuleName, const tConverterSignalEntryName& converterSignalEntryName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterLogicModuleName)
		{
			const std::vector<tSignalEntryName>& iterSignalEntryNames = std::get<2>(iter);

			for (const auto& iter : iterSignalEntryNames)
			{
				const tSignalEntryName& signalEntryName = iter;

				if (signalEntryName.getString() == converterSignalEntryName)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool cConverterJson::isSignalExit(const tConverterModuleName& converterLogicModuleName,
                                  const tConverterSignalExitName& converterSignalExitName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterLogicModuleName)
		{
			const std::vector<tSignalExitName>& iterSignalExitNames = std::get<4>(iter);

			for (const auto& iter : iterSignalExitNames)
			{
				const tSignalExitName& signalExitName = iter;

				if (signalExitName.getString() == converterSignalExitName)
				{
					return true;
				}
			}
		}
	}

	return false;
}

eResult cConverterJson::getMemoryEntryId(const tConverterModuleName& converterLogicModuleName,
                                         const tConverterMemoryEntryName& converterMemoryEntryName,
                                         tMemoryEntryId& memoryEntryId) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterLogicModuleName)
		{
			const auto& iterMemoryNames = std::get<3>(iter);

			memoryEntryId = 0;
			for (const auto& iter : iterMemoryNames)
			{
				const tMemoryEntryName& memoryName = std::get<0>(iter);

				if (memoryName.getString() == converterMemoryEntryName)
				{
					return eResult::success;
				}

				memoryEntryId++;
			}

			return eResult::invalidMemoryEntry;
		}
	}

	return eResult::invalidMemoryEntry;
}

eResult cConverterJson::getMemoryExitId(const tConverterModuleName& converterLogicModuleName,
                                        const tConverterMemoryExitName& converterMemoryExitName,
                                        tMemoryExitId& memoryExitId) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterLogicModuleName)
		{
			const auto& iterMemoryNames = std::get<5>(iter);

			memoryExitId = 0;
			for (const auto& iter : iterMemoryNames)
			{
				const tMemoryExitName& memoryName = std::get<0>(iter);

				if (memoryName.getString() == converterMemoryExitName)
				{
					return eResult::success;
				}

				memoryExitId++;
			}

			return eResult::invalidMemoryExit;
		}
	}

	return eResult::invalidMemoryExit;
}

eResult cConverterJson::getMemoryEntryExitId(const tConverterModuleName& converterLogicModuleName,
                                             const tConverterMemoryEntryExitName& converterMemoryEntryExitName,
                                             tMemoryEntryExitId& memoryEntryExitId) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterLogicModuleName)
		{
			const auto& iterMemoryNames = std::get<6>(iter);

			memoryEntryExitId = 0;
			for (const auto& iter : iterMemoryNames)
			{
				const tMemoryEntryExitName& memoryName = std::get<0>(iter);

				if (memoryName.getString() == converterMemoryEntryExitName)
				{
					return eResult::success;
				}

				memoryEntryExitId++;
			}

			return eResult::invalidMemoryEntryExit;
		}
	}

	return eResult::invalidMemoryEntryExit;
}

tRootSignalExitId cConverterJson::getRootSignalExitId(const tConverterRootModuleName& converterRootModuleName,
                                                      const tConverterSignalExitName& converterRootSignalExitName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterRootModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterRootModuleName)
		{
			const std::vector<std::tuple<tSignalExitName,
			                             tRootSignalExitId>>& iterSignalExitNames = std::get<2>(iter);

			for (const auto& iter : iterSignalExitNames)
			{
				const tSignalExitName& signalExitName = std::get<0>(iter);

				if (signalExitName.getString() == converterRootSignalExitName)
				{
					return std::get<1>(iter);
				}
			}
		}
	}

	return 0;
}

tRootMemoryExitId cConverterJson::getRootMemoryExitId(const tConverterRootModuleName& converterRootModuleName,
                                                      const tConverterMemoryExitName& converterRootMemoryExitName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterRootModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterRootModuleName)
		{
			const auto& iterMemoryExitNames = std::get<2>(iter);

			for (const auto& iter : iterMemoryExitNames)
			{
				const tMemoryExitName& memoryExitName = std::get<0>(iter);

				if (memoryExitName.getString() == converterRootMemoryExitName)
				{
					return std::get<1>(iter);
				}
			}
		}
	}

	return 0;
}

tConverterMemoryModuleName cConverterJson::getRootMemoryExitModuleName(const tConverterRootModuleName& converterRootModuleName,
                                                                       const tConverterMemoryExitName& converterRootMemoryExitName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterRootModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterRootModuleName)
		{
			const std::vector<std::tuple<tMemoryExitName,
			                             tModuleName>>& iterMemoryExitNames = std::get<3>(iter);

			for (const auto& iter : iterMemoryExitNames)
			{
				const tMemoryExitName& memoryExitName = std::get<0>(iter);

				if (memoryExitName.getString() == converterRootMemoryExitName)
				{
					return std::get<1>(iter).getString();
				}
			}
		}
	}

	return "";
}

eResult cConverterJson::getMemoryModuleName(const tConverterLogicModuleName& converterLogicModuleName,
                                            const tConverterMemoryEntryName& converterMemoryEntryName,
                                            tConverterMemoryModuleName& converterMemoryModuleName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterLogicModuleName)
		{
			const std::vector<std::tuple<tMemoryEntryName,
			                             tModuleName>>& iterMemoryEntryNames = std::get<3>(iter);

			for (const auto& iter : iterMemoryEntryNames)
			{
				const tMemoryEntryName& memoryEntryName = std::get<0>(iter);

				if (memoryEntryName.getString() == converterMemoryEntryName)
				{
					converterMemoryModuleName = std::get<1>(iter).getString();
					return eResult::success;
				}
			}
		}
	}

	return eResult::invalidMemoryEntry;
}

tMemoryModuleType cConverterJson::getMemoryModuleType(const tConverterMemoryModuleName& converterMemoryModuleName) const
{
	/// @todo: rewrite

	tMemoryModuleType memoryModuleType = 0;
	for (const auto& iter : converterMemoryModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterMemoryModuleName)
		{
			return memoryModuleType;
		}

		memoryModuleType++;
	}

	return 0;
}

tRootModuleType cConverterJson::getRootModuleType(const tConverterRootModuleName& converterRootModuleName) const
{
	/// @todo: rewrite

	tRootModuleType rootModuleType = 0;
	for (const auto& iter : converterRootModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterRootModuleName)
		{
			return rootModuleType;
		}

		rootModuleType++;
	}

	return 0;
}

tLogicModuleType cConverterJson::getLogicModuleType(const tConverterLogicModuleName& converterLogicModuleName) const
{
	/// @todo: rewrite

	tLogicModuleType logicModuleType = 0;
	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterLogicModuleName)
		{
			return logicModuleType;
		}

		logicModuleType++;
	}

	return 0;
}

tSignalExitId cConverterJson::getSignalExitId(const tConverterModuleName& converterModuleName,
                                              const tConverterSignalExitName& converterSignalExitName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterModuleName)
		{
			const std::vector<tSignalExitName>& iterSignalExitNames = std::get<4>(iter);

			tSignalExitId signalExitId = 0;
			for (const auto& iter : iterSignalExitNames)
			{
				const tSignalExitName& signalExitName = iter;

				if (signalExitName.getString() == converterSignalExitName)
				{
					return signalExitId;
				}

				signalExitId++;
			}
		}
	}

	return 0;
}

tSignalEntryId cConverterJson::getSignalEntryId(const tConverterModuleName& converterModuleName,
                                                const tConverterSignalEntryName& converterSignalEntryName) const
{
	/// @todo: rewrite

	for (const auto& iter : converterLogicModules)
	{
		const tModuleName& iterModuleName = std::get<0>(iter);

		if (iterModuleName.getString() == converterModuleName)
		{
			const std::vector<tSignalEntryName>& iterSignalEntryNames = std::get<2>(iter);

			tSignalEntryId signalEntryId = 0;
			for (const auto& iter : iterSignalEntryNames)
			{
				const tSignalEntryName& signalEntryName = iter;

				if (signalEntryName.getString() == converterSignalEntryName)
				{
					return signalEntryId;
				}

				signalEntryId++;
			}
		}
	}

	return 0;
}

//

inline cProjectJson::cProjectJson(const cConverterJson* converterJson) :
        converterJson(converterJson)
{
}

inline eResult cProjectJson::convert(const nlohmann::json& fromJsonRoot,
                                     nFileFormat::tProject& toFileProject)
{
	eResult result = eResult::success;

	if (fromJsonRoot.find("rootMemories") != fromJsonRoot.end())
	{
		result = parseRootMemories(fromJsonRoot.find("rootMemories").value());
		if (result != eResult::success)
		{
			return result;
		}
	}

	if (fromJsonRoot.find("globalMemories") != fromJsonRoot.end())
	{
		result = parseGlobalMemories(fromJsonRoot.find("globalMemories").value());
		if (result != eResult::success)
		{
			return result;
		}
	}

	if (fromJsonRoot.find("customSchemes") != fromJsonRoot.end())
	{
		result = parseCustomSchemes(fromJsonRoot.find("customSchemes").value());
		if (result != eResult::success)
		{
			return result;
		}
	}

	if (fromJsonRoot.find("schemes") != fromJsonRoot.end())
	{
		result = parseSchemes(fromJsonRoot.find("schemes").value());
		if (result != eResult::success)
		{
			return result;
		}
	}

	result = fillFile(toFileProject);
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

eResult cProjectJson::parseRootMemories(const nlohmann::json& jsonRootMemories)
{
	eResult result = eResult::success;

	for (const nlohmann::json& jsonRootMemory : jsonRootMemories)
	{
		tConverterRootModuleName converterRootModuleName = jsonRootMemory["moduleName"];

		if (!converterJson->isRootModule(converterRootModuleName))
		{
			TVM_LOG_ERROR("unknown converterRootModuleName: '%s'\n", converterRootModuleName.data());
			return eResult::unknownModuleName;
		}

		std::vector<std::tuple<tConverterMemoryExitName,
		                       tConverterMemoryModuleName,
		                       tMemoryModuleType,
		                       std::vector<uint8_t>>> rootMemoryExits;
		for (const nlohmann::json& jsonMemoryExit : jsonRootMemory["memoryExits"])
		{
			tConverterMemoryExitName converterRootMemoryExitName = jsonMemoryExit["memoryExitName"];

			if (!converterJson->isRootMemoryExit(converterRootModuleName,
			                                     converterRootMemoryExitName))
			{
				return eResult::unknownRootMemoryExit;
			}

			tConverterMemoryModuleName converterMemoryModuleName = converterJson->getRootMemoryExitModuleName(converterRootModuleName,
			                                                                                                  converterRootMemoryExitName);

			std::vector<uint8_t> data;
			if (jsonMemoryExit.find("value") != jsonMemoryExit.end())
			{
				result = converterJson->convertValue(converterMemoryModuleName,
				                                     jsonMemoryExit["value"],
				                                     data);
				if (result != eResult::success)
				{
					return result;
				}
			}

			/// @todo: check exist

			rootMemoryExits.emplace_back(converterRootMemoryExitName,
			                             converterMemoryModuleName,
			                             converterJson->getMemoryModuleType(converterMemoryModuleName),
			                             data);
		}

		/// @todo: check exist

		rootModules[converterRootModuleName] = {rootMemoryExits};
	}

	return result;
}

eResult cProjectJson::parseGlobalMemories(const nlohmann::json& jsonGlobalMemories)
{
	eResult result = eResult::success;

	for (const nlohmann::json& jsonGlobalMemory : jsonGlobalMemories)
	{
		tConverterModuleId converterMemoryModuleId = jsonGlobalMemory["id"];
		tConverterMemoryModuleName converterMemoryModuleName = jsonGlobalMemory["moduleName"];

		if (globalMemoryExist(converterMemoryModuleId))
		{
			return eResult::alreadyExist;
		}

		if (!converterJson->isMemoryModule(converterMemoryModuleName))
		{
			TVM_LOG_ERROR("unknown converterMemoryModuleName: '%s'\n", converterMemoryModuleName.data());
			return eResult::unknownModuleName;
		}

		std::vector<uint8_t> data;
		if (jsonGlobalMemory.find("value") != jsonGlobalMemory.end())
		{
			result = converterJson->convertValue(converterMemoryModuleName,
			                                     jsonGlobalMemory["value"],
			                                     data);
			if (result != eResult::success)
			{
				return result;
			}
		}

		tMemoryModuleId memoryModuleId = globalMemories.size();

		globalMemories[converterMemoryModuleId] = {converterMemoryModuleName,
		                                           converterJson->getMemoryModuleType(converterMemoryModuleName),
		                                           data,
		                                           memoryModuleId};
	}

	return result;
}

eResult cProjectJson::parseCustomSchemes(const nlohmann::json& jsonCustomSchemes)
{
	/// @todo
	(void)jsonCustomSchemes;
	return eResult::notImplemented;
}

eResult cProjectJson::parseSchemes(const nlohmann::json& jsonSchemes)
{
	eResult result = eResult::success;

	for (const nlohmann::json& jsonScheme : jsonSchemes)
	{
		nFileFormat::tScheme fileFormatScheme;

		{
			cSchemeJson schemeJson(this);

			eResult result = schemeJson.convert(jsonScheme, fileFormatScheme);
			if (result != eResult::success)
			{
				return result;
			}
		}

		schemes.emplace_back(fileFormatScheme);
	}

	return result;
}

eResult cProjectJson::fillFile(nFileFormat::tProject& fileProject) const
{
	eResult result = eResult::success;

	std::vector<nFileFormat::tRootModule>& fileRootModules = std::get<0>(fileProject);
	std::vector<nFileFormat::tMemoryModule>& fileGlobalMemories = std::get<1>(fileProject);
	std::vector<nFileFormat::tMemoryModule>& fileConstMemories = std::get<2>(fileProject);
	std::vector<nFileFormat::tScheme>& fileSchemes = std::get<3>(fileProject);
	nFileFormat::tDebugInformation& fileDebugInformation = std::get<4>(fileProject);

	result = fillFileRootModules(fileRootModules);
	if (result != eResult::success)
	{
		return result;
	}

	result = fillFileGlobalMemories(fileGlobalMemories);
	if (result != eResult::success)
	{
		return result;
	}

	result = fillFileConstMemories(fileConstMemories);
	if (result != eResult::success)
	{
		return result;
	}

	result = fillFileSchemes(fileSchemes);
	if (result != eResult::success)
	{
		return result;
	}

	result = fillFileDebugInformation(fileDebugInformation);
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

eResult cProjectJson::fillFileRootModules(std::vector<nFileFormat::tRootModule>& fileRootModules) const
{
	for (const auto& rootModule : rootModules)
	{
		const tConverterRootModuleName& converterRootModuleName = rootModule.first;

		if (!converterJson->isRootModule(converterRootModuleName))
		{
			return eResult::invalidModuleType;
		}

		tRootModuleType rootModuleType = converterJson->getRootModuleType(converterRootModuleName);

		std::vector<std::tuple<tRootMemoryExitId,
		                       nFileFormat::tMemoryModule>> fileRootMemoryModules;
		for (const auto& rootMemoryModule : std::get<0>(rootModule.second))
		{
			const tConverterMemoryExitName& converterRootMemoryExitName = std::get<0>(rootMemoryModule);

			if (!converterJson->isRootMemoryExit(converterRootModuleName,
			                                     converterRootMemoryExitName))
			{
				return eResult::unknownRootMemoryExit;
			}

			tRootMemoryExitId rootMemoryExitId = converterJson->getRootMemoryExitId(converterRootModuleName,
			                                                                        converterRootMemoryExitName);

			fileRootMemoryModules.emplace_back(rootMemoryExitId,
			                                   nFileFormat::tMemoryModule{std::get<2>(rootMemoryModule),
			                                                              std::get<3>(rootMemoryModule)});
		}

		fileRootModules.emplace_back(rootModuleType,
		                             fileRootMemoryModules);
	}

	return eResult::success;
}

eResult cProjectJson::fillFileGlobalMemories(std::vector<nFileFormat::tMemoryModule>& fileGlobalMemories) const
{
	for (const auto& iter : globalMemories)
	{
		const tMemoryModuleType& memoryModuleType = std::get<1>(iter.second);
		const std::vector<uint8_t>& data = std::get<2>(iter.second);

		fileGlobalMemories.emplace_back(memoryModuleType,
		                                data);
	}

	return eResult::success;
}

eResult cProjectJson::fillFileConstMemories(std::vector<nFileFormat::tMemoryModule>& fileConstMemories) const
{
	for (const auto& iter : constMemories)
	{
		const tMemoryModuleType& memoryModuleType = std::get<0>(iter.second);
		const std::vector<uint8_t>& data = std::get<1>(iter.second);

		fileConstMemories.emplace_back(memoryModuleType,
		                               data);
	}

	return eResult::success;
}

eResult cProjectJson::fillFileSchemes(std::vector<nFileFormat::tScheme>& fileSchemes) const
{
	for (const auto& iter : schemes)
	{
		fileSchemes.emplace_back(std::get<0>(iter));
	}

	return eResult::success;
}

eResult cProjectJson::fillFileDebugInformation(nFileFormat::tDebugInformation& fileDebugInformation) const
{
	/// @todo
	(void)fileDebugInformation;
	return eResult::success;
}

eResult cProjectJson::createConstMemory(const tConverterMemoryModuleName& converterMemoryModuleName,
                                        const nlohmann::json& jsonValue,
                                        tMemoryModuleId& memoryModuleId)
{
	eResult result = eResult::success;

	const auto& iter = constMemories.find({converterMemoryModuleName, jsonValue});
	if (iter == constMemories.end())
	{
		std::vector<uint8_t> data;
		result = converterJson->convertValue(converterMemoryModuleName,
		                                     jsonValue,
		                                     data);
		if (result != eResult::success)
		{
			return result;
		}

		memoryModuleId = constMemories.size();

		constMemories[{converterMemoryModuleName, jsonValue}] = {converterJson->getMemoryModuleType(converterMemoryModuleName),
		                                                         data,
		                                                         memoryModuleId};

		return result;
	}

	memoryModuleId = std::get<2>(iter->second);

	return result;
}

bool cProjectJson::globalMemoryExist(const tConverterModuleId& converterMemoryModuleId) const
{
	if (globalMemories.find(converterMemoryModuleId) != globalMemories.end())
	{
		return true;
	}

	return false;
}

bool cProjectJson::rootMemoryExist(const tConverterRootModuleName& converterRootModuleName,
                                   const tConverterMemoryExitName& converterRootMemoryExitName) const
{
	/// @todo: rewrite

	if (rootModules.find(converterRootModuleName) == rootModules.end())
	{
		return false;
	}

	for (const auto& iter : std::get<0>(rootModules.find(converterRootModuleName)->second))
	{
		const tConverterMemoryExitName& iterConverterRootMemoryExitName = std::get<0>(iter);

		if (iterConverterRootMemoryExitName == converterRootMemoryExitName)
		{
			return true;
		}
	}

	return false;
}

bool cProjectJson::constMemoryExist(const tMemoryModuleId& memoryModuleId) const
{
	/// @todo: rewrite

	for (const auto& iter : constMemories)
	{
		if (std::get<2>(iter.second) == memoryModuleId)
		{
			return true;
		}
	}

	return false;
}

tMemoryEntryId cProjectJson::getGlobalMemoryId(const tConverterModuleId& converterMemoryModuleId) const
{
	return std::get<3>(globalMemories.find(converterMemoryModuleId)->second);
}

tMemoryEntryId cProjectJson::getRootMemoryId(const tConverterRootModuleName& converterRootModuleName,
                                             const tConverterMemoryExitName& converterRootMemoryExitName) const
{
	/// @todo: rewrite

	tMemoryEntryId memoryEntryId = 0;

	for (const auto& iter : converterJson->converterRootModules)
	{
		tConverterRootModuleName iterConverterRootModuleName = std::get<0>(iter).getString();

		const auto& memoryExits = std::get<3>(iter);
		for (const auto& memoryExit : memoryExits)
		{
			tConverterMemoryExitName iterConverterRootMemoryExitName = std::get<0>(memoryExit).getString();

			if (iterConverterRootModuleName == converterRootModuleName &&
			    iterConverterRootMemoryExitName == converterRootMemoryExitName)
			{
				return memoryEntryId;
			}

			memoryEntryId++;
		}
	}

	return 0;
}

//

inline cSchemeJson::cSchemeJson(cProjectJson* projectJson) :
        projectJson(projectJson)
{
}

eResult cSchemeJson::convert(const nlohmann::json& fromJsonScheme,
                             nFileFormat::tScheme& toFileFormatScheme)
{
	eResult result = eResult::success;

	if (fromJsonScheme.find("modules") != fromJsonScheme.end())
	{
		result = parseModules(fromJsonScheme.find("modules").value());
		if (result != eResult::success)
		{
			return result;
		}
	}

	result = fillFileScheme(toFileFormatScheme);
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

eResult cSchemeJson::parseModules(const nlohmann::json& jsonModules)
{
	eResult result = eResult::success;

	for (const nlohmann::json& jsonModule : jsonModules)
	{
		tConverterModuleId converterModuleId = jsonModule["id"];
		tConverterModuleName converterModuleName = jsonModule["moduleName"];

		tConverterModuleType converterModuleType = projectJson->converterJson->getModuleType(converterModuleName);
		if (converterModuleType == "memory")
		{
			result = parseMemoryModule(jsonModule);
			if (result != eResult::success)
			{
				return result;
			}
		}
		else if (converterModuleType == "root")
		{
			result = parseRootModule(jsonModule);
			if (result != eResult::success)
			{
				return result;
			}
		}
		else if (converterModuleType == "logic")
		{
			result = parseLogicModule(jsonModule);
			if (result != eResult::success)
			{
				return result;
			}
		}
		else if (converterModuleType == "custom")
		{
			result = parseCustomModule(converterModuleId,
			                           jsonModule);
			if (result != eResult::success)
			{
				return result;
			}
		}
		else
		{
			return eResult::invalidModuleType;
		}
	}

	return result;
}

eResult cSchemeJson::parseMemoryModule(const nlohmann::json& jsonMemoryModule)
{
	eResult result = eResult::success;

	tConverterModuleId converterModuleId = jsonMemoryModule["id"];
	tConverterMemoryModuleName converterMemoryModuleName = jsonMemoryModule["moduleName"];

	if (moduleExist(converterModuleId))
	{
		return eResult::alreadyExist;
	}

	if (!projectJson->converterJson->isMemoryModule(converterMemoryModuleName))
	{
		TVM_LOG_ERROR("unknown converterMemoryModuleName: '%s'\n", converterMemoryModuleName.data());
		return eResult::unknownModuleName;
	}

	std::vector<uint8_t> data;
	if (jsonMemoryModule.find("value") != jsonMemoryModule.end())
	{
		result = projectJson->converterJson->convertValue(converterMemoryModuleName,
		                                                  jsonMemoryModule["value"],
		                                                  data);
		if (result != eResult::success)
		{
			return result;
		}
	}

	tMemoryModuleId memoryModuleId = memoryModules.size();

	memoryModules[converterModuleId] = {converterMemoryModuleName,
	                                    projectJson->converterJson->getMemoryModuleType(converterMemoryModuleName),
	                                    data,
	                                    memoryModuleId};

	return result;
}

eResult cSchemeJson::parseRootModule(const nlohmann::json& jsonRootModule)
{
	eResult result = eResult::success;

	tConverterModuleId converterModuleId = jsonRootModule["id"];
	tConverterRootModuleName converterRootModuleName = jsonRootModule["moduleName"];

	if (moduleExist(converterModuleId))
	{
		return eResult::alreadyExist;
	}

	if (!projectJson->converterJson->isRootModule(converterRootModuleName))
	{
		TVM_LOG_ERROR("unknown converterRootModuleName: '%s'\n", converterRootModuleName.data());
		return eResult::unknownModuleName;
	}

	std::vector<tConverterRootSignalExit> converterRootSignalExits;
	if (jsonRootModule.find("signalExits") != jsonRootModule.end())
	{
		for (const nlohmann::json& jsonSignalExit : jsonRootModule["signalExits"])
		{
			tConverterSignalExitName converterRootSignalExitName = jsonSignalExit["signalExitName"];

			if (!projectJson->converterJson->isRootSignalExit(converterRootModuleName,
			                                                  converterRootSignalExitName))
			{
				return eResult::unknownRootSignalExit;
			}

			/// @todo: targetType

			tConverterModuleId converterTargetModuleId = jsonSignalExit["target"]["id"];
			tConverterSignalEntryName converterTargetSignalEntryName = jsonSignalExit["target"]["signalEntryName"];

			converterRootSignalExits.emplace_back(converterRootSignalExitName,
			                                      projectJson->converterJson->getRootSignalExitId(converterRootModuleName,
			                                                                                      converterRootSignalExitName),
			                                      converterTargetModuleId,
			                                      converterTargetSignalEntryName);
		}
	}

	rootModules[converterModuleId] = {converterRootModuleName,
	                                  converterRootSignalExits};

	return result;
}

eResult cSchemeJson::parseLogicModule(const nlohmann::json& jsonLogicModule)
{
	eResult result = eResult::success;

	tConverterModuleId converterModuleId = jsonLogicModule["id"];
	tConverterLogicModuleName converterLogicModuleName = jsonLogicModule["moduleName"];

	if (moduleExist(converterModuleId))
	{
		return eResult::alreadyExist;
	}

	if (!projectJson->converterJson->isLogicModule(converterLogicModuleName))
	{
		TVM_LOG_ERROR("unknown converterLogicModuleName: '%s'\n", converterLogicModuleName.data());
		return eResult::unknownModuleName;
	}

	std::vector<tConverterSignalExit> converterSignalExits;
	if (jsonLogicModule.find("signalExits") != jsonLogicModule.end())
	{
		for (const nlohmann::json& jsonSignalExit : jsonLogicModule["signalExits"])
		{
			tConverterSignalExitName converterSignalExitName = jsonSignalExit["signalExitName"];

			if (!projectJson->converterJson->isSignalExit(converterLogicModuleName,
			                                              converterSignalExitName))
			{
				return eResult::unknownSignalExit;
			}

			/// @todo: targetType

			tConverterModuleId converterTargetModuleId = jsonSignalExit["target"]["id"];
			tConverterSignalEntryName converterTargetSignalEntryName = jsonSignalExit["target"]["signalEntryName"];

			converterSignalExits.emplace_back(converterSignalExitName,
			                                  converterTargetModuleId,
			                                  converterTargetSignalEntryName);
		}
	}

	std::vector<tConverterMemoryEntry> converterMemoryEntries;
	if (jsonLogicModule.find("memoryEntries") != jsonLogicModule.end())
	{
		for (const nlohmann::json& jsonMemoryEntry : jsonLogicModule["memoryEntries"])
		{
			tConverterMemoryEntryName converterMemoryEntryName = jsonMemoryEntry["memoryEntryName"];

			tMemoryEntryId memoryEntryId;
			result = projectJson->converterJson->getMemoryEntryId(converterLogicModuleName,
			                                                      converterMemoryEntryName,
			                                                      memoryEntryId);
			if (result != eResult::success)
			{
				return result;
			}

			eMemoryPosition memoryPosition = eMemoryPosition::scheme;
			if (jsonMemoryEntry["target"].find("targetType") != jsonMemoryEntry["target"].end())
			{
				tConverterTargetType converterTargetType = jsonMemoryEntry["target"]["targetType"];
				if (converterTargetType == "scheme")
				{
					memoryPosition = eMemoryPosition::scheme;

					tConverterModuleId converterTargetMemoryModuleId = jsonMemoryEntry["target"]["id"];

					converterMemoryEntries.emplace_back(converterMemoryEntryName,
					                                    memoryEntryId,
					                                    memoryPosition,
					                                    converterTargetMemoryModuleId);
				}
				else if (converterTargetType == "global")
				{
					memoryPosition = eMemoryPosition::global;

					tConverterModuleId converterTargetMemoryModuleId = jsonMemoryEntry["target"]["id"];

					converterMemoryEntries.emplace_back(converterMemoryEntryName,
					                                    memoryEntryId,
					                                    memoryPosition,
					                                    converterTargetMemoryModuleId);
				}
				else if (converterTargetType == "root")
				{
					memoryPosition = eMemoryPosition::root;

					std::tuple<tConverterRootModuleName,
					           tConverterMemoryExitName> converterRootMemory = {jsonMemoryEntry["target"]["moduleName"],
					                                                            jsonMemoryEntry["target"]["memoryExitName"]};

					converterMemoryEntries.emplace_back(converterMemoryEntryName,
					                                    memoryEntryId,
					                                    memoryPosition,
					                                    converterRootMemory);
				}
				else if (converterTargetType == "const")
				{
					memoryPosition = eMemoryPosition::constant;

					tConverterMemoryModuleName converterMemoryModuleName;
					result = projectJson->converterJson->getMemoryModuleName(converterLogicModuleName,
					                                                         converterMemoryEntryName,
					                                                         converterMemoryModuleName);
					if (result != eResult::success)
					{
						return result;
					}

					tMemoryModuleId memoryModuleId;
					result = projectJson->createConstMemory(converterMemoryModuleName,
					                                        jsonMemoryEntry["target"]["value"],
					                                        memoryModuleId);
					if (result != eResult::success)
					{
						return result;
					}

					converterMemoryEntries.emplace_back(converterMemoryEntryName,
					                                    memoryEntryId,
					                                    memoryPosition,
					                                    memoryModuleId);
				}
				else
				{
					TVM_LOG_ERROR("invalid converterTargetType: '%s'\n", converterTargetType.data());
					return eResult::invalidTargetType;
				}
			}
			else /// targetType == scheme (default)
			{
				memoryPosition = eMemoryPosition::scheme;

				tConverterModuleId converterTargetMemoryModuleId = jsonMemoryEntry["target"]["id"];

				converterMemoryEntries.emplace_back(converterMemoryEntryName,
				                                    memoryEntryId,
				                                    memoryPosition,
				                                    converterTargetMemoryModuleId);
			}
		}
	}

	std::vector<tConverterMemoryExit> converterMemoryExits;
	if (jsonLogicModule.find("memoryExits") != jsonLogicModule.end())
	{
		for (const nlohmann::json& jsonMemoryExit : jsonLogicModule["memoryExits"])
		{
			tConverterMemoryExitName converterMemoryExitName = jsonMemoryExit["memoryExitName"];

			tMemoryExitId memoryExitId;
			result = projectJson->converterJson->getMemoryExitId(converterLogicModuleName,
			                                                     converterMemoryExitName,
			                                                     memoryExitId);
			if (result != eResult::success)
			{
				return result;
			}

			eMemoryPosition memoryPosition = eMemoryPosition::scheme;
			if (jsonMemoryExit["target"].find("targetType") != jsonMemoryExit["target"].end())
			{
				tConverterTargetType converterTargetType = jsonMemoryExit["target"]["targetType"];
				if (converterTargetType == "scheme")
				{
					memoryPosition = eMemoryPosition::scheme;
				}
				else if (converterTargetType == "global")
				{
					memoryPosition = eMemoryPosition::global;
				}
				else
				{
					TVM_LOG_ERROR("invalid converterTargetType: '%s'\n", converterTargetType.data());
					return eResult::invalidTargetType;
				}
			}

			tConverterModuleId converterTargetMemoryModuleId = jsonMemoryExit["target"]["id"];

			converterMemoryExits.emplace_back(converterMemoryExitName,
			                                  memoryExitId,
			                                  memoryPosition,
			                                  converterTargetMemoryModuleId);
		}
	}

	std::vector<tConverterMemoryEntryExit> converterMemoryEntryExits;
	if (jsonLogicModule.find("memoryEntryExits") != jsonLogicModule.end())
	{
		for (const nlohmann::json& jsonMemoryEntryExit : jsonLogicModule["memoryEntryExits"])
		{
			tConverterMemoryEntryExitName converterMemoryEntryExitName = jsonMemoryEntryExit["memoryEntryExitName"];

			tMemoryEntryExitId memoryEntryExitId;
			result = projectJson->converterJson->getMemoryEntryExitId(converterLogicModuleName,
			                                                          converterMemoryEntryExitName,
			                                                          memoryEntryExitId);
			if (result != eResult::success)
			{
				return result;
			}

			eMemoryPosition memoryPosition = eMemoryPosition::scheme;
			if (jsonMemoryEntryExit["target"].find("targetType") != jsonMemoryEntryExit["target"].end())
			{
				tConverterTargetType converterTargetType = jsonMemoryEntryExit["target"]["targetType"];
				if (converterTargetType == "scheme")
				{
					memoryPosition = eMemoryPosition::scheme;
				}
				else if (converterTargetType == "global")
				{
					memoryPosition = eMemoryPosition::global;
				}
				else
				{
					TVM_LOG_ERROR("invalid converterTargetType: '%s'\n", converterTargetType.data());
					return eResult::invalidTargetType;
				}
			}

			tConverterModuleId converterTargetMemoryModuleId = jsonMemoryEntryExit["target"]["id"];

			converterMemoryEntryExits.emplace_back(tMemoryEntryExitName(converterMemoryEntryExitName.data()),
			                                       memoryEntryExitId,
			                                       memoryPosition,
			                                       converterTargetMemoryModuleId);
		}
	}

	tLogicModuleId logicModuleId = logicModules.size();

	logicModules[converterModuleId] = {converterLogicModuleName,
	                                   projectJson->converterJson->getLogicModuleType(converterLogicModuleName),
	                                   logicModuleId,
	                                   converterSignalExits,
	                                   converterMemoryEntries,
	                                   converterMemoryExits,
	                                   converterMemoryEntryExits};

	return result;
}

eResult cSchemeJson::parseCustomModule(const tConverterModuleId& converterModuleId,
                                       const nlohmann::json& jsonCustomModule)
{
	/// @todo
	(void)converterModuleId;
	(void)jsonCustomModule;
	return eResult::notImplemented;
}

eResult cSchemeJson::fillFileScheme(nFileFormat::tScheme& fileScheme) const
{
	eResult result = eResult::success;

	std::vector<nFileFormat::tMemoryModule>& fileMemoryModules = std::get<0>(fileScheme);
	std::vector<nFileFormat::tLogicModule>& fileLogicModules = std::get<1>(fileScheme);
	std::vector<nFileFormat::tRootModuleSignalExits>& fileRootModuleSignalExits = std::get<2>(fileScheme);
	std::vector<nFileFormat::tLogicModuleSignalExits>& fileLogicModuleSignalExits = std::get<3>(fileScheme);
	std::vector<nFileFormat::tLogicModuleMemories>& fileLogicModuleMemories = std::get<4>(fileScheme);

	result = fillFileMemoryModules(fileMemoryModules);
	if (result != eResult::success)
	{
		return result;
	}

	result = fillFileLogicModules(fileLogicModules);
	if (result != eResult::success)
	{
		return result;
	}

	result = fillFileRootModuleSignalExits(fileRootModuleSignalExits);
	if (result != eResult::success)
	{
		return result;
	}

	result = fillFileLogicModuleSignalExits(fileLogicModuleSignalExits);
	if (result != eResult::success)
	{
		return result;
	}

	result = fillFileLogicModuleMemories(fileLogicModuleMemories);
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

eResult cSchemeJson::fillFileMemoryModules(std::vector<nFileFormat::tMemoryModule>& fileMemoryModules) const
{
	for (const auto& iter : memoryModules)
	{
		const tMemoryModuleType& memoryModuleType = std::get<1>(iter.second);
		const std::vector<uint8_t>& data = std::get<2>(iter.second);

		fileMemoryModules.emplace_back(memoryModuleType,
		                               data);
	}

	return eResult::success;
}

eResult cSchemeJson::fillFileLogicModules(std::vector<nFileFormat::tLogicModule>& fileLogicModules) const
{
	for (const auto& iter : logicModules)
	{
		const tLogicModuleType& logicModuleType = std::get<1>(iter.second);

		fileLogicModules.emplace_back(logicModuleType);
	}

	return eResult::success;
}

eResult cSchemeJson::fillFileRootModuleSignalExits(std::vector<nFileFormat::tRootModuleSignalExits>& fileRootModuleSignalExits) const
{
	for (const auto& iter : rootModules)
	{
		for (const auto& converterRootSignalExit : std::get<1>(iter.second))
		{
			const tRootSignalExitId& rootSignalExitId = std::get<1>(converterRootSignalExit);
			const tConverterModuleId& converterTargetLogicModuleId = std::get<2>(converterRootSignalExit);
			const tConverterSignalEntryName& converterTargetSignalEntryName = std::get<3>(converterRootSignalExit);

			if (!logicModuleExist(converterTargetLogicModuleId))
			{
				TVM_LOG_ERROR("invalid converterTargetLogicModuleId: '%s'\n", converterTargetLogicModuleId.data());
				return eResult::invalidModuleId;
			}

			tConverterModuleName converterTargetModuleName = getConverterModuleName(converterTargetLogicModuleId);

			if (!projectJson->converterJson->isSignalEntry(converterTargetModuleName,
			                                               converterTargetSignalEntryName))
			{
				return eResult::invalidSignalEntryId;
			}

			tLogicModuleId targetLogicModuleId = getLogicModuleId(converterTargetLogicModuleId);
			tSignalEntryId targetSignalEntryId = projectJson->converterJson->getSignalEntryId(converterTargetModuleName,
			                                                                                  converterTargetSignalEntryName);

			fileRootModuleSignalExits.emplace_back(rootSignalExitId,
			                                       targetLogicModuleId,
			                                       targetSignalEntryId);
		}
	}

	return eResult::success;
}

eResult cSchemeJson::fillFileLogicModuleSignalExits(std::vector<nFileFormat::tLogicModuleSignalExits>& fileLogicModuleSignalExits) const
{
	for (const auto& iter : logicModules)
	{
		const tConverterModuleName& converterModuleName = std::get<0>(iter.second);
		const tLogicModuleId& logicModuleId = std::get<2>(iter.second);

		std::vector<std::tuple<tSignalExitId,
		                       tLogicModuleId,
		                       tSignalEntryId>> signalExits;
		for (const auto& converterSignalExit : std::get<3>(iter.second))
		{
			const tConverterSignalExitName& converterSignalExitName = std::get<0>(converterSignalExit);
			const tConverterModuleId& converterTargetLogicModuleId = std::get<1>(converterSignalExit);
			const tConverterSignalEntryName& converterTargetSignalEntryName = std::get<2>(converterSignalExit);

			if (!projectJson->converterJson->isSignalExit(converterModuleName,
			                                              converterSignalExitName))
			{
				return eResult::invalidSignalExitId;
			}

			tSignalExitId signalExitId = projectJson->converterJson->getSignalExitId(converterModuleName,
			                                                                         converterSignalExitName);

			if (!logicModuleExist(converterTargetLogicModuleId))
			{
				return eResult::invalidModuleId;
			}

			tConverterModuleName converterTargetModuleName = getConverterModuleName(converterTargetLogicModuleId);

			if (!projectJson->converterJson->isSignalEntry(converterTargetModuleName,
			                                               converterTargetSignalEntryName))
			{
				return eResult::invalidSignalEntryId;
			}

			tLogicModuleId targetLogicModuleId = getLogicModuleId(converterTargetLogicModuleId);
			tSignalEntryId targetSignalEntryId = projectJson->converterJson->getSignalEntryId(converterTargetModuleName,
			                                                                                  converterTargetSignalEntryName);

			signalExits.emplace_back(signalExitId,
			                         targetLogicModuleId,
			                         targetSignalEntryId);
		}

		fileLogicModuleSignalExits.emplace_back(logicModuleId,
		                                        signalExits);
	}

	return eResult::success;
}

eResult cSchemeJson::fillFileLogicModuleMemories(std::vector<nFileFormat::tLogicModuleMemories>& fileLogicModuleMemories) const
{
	for (const auto& iter : logicModules)
	{
		const tLogicModuleId& logicModuleId = std::get<2>(iter.second);

		std::vector<std::tuple<tMemoryEntryId,
		                       eMemoryPosition,
		                       tMemoryModuleId>> memoryEntries;
		for (const auto& converterMemoryEntry : std::get<4>(iter.second))
		{
			const tMemoryEntryId& memoryEntryId = std::get<1>(converterMemoryEntry);
			const eMemoryPosition& memoryPosition = std::get<2>(converterMemoryEntry);

			tMemoryModuleId targetMemoryModuleId;

			if (memoryPosition == eMemoryPosition::scheme)
			{
				const tConverterModuleId& converterTargetMemoryModuleId = std::get<tConverterModuleId>(std::get<3>(converterMemoryEntry));

				if (!memoryModuleExist(converterTargetMemoryModuleId))
				{
					return eResult::invalidModuleId;
				}

				targetMemoryModuleId = getMemoryModuleId(converterTargetMemoryModuleId);
			}
			else if (memoryPosition == eMemoryPosition::global)
			{
				const tConverterModuleId& converterTargetMemoryModuleId = std::get<tConverterModuleId>(std::get<3>(converterMemoryEntry));

				if (!projectJson->globalMemoryExist(converterTargetMemoryModuleId))
				{
					return eResult::invalidModuleId;
				}

				targetMemoryModuleId = projectJson->getGlobalMemoryId(converterTargetMemoryModuleId);
			}
			else if (memoryPosition == eMemoryPosition::root)
			{
				const std::tuple<tConverterRootModuleName,
				                 tConverterMemoryExitName>& converterRootMemory = std::get<1>(std::get<3>(converterMemoryEntry));

				if (!projectJson->rootMemoryExist(std::get<0>(converterRootMemory),
				                                  std::get<1>(converterRootMemory)))
				{
					return eResult::invalidModuleId;
				}

				targetMemoryModuleId = projectJson->getRootMemoryId(std::get<0>(converterRootMemory),
				                                                    std::get<1>(converterRootMemory));
			}
			else if (memoryPosition == eMemoryPosition::constant)
			{
				const tMemoryModuleId& memoryModuleId = std::get<tMemoryModuleId>(std::get<3>(converterMemoryEntry));

				if (!projectJson->constMemoryExist(memoryModuleId))
				{
					return eResult::invalidModuleId;
				}

				targetMemoryModuleId = memoryModuleId;
			}
			else
			{
				return eResult::invalidMemoryPosition;
			}

			memoryEntries.emplace_back(memoryEntryId,
			                           memoryPosition,
			                           targetMemoryModuleId);
		}

		std::vector<std::tuple<tMemoryExitId,
		                       eMemoryPosition,
		                       tMemoryModuleId>> memoryExits;
		for (const auto& converterMemoryExit : std::get<5>(iter.second))
		{
			const tMemoryExitId& memoryExitId = std::get<1>(converterMemoryExit);
			const eMemoryPosition& memoryPosition = std::get<2>(converterMemoryExit);
			const tConverterModuleId& converterTargetMemoryModuleId = std::get<3>(converterMemoryExit);

			tMemoryModuleId targetMemoryModuleId;

			if (memoryPosition == eMemoryPosition::scheme)
			{
				if (!memoryModuleExist(converterTargetMemoryModuleId))
				{
					return eResult::invalidModuleId;
				}

				targetMemoryModuleId = getMemoryModuleId(converterTargetMemoryModuleId);
			}
			else if (memoryPosition == eMemoryPosition::global)
			{
				if (!projectJson->globalMemoryExist(converterTargetMemoryModuleId))
				{
					return eResult::invalidModuleId;
				}

				targetMemoryModuleId = projectJson->getGlobalMemoryId(converterTargetMemoryModuleId);
			}
			else
			{
				return eResult::invalidMemoryPosition;
			}

			memoryExits.emplace_back(memoryExitId,
			                         memoryPosition,
			                         targetMemoryModuleId);
		}

		std::vector<std::tuple<tMemoryEntryExitId,
		                       eMemoryPosition,
		                       tMemoryModuleId>> memoryEntryExits;
		for (const auto& converterMemoryEntryExit : std::get<6>(iter.second))
		{
			const tMemoryEntryExitId& memoryEntryExitId = std::get<1>(converterMemoryEntryExit);
			const eMemoryPosition& memoryPosition = std::get<2>(converterMemoryEntryExit);
			const tConverterModuleId& converterTargetMemoryModuleId = std::get<3>(converterMemoryEntryExit);

			tMemoryModuleId targetMemoryModuleId;

			if (memoryPosition == eMemoryPosition::scheme)
			{
				if (!memoryModuleExist(converterTargetMemoryModuleId))
				{
					return eResult::invalidModuleId;
				}

				targetMemoryModuleId = getMemoryModuleId(converterTargetMemoryModuleId);
			}
			else if (memoryPosition == eMemoryPosition::global)
			{
				if (!projectJson->globalMemoryExist(converterTargetMemoryModuleId))
				{
					return eResult::invalidModuleId;
				}

				targetMemoryModuleId = projectJson->getGlobalMemoryId(converterTargetMemoryModuleId);
			}
			else
			{
				return eResult::invalidMemoryPosition;
			}

			memoryEntryExits.emplace_back(memoryEntryExitId,
			                              memoryPosition,
			                              targetMemoryModuleId);
		}

		fileLogicModuleMemories.emplace_back(logicModuleId,
		                                     memoryEntries,
		                                     memoryExits,
		                                     memoryEntryExits);
	}

	return eResult::success;
}

bool cSchemeJson::moduleExist(const tConverterModuleId& converterModuleId) const
{
	if (memoryModules.find(converterModuleId) != memoryModules.end())
	{
		return true;
	}

	if (rootModules.find(converterModuleId) != rootModules.end())
	{
		return true;
	}

	if (logicModules.find(converterModuleId) != logicModules.end())
	{
		return true;
	}

	return false;
}

bool cSchemeJson::memoryModuleExist(const tConverterModuleId& converterModuleId) const
{
	if (memoryModules.find(converterModuleId) != memoryModules.end())
	{
		return true;
	}

	return false;
}

bool cSchemeJson::logicModuleExist(const tConverterModuleId& converterModuleId) const
{
	if (logicModules.find(converterModuleId) != logicModules.end())
	{
		return true;
	}

	return false;
}

tLogicModuleId cSchemeJson::getLogicModuleId(const tConverterModuleId& converterModuleId) const
{
	return std::get<2>(logicModules.find(converterModuleId)->second);
}

tMemoryEntryId cSchemeJson::getMemoryModuleId(const tConverterModuleId& converterModuleId) const
{
	return std::get<3>(memoryModules.find(converterModuleId)->second);
}

tConverterModuleName cSchemeJson::getConverterModuleName(const tConverterModuleId& converterModuleId) const
{
	if (memoryModules.find(converterModuleId) != memoryModules.end())
	{
		return std::get<0>(memoryModules.find(converterModuleId)->second);
	}

	if (rootModules.find(converterModuleId) != rootModules.end())
	{
		return std::get<0>(rootModules.find(converterModuleId)->second);
	}

	if (logicModules.find(converterModuleId) != logicModules.end())
	{
		return std::get<0>(logicModules.find(converterModuleId)->second);
	}

	return tConverterModuleName();
}

}
