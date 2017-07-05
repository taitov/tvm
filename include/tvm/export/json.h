// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_EXPORT_JSON_H
#define TVM_EXPORT_JSON_H

#include <inttypes.h>

#include <vector>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include <tvm/vm.h>

namespace nVirtualMachine
{

namespace nExport
{

class cJson
{
public:
	using tGuiModuleIds = std::map<QString,
	                               tModuleId>;

public:
	static void readSchemes(std::map<tSchemeName, QJsonObject>& schemes,
	                        const QJsonObject& jsonObject)
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

						/** @todo */
						std::string filePath = "";
//						std::string filePath = getFilePathOfScheme(std::vector<std::string>(),
//						                                           schemeName);

						if (!QFileInfo::exists(QString::fromUtf8(filePath.c_str())))
						{
							printf("error: QFileInfo::exists()\n");
							continue;
						}

						QFile file(QString::fromUtf8(filePath.c_str()));
						if (!file.open(QIODevice::ReadOnly))
						{
							printf("error: file.open()\n");
							continue;
						}

						QByteArray wholeFile = file.readAll();

						schemes[schemeName] = QJsonDocument::fromJson(wholeFile).object();
						readSchemes(schemes, schemes[schemeName]);
					}
				}
			}
		}
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

	static std::vector<uint8_t> exportMemoryModuleVariables(QJsonObject jsonObject)
	{
		cStreamOut stream;

		QString type = jsonObject["value"].toObject()["type"].toString();
		auto valueJson = jsonObject["value"].toObject()["value"];

		if (type == "std::string")
		{
			std::string value = valueJson.toString().toStdString();
			stream.push(value);
		}
		else if (type == "uint64_t")
		{
			uint64_t value = valueJson.toString().toULongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == "uint32_t")
		{
			uint32_t value = valueJson.toString().toULongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == "uint16_t")
		{
			uint16_t value = valueJson.toString().toULongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == "uint8_t")
		{
			uint8_t value = valueJson.toString().toULongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == "int64_t")
		{
			int64_t value = valueJson.toString().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == "int32_t")
		{
			int32_t value = valueJson.toString().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == "int16_t")
		{
			int16_t value = valueJson.toString().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == "int8_t")
		{
			int8_t value = valueJson.toString().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == "bool")
		{
			bool value = valueJson.toBool();
			stream.push(value);
		}
		else if (type == "double")
		{
			double value = valueJson.toString().toDouble();
			stream.push(value);
		}

		return stream.getBuffer();
	}

	static std::vector<uint8_t> exportToMemory(const QByteArray& byteArray)
	{
		QJsonObject jsonObject = QJsonDocument::fromJson(byteArray).object();

		cStreamOut stream;
		stream.push(fileHeaderMagic);

		std::map<tSchemeName,
		         QJsonObject> schemes;

		schemes["main"] = jsonObject;
		readSchemes(schemes, schemes["main"]);

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
							auto key = moduleIds[nodeJson["id"].toString()];
							auto value = modelJson["memoryTypeName"].toString().toStdString();
							memories[key] = value;
							memoryModuleVariables[key] = exportMemoryModuleVariables(modelJson["variables"].toObject());
						}
						else if (modelJson["moduleTypeName"].toString() == "logic" ||
						         modelJson["moduleTypeName"].toString() == "convert")
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
