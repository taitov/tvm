// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_EXPORT_JSON_H
#define TVM_EXPORT_JSON_H

#include <inttypes.h>

#include <vector>
#include <algorithm>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QFileInfo>

#include <tvm/vm.h>

namespace nVirtualMachine
{

namespace nExport
{

class cJson
{
public:
	using tBoolean = bool;
	using tString = std::string;
	using tInteger = int64_t;
	using tFloat = double;

	using tGuiModuleIds = std::map<QString,
	                               tModuleId>;

public:
	static QString getFilePathOfScheme(const std::vector<QString>& customModulePaths,
	                                   const tSchemeName& schemeName)
	{
		QString schemePath = QString::fromUtf8(schemeName.value.c_str());
		schemePath.replace(':', '/');
		for (const QString& customModulePath : customModulePaths)
		{
			QString filePath = customModulePath + "/" + schemePath + ".tvmcustom";
			if (QFileInfo::exists(filePath))
			{
				return filePath;
			}
		}
		return "";
	}

	static bool readSchemes(std::map<tSchemeName, QJsonObject>& schemes,
	                        const QJsonObject& jsonObject,
	                        const std::vector<QString>& customModulePaths)
	{
		QJsonArray connectionJsonArray = jsonObject["nodes"].toArray();
		for (int i = 0; i < connectionJsonArray.size(); ++i)
		{
			QJsonObject nodeJson = connectionJsonArray[i].toObject();
			if (nodeJson.find("model") != nodeJson.end())
			{
				QJsonObject modelJson = nodeJson["model"].toObject();
				if (modelJson.find("name") != modelJson.end())
				{
					if (modelJson["name"].toString().startsWith(":custom:"))
					{
						tSchemeName schemeName = modelJson["name"].toString().mid(8).toStdString();
						if (schemes.find(schemeName) != schemes.end())
						{
							continue;
						}

						QString filePath = getFilePathOfScheme(customModulePaths,
						                                       schemeName);
						if (filePath.isEmpty())
						{
							return false;
						}

						QFile file(filePath);
						if (!file.open(QIODevice::ReadOnly))
						{
							printf("error: file.open()\n");
							continue;
						}

						QByteArray wholeFile = file.readAll();

						schemes[schemeName] = QJsonDocument::fromJson(wholeFile).object();
						if (!readSchemes(schemes,
						                 schemes[schemeName],
						                 customModulePaths))
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	static tGuiModuleIds getModuleIds(const QJsonObject& jsonObject)
	{
		tGuiModuleIds guiModuleIds;

		QJsonArray connectionJsonArray = jsonObject["nodes"].toArray();
		for (int i = 0; i < connectionJsonArray.size(); ++i)
		{
			QJsonObject nodeJson = connectionJsonArray[i].toObject();
			if (nodeJson.find("id") != nodeJson.end())
			{
				tModuleId moduleId = guiModuleIds.size() + 1;
				guiModuleIds[nodeJson["id"].toString()] = moduleId;
			}
		}

		return guiModuleIds;
	}

	static std::vector<uint8_t> exportMemoryModuleVariables(const QString& typeName,
	                                                        const QJsonObject& jsonObject)
	{
		cStreamOut stream;

		if (typeName == "boolean")
		{
			tBoolean value = jsonObject["value"].toBool();
			stream.push(value);
		}
		else if (typeName == "string")
		{
			tString value = jsonObject["value"].toString().toStdString();
			stream.push(value);
		}
		else if (typeName == "integer")
		{
			tInteger value = jsonObject["value"].toString().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (typeName == "float")
		{
			tFloat value = jsonObject["value"].toString().toDouble();
			stream.push(value);
		}

		return stream.getBuffer();
	}

	static std::vector<uint8_t> exportToMemory(const QByteArray& byteArray,
	                                           const std::vector<QString>& customModulePaths)
	{
		QJsonObject jsonObject = QJsonDocument::fromJson(byteArray).object();

		cStreamOut stream;
		stream.push(fileHeaderMagic);

		std::map<tSchemeName,
		         QJsonObject> schemes;

		schemes["main"] = jsonObject;
		if (!readSchemes(schemes,
		                 schemes["main"],
		                 customModulePaths))
		{
			return std::vector<uint8_t>();
		}

		uint32_t schemesCount = schemes.size();
		stream.push(schemesCount);

		for (const auto& iter : schemes)
		{
			tGuiModuleIds moduleIds = getModuleIds(iter.second);

			std::map<QString,
			         QJsonObject> moduleJsons;

			std::string schemeName = iter.first;
			stream.push(schemeName);

			cScheme::tLoadMemories memories;
			cScheme::tLoadModules modules;
			cScheme::tLoadCustomModules customModules;
			cScheme::tLoadSchemeSignalEntryModules schemeSignalEntryModules;
			cScheme::tLoadSchemeSignalExitModules schemeSignalExitModules;
			cScheme::tLoadSchemeMemoryEntryModules schemeMemoryEntryModules;
			cScheme::tLoadSchemeMemoryExitModules schemeMemoryExitModules;
			cScheme::tLoadRootSignalFlows rootSignalFlows;
			cScheme::tLoadRootMemoryExitFlows rootMemoryExitFlows;
			cScheme::tLoadSignalFlows signalFlows;
			cScheme::tLoadMemoryFlows memoryFlows;
			cScheme::tLoadMemoryModuleVariables memoryModuleVariables;

			QJsonArray nodesJsonArray = iter.second["nodes"].toArray();
			for (int i = 0; i < nodesJsonArray.size(); ++i)
			{
				QJsonObject nodeJson = nodesJsonArray[i].toObject();
				moduleJsons[nodeJson["id"].toString()] = nodesJsonArray[i].toObject();
				if (nodeJson.find("model") != nodeJson.end())
				{
					QJsonObject modelJson = nodeJson["model"].toObject();
					if (modelJson.find("moduleTypeName") != modelJson.end())
					{
						if (modelJson["moduleTypeName"].toString() == "memory")
						{
							QString typeName = modelJson["memoryTypeName"].toString();
							auto key = moduleIds[nodeJson["id"].toString()];
							auto value = typeName.toStdString();
							memories[key] = value;
							memoryModuleVariables[key] = exportMemoryModuleVariables(typeName,
							                                                         modelJson["variables"].toObject()[typeName].toObject());
						}
						else if (modelJson["moduleTypeName"].toString() == "logic" ||
						         modelJson["moduleTypeName"].toString() == "action")
						{
							auto key = moduleIds[nodeJson["id"].toString()];
							auto value = std::make_tuple(modelJson["libraryName"].toString().toStdString(),
							                             modelJson["moduleName"].toString().toStdString());
							modules[key] = value;
						}
						else if (modelJson["moduleTypeName"].toString() == "custom")
						{
							auto key = moduleIds[nodeJson["id"].toString()];
							auto value = modelJson["schemeName"].toString().toStdString();
							customModules[key] = value;
						}
						else if (modelJson["moduleTypeName"].toString() == "scheme")
						{
							auto key = moduleIds[nodeJson["id"].toString()];
							auto value = modelJson["portName"].toString().toStdString();
							if (modelJson["direction"].toString() == "inSignal")
							{
								if (value.empty())
								{
									value = "signal";
								}
								schemeSignalEntryModules[key] = value;
							}
							else if (modelJson["direction"].toString() == "outSignal")
							{
								if (value.empty())
								{
									value = "signal";
								}
								schemeSignalExitModules[key] = value;
							}
							else if (modelJson["direction"].toString() == "inMemory")
							{
								auto portType = modelJson["portType"].toString().toStdString();
								if (value.empty())
								{
									value = portType;
								}
								schemeMemoryEntryModules[key] = value;
							}
							else if (modelJson["direction"].toString() == "outMemory")
							{
								auto portType = modelJson["portType"].toString().toStdString();
								if (value.empty())
								{
									value = portType;
								}
								schemeMemoryExitModules[key] = value;
							}
						}
					}
				}
			}

			QJsonArray connectionsJsonArray = iter.second["connections"].toArray();
			for (int i = 0; i < connectionsJsonArray.size(); ++i)
			{
				QJsonObject connectionJson = connectionsJsonArray[i].toObject();
				QJsonObject outModuleJson = moduleJsons[connectionJson["out_id"].toString()]["model"].toObject();
				QJsonObject inModuleJson = moduleJsons[connectionJson["in_id"].toString()]["model"].toObject();
				if (connectionJson["in_portType"].toString() == "signal")
				{
					if (outModuleJson["moduleTypeName"].toString() == "root")
					{
						auto key = std::make_tuple(outModuleJson["libraryName"].toString().toStdString(),
						                           outModuleJson["rootModuleName"].toString().toStdString(),
						                           connectionJson["out_portName"].toString().toStdString());
						auto value = std::make_tuple(moduleIds[connectionJson["in_id"].toString()],
						                             connectionJson["in_portName"].toString().toStdString());
						rootSignalFlows[key] = value;
					}
					else
					{
						auto key = std::make_tuple(moduleIds[connectionJson["out_id"].toString()],
						                           connectionJson["out_portName"].toString().toStdString());
						auto value = std::make_tuple(moduleIds[connectionJson["in_id"].toString()],
						                             connectionJson["in_portName"].toString().toStdString());
						signalFlows[key] = value;
					}
				}
				else
				{
					if (outModuleJson["moduleTypeName"].toString() == "root")
					{
						auto key = std::make_tuple(outModuleJson["libraryName"].toString().toStdString(),
						                           outModuleJson["rootModuleName"].toString().toStdString(),
						                           connectionJson["out_portName"].toString().toStdString());
						auto value = std::make_tuple(moduleIds[connectionJson["in_id"].toString()],
						                             connectionJson["in_portName"].toString().toStdString());
						rootMemoryExitFlows[key] = value;
					}
					else
					{
						memoryFlows.emplace_back(std::make_tuple(moduleIds[connectionJson["out_id"].toString()],
						                         connectionJson["out_portName"].toString().toStdString(),
						                         moduleIds[connectionJson["in_id"].toString()],
						                         connectionJson["in_portName"].toString().toStdString()));
					}
				}
			}

			stream.push(memories);
			stream.push(modules);
			stream.push(customModules);
			stream.push(schemeSignalEntryModules);
			stream.push(schemeSignalExitModules);
			stream.push(schemeMemoryEntryModules);
			stream.push(schemeMemoryExitModules);
			stream.push(rootSignalFlows);
			stream.push(rootMemoryExitFlows);
			stream.push(signalFlows);
			stream.push(memoryFlows);
			stream.push(memoryModuleVariables);
		}

		return stream.getBuffer();
	}
};

}

}

#endif // TVM_EXPORT_JSON_H
