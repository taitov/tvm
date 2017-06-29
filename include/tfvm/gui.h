// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_GUI_H
#define TFVM_GUI_H

#include <QtCore/QObject>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QInputDialog>

#include <QtGui/QIntValidator>

#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>
#include <nodes/ConnectionStyle>
#include <nodes/NodeData>
#include <nodes/NodeDataModel>

#include "vm.h"

using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::ConnectionStyle;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeStyle;

namespace nVirtualMachine
{

namespace nGui
{

class cGui
{
	friend class cIde;

public:
	using tGuiRootSignalExits = std::vector<tSignalExitName>;

	using tGuiRootMemoryExits = std::vector<std::tuple<tMemoryExitName,
	                                                   tMemoryTypeName>>;

	using tGuiModuleIds = std::map<QString,
	                               tModuleId>;

	static std::shared_ptr<QtNodes::DataModelRegistry> makeDataModelRegistry(const cVirtualMachine* virtualMachine)
	{
		auto dataModelRegistry = std::make_shared<QtNodes::DataModelRegistry>();

		const auto memoryTypes = virtualMachine->getGuiMemoryTypes();
		for (const auto& iterMemoryType : memoryTypes)
		{
			dataModelRegistry->registerModel<cMemoryDataModel>(std::make_unique<cMemoryDataModel>(iterMemoryType.first,
			                                                                                      iterMemoryType.second));
		}

		const auto rootModules = virtualMachine->getGuiRootModules();
		for (const auto& iterRootModule : rootModules)
		{
			dataModelRegistry->registerModel<cRootModuleDataModel>(std::make_unique<cRootModuleDataModel>(std::get<0>(iterRootModule.first),
			                                                                                              std::get<1>(iterRootModule.first),
			                                                                                              std::get<0>(iterRootModule.second),
			                                                                                              std::get<1>(iterRootModule.second)));
		}

		const auto modules = virtualMachine->getGuiModules();
		for (const auto& iterModule : modules)
		{
			dataModelRegistry->registerModel<cModuleDataModel>(std::make_unique<cModuleDataModel>(std::get<0>(iterModule.first),
			                                                                                      std::get<1>(iterModule.first),
			                                                                                      iterModule.second));
		}

		const auto memoryModules = virtualMachine->getGuiMemoryModules();
		for (const auto& iterMemoryModules : memoryModules)
		{
			dataModelRegistry->registerModel<cModuleDataModel>(std::make_unique<cModuleDataModel>(":memory:" + std::get<0>(iterMemoryModules.first).value,
			                                                                                      std::get<1>(iterMemoryModules.first),
			                                                                                      iterMemoryModules.second));
		}

		return dataModelRegistry;
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

	static std::string getFilePathOfScheme(const std::vector<std::string>& customsLibraries,
	                                       const tSchemeName& schemeName)
	{
		/** @todo */
		return QDir::homePath().toStdString() + "/.config/" + schemeName.value + ".json";
	}

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

						std::string filePath = getFilePathOfScheme(std::vector<std::string>(),
						                                                 schemeName);

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

		return stream.getBuffer();
	}

	static std::vector<uint8_t> exportToMemory(QJsonObject& jsonObject)
	{
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

	class QTextEditSizeHint : public QTextEdit
	{
	public:
		QSize sizeHint() const override
		{
			return QSize(128, 16);
		}
	};

	static QWidget* makeElementWidget(const std::type_index& type,
	                                  const tVariableName& variableName,
	                                  const void* defaultValue)
	{
		if (type == typeid(std::string))
		{
			std::string* value = (std::string*)defaultValue;

			QTextEditSizeHint* textEdit = new QTextEditSizeHint();
			textEdit->setPlainText(QString::fromUtf8(value->c_str()));
			textEdit->setAcceptRichText(false);
			textEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			return textEdit;
		}
		else if (type == typeid(uint64_t))
		{
			uint64_t* value = (uint64_t*)defaultValue;

			QLineEdit* lineEdit = new QLineEdit(QString::number(*value));
			lineEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			/** @todo: add validator */
			return lineEdit;
		}
		else if (type == typeid(uint32_t))
		{
			uint32_t* value = (uint32_t*)defaultValue;

			QLineEdit* lineEdit = new QLineEdit(QString::number(*value));
			lineEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			/** @todo: add validator */
			return lineEdit;
		}
		else if (type == typeid(uint16_t))
		{
			uint16_t* value = (uint16_t*)defaultValue;

			QLineEdit* lineEdit = new QLineEdit(QString::number(*value));
			lineEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			/** @todo: add validator */
			return lineEdit;
		}
		else if (type == typeid(uint8_t))
		{
			uint8_t* value = (uint8_t*)defaultValue;

			QLineEdit* lineEdit = new QLineEdit(QString::number(*value));
			lineEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			/** @todo: add validator */
			return lineEdit;
		}
		else if (type == typeid(int64_t))
		{
			int64_t* value = (int64_t*)defaultValue;

			QLineEdit* lineEdit = new QLineEdit(QString::number(*value));
			lineEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			/** @todo: add validator */
			return lineEdit;
		}
		else if (type == typeid(int32_t))
		{
			int32_t* value = (int32_t*)defaultValue;

			QLineEdit* lineEdit = new QLineEdit(QString::number(*value));
			lineEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			/** @todo: add validator */
			return lineEdit;
		}
		else if (type == typeid(int16_t))
		{
			int16_t* value = (int16_t*)defaultValue;

			QLineEdit* lineEdit = new QLineEdit(QString::number(*value));
			lineEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			/** @todo: add validator */
			return lineEdit;
		}
		else if (type == typeid(int8_t))
		{
			int8_t* value = (int8_t*)defaultValue;

			QLineEdit* lineEdit = new QLineEdit(QString::number(*value));
			lineEdit->setToolTip(QString::fromUtf8(variableName.value.c_str()));
			/** @todo: add validator */
			return lineEdit;
		}
		else if (type == typeid(bool))
		{
			bool* value = (bool*)defaultValue;

			QCheckBox* checkBox = new QCheckBox();
			checkBox->setChecked(*value);
			return checkBox;
		}

		return nullptr;
	}

	static QJsonObject saveElementWidget(const std::type_index& type,
	                                     const QWidget* widget)
	{
		if (type == typeid(std::string))
		{
			QJsonObject json;
			json["type"] = "std::string";
			QTextEditSizeHint* textEdit = (QTextEditSizeHint*)widget;
			json["value"] = QJsonValue(textEdit->toPlainText());
			return json;
		}
		else if (type == typeid(uint64_t))
		{
			QJsonObject json;
			json["type"] = "uint64_t";
			QLineEdit* lineEdit = (QLineEdit*)widget;
			json["value"] = QJsonValue(lineEdit->text());
			return json;
		}
		else if (type == typeid(uint32_t))
		{
			QJsonObject json;
			json["type"] = "uint32_t";
			QLineEdit* lineEdit = (QLineEdit*)widget;
			json["value"] = QJsonValue(lineEdit->text());
			return json;
		}
		else if (type == typeid(uint16_t))
		{
			QJsonObject json;
			json["type"] = "uint16_t";
			QLineEdit* lineEdit = (QLineEdit*)widget;
			json["value"] = QJsonValue(lineEdit->text());
			return json;
		}
		else if (type == typeid(uint8_t))
		{
			QJsonObject json;
			json["type"] = "uint8_t";
			QLineEdit* lineEdit = (QLineEdit*)widget;
			json["value"] = QJsonValue(lineEdit->text());
			return json;
		}
		else if (type == typeid(int64_t))
		{
			QJsonObject json;
			json["type"] = "int64_t";
			QLineEdit* lineEdit = (QLineEdit*)widget;
			json["value"] = QJsonValue(lineEdit->text());
			return json;
		}
		else if (type == typeid(int32_t))
		{
			QJsonObject json;
			json["type"] = "int32_t";
			QLineEdit* lineEdit = (QLineEdit*)widget;
			json["value"] = QJsonValue(lineEdit->text());
			return json;
		}
		else if (type == typeid(int16_t))
		{
			QJsonObject json;
			json["type"] = "int16_t";
			QLineEdit* lineEdit = (QLineEdit*)widget;
			json["value"] = QJsonValue(lineEdit->text());
			return json;
		}
		else if (type == typeid(int8_t))
		{
			QJsonObject json;
			json["type"] = "int8_t";
			QLineEdit* lineEdit = (QLineEdit*)widget;
			json["value"] = QJsonValue(lineEdit->text());
			return json;
		}
		else if (type == typeid(bool))
		{
			QJsonObject json;
			json["type"] = "bool";
			QCheckBox* checkBox = (QCheckBox*)widget;
			json["value"] = QJsonValue(checkBox->isChecked());
			return json;
		}

		return QJsonObject();
	}

	static void restoreElementWidget(const std::type_index& type,
	                                 const QWidget* widget,
	                                 QJsonObject jsonObject)
	{
		if (type == typeid(std::string))
		{
			QTextEditSizeHint* textEdit = (QTextEditSizeHint*)widget;
			textEdit->setPlainText(jsonObject["value"].toString());
		}
		else if (type == typeid(uint64_t) ||
		         type == typeid(uint32_t) ||
		         type == typeid(uint16_t) ||
		         type == typeid(uint8_t) ||
		         type == typeid(int64_t) ||
		         type == typeid(int32_t) ||
		         type == typeid(int16_t) ||
		         type == typeid(int8_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			lineEdit->setText(jsonObject["value"].toString());
		}
		else if (type == typeid(bool))
		{
			QCheckBox* checkBox = (QCheckBox*)widget;
			checkBox->setChecked(jsonObject["value"].toBool());
		}
	}

private:
	class cModelData
	{
	public:
		cModelData()
		{
			mainWidget = nullptr;
		}

		QJsonObject save() const
		{
			QJsonObject jsonObject;

			for (const auto& widget : widgets)
			{
				jsonObject[QString::fromUtf8(widget.first.value.c_str())] = saveElementWidget(std::get<0>(widget.second),
				                                                                              std::get<1>(widget.second));
			}

			return jsonObject;
		}

		void restore(const QJsonValue& jsonValue)
		{
			for (QString& key : jsonValue.toObject().keys())
			{
				const QJsonObject& jsonItem = jsonValue.toObject()[key].toObject();
				tVariableName variableName = key.toStdString();
				if (widgets.find(variableName) != widgets.end())
				{
					restoreElementWidget(std::get<0>(widgets.find(variableName)->second),
					                     std::get<1>(widgets.find(variableName)->second),
					                     jsonItem);
				}
			}
		}

	public:
		bool makeWidget(const cModule::tVariables& variables)
		{
			mainWidget = new QWidget();
			mainWidget->setAttribute(Qt::WA_NoSystemBackground, true);
			QVBoxLayout* layout = new QVBoxLayout();
			layout->setContentsMargins(0, 0, 0, 0);

			for (const auto& variable : variables)
			{
				QWidget* elementWidget = makeElementWidget(std::get<0>(variable.second),
				                                           variable.first,
				                                           std::get<1>(variable.second));
				if (elementWidget)
				{
					widgets.insert(std::make_pair(variable.first,
					                              std::make_tuple(std::get<0>(variable.second),
					                                              elementWidget)));
					layout->addWidget(elementWidget);
				}
			}

			mainWidget->setLayout(layout);
			return true;
		}

	public:
		tLibraryName libraryName;
		tModuleTypeName moduleTypeName;
		tModuleName moduleName;
		tMemoryTypeName memoryTypeName;
		tRootModuleName rootModuleName;
		tSchemeName schemeName;

		QWidget* mainWidget;
		std::map<tVariableName, std::tuple<std::type_index, QWidget*>> widgets;
	};

	class cRootModuleDataModel : public NodeDataModel
	{
	public:
		cRootModuleDataModel(const tLibraryName& libraryName,
		                     const tRootModuleName& rootModuleName,
		                     const tGuiRootSignalExits& guiRootSignalExits,
		                     const tGuiRootMemoryExits& guiRootMemoryExits) :
			libraryName(libraryName),
			rootModuleName(rootModuleName),
			guiRootSignalExits(guiRootSignalExits),
			guiRootMemoryExits(guiRootMemoryExits)
		{
			modelData.moduleTypeName = "root";
			modelData.libraryName = libraryName;
			modelData.rootModuleName = rootModuleName;

			NodeStyle style = nodeStyle();
			style.GradientColor0 = QColor(0xff8080);
			style.GradientColor1 = QColor(0xc30101);
			style.GradientColor2 = QColor(0xc30101);
			style.GradientColor3 = QColor(0xc30101);
			setNodeStyle(style);
		}

		~cRootModuleDataModel()
		{
		}

		QJsonObject save() const override
		{
			QJsonObject jsonObject = NodeDataModel::save();
			jsonObject["variables"] = modelData.save();
			jsonObject["moduleTypeName"] = QString::fromUtf8(modelData.moduleTypeName.value.c_str());
			jsonObject["libraryName"] = QString::fromUtf8(modelData.libraryName.value.c_str());
			jsonObject["rootModuleName"] = QString::fromUtf8(modelData.rootModuleName.value.c_str());
			return jsonObject;
		}

	public:
		QString caption() const override
		{
			return QString::fromUtf8((libraryName.value + ":" + rootModuleName.value).c_str());
		}

		QString name() const override
		{
			return QString::fromUtf8((libraryName.value + ":root:" + rootModuleName.value).c_str());
		}

		std::unique_ptr<NodeDataModel> clone() const override
		{
			return std::make_unique<cRootModuleDataModel>(libraryName,
			                                              rootModuleName,
			                                              guiRootSignalExits,
			                                              guiRootMemoryExits);
		}

		const void* getData() const override
		{
			return &modelData;
		}

		size_t uniqueCount() const override
		{
			return 1;
		}

	public:
		unsigned int nPorts(PortType portType) const override
		{
			if (portType == PortType::In)
			{
				return 0;
			}
			else if (portType == PortType::Out)
			{
				return guiRootSignalExits.size() + guiRootMemoryExits.size();
			}

			return 0;
		}

		NodeDataType dataType(PortType portType, PortIndex portIndex) const override
		{
			if (portType == PortType::In)
			{
			}
			else if (portType == PortType::Out)
			{
				if (portIndex < guiRootSignalExits.size())
				{
					auto& iter = guiRootSignalExits[portIndex];
					return NodeDataType {"signal",
					                     QString::fromUtf8(iter.value.c_str())};
				}
				else if (portIndex < guiRootSignalExits.size() + guiRootMemoryExits.size())
				{
					auto& iter = guiRootMemoryExits[portIndex - guiRootSignalExits.size()];
					return NodeDataType {QString::fromUtf8((std::get<1>(iter)).value.c_str()),
					                     QString::fromUtf8((std::get<0>(iter)).value.c_str())};
				}
			}

			return NodeDataType();
		}

		ConnectionPolicy portOutConnectionPolicy(PortIndex) const override
		{
			return ConnectionPolicy::One;
		}

		std::shared_ptr<NodeData> outData(PortIndex) override
		{
			return nullptr;
		}

		void setInData(std::shared_ptr<NodeData>, int) override
		{
		}

		QWidget* embeddedWidget() override
		{
			return nullptr;
		}

	private:
		cModelData modelData;
		const tLibraryName libraryName;
		const tRootModuleName rootModuleName;
		const tGuiRootSignalExits guiRootSignalExits;
		const tGuiRootMemoryExits guiRootMemoryExits;
	};

	class cModuleDataModel : public NodeDataModel
	{
	public:
		cModuleDataModel(const tLibraryName& libraryName,
		                 const tModuleName& moduleName,
		                 const cModule* module) :
			libraryName(libraryName),
			moduleName(moduleName),
			module(module)
		{
			modelData.moduleTypeName = module->getModuleTypeName();
			modelData.libraryName = libraryName;
			modelData.moduleName = moduleName;

			modelData.makeWidget(module->getVariables());

			if (module->getModuleTypeName().value == "logic" ||
			    module->getModuleTypeName().value == "convert")
			{
				NodeStyle style = nodeStyle();
				style.GradientColor0 = QColor(0xbce085);
				style.GradientColor1 = QColor(0x234d20);
				style.GradientColor2 = QColor(0x234d20);
				style.GradientColor3 = QColor(0x234d20);
				setNodeStyle(style);
			}
		}

		~cModuleDataModel()
		{
		}

		QJsonObject save() const override
		{
			QJsonObject jsonObject = NodeDataModel::save();
			jsonObject["variables"] = modelData.save();
			jsonObject["moduleTypeName"] = QString::fromUtf8(modelData.moduleTypeName.value.c_str());
			jsonObject["libraryName"] = QString::fromUtf8(modelData.libraryName.value.c_str());
			jsonObject["moduleName"] = QString::fromUtf8(modelData.moduleName.value.c_str());
			return jsonObject;
		}

		void restore(const QJsonObject& jsonObject) override
		{
			modelData.restore(jsonObject["variables"]);
		}

	public:
		QString caption() const override
		{
			if (!module->getCaptionName().value.length())
			{
				return QString::fromUtf8((libraryName.value + ":" + moduleName.value).c_str());
			}

			return QString::fromUtf8((module->getCaptionName().value).c_str());
		}

		QString name() const override
		{
			return QString::fromUtf8((libraryName.value + ":" + moduleName.value).c_str());
		}

		std::unique_ptr<NodeDataModel> clone() const override
		{
			return std::make_unique<cModuleDataModel>(libraryName,
			                                          moduleName,
			                                          module);
		}

		const void* getData() const override
		{
			return &modelData;
		}

		const bool canConnect(PortType portType,
		                      NodeDataModel* model,
		                      NodeDataType nodeDataType) const override
		{
			cModelData* data = (cModelData*)model->getData();

			if (data->moduleTypeName.value == "scheme")
			{
				return true;
			}
			else if (data->moduleTypeName.value == "custom")
			{
				/** @todo */
				return true;
			}

			if (nodeDataType.id != "signal" &&
			    data->moduleTypeName.value != "memory")
			{
				return false;
			}

			return true;
		}

	public:
		unsigned int nPorts(PortType portType) const override
		{
			if (portType == PortType::In)
			{
				return module->getSignalEntries().size() + module->getMemoryEntries().size();
			}
			else if (portType == PortType::Out)
			{
				return module->getSignalExits().size() + module->getMemoryExits().size();
			}

			return 0;
		}

		NodeDataType dataType(PortType portType, PortIndex portIndex) const override
		{
			if (portType == PortType::In)
			{
				if (portIndex < module->getSignalEntries().size())
				{
					auto iter = module->getSignalEntries().begin();
					std::advance(iter, portIndex);
					return NodeDataType {"signal",
					                     QString::fromUtf8(iter->first.value.c_str())};
				}
				else if (portIndex < module->getSignalEntries().size() + module->getMemoryEntries().size())
				{
					auto iter = module->getMemoryEntries().begin();
					std::advance(iter, portIndex - module->getSignalEntries().size());
					return NodeDataType {QString::fromUtf8((std::get<0>(iter->second)).value.c_str()),
					                     QString::fromUtf8((iter->first).value.c_str())};
				}
			}
			else if (portType == PortType::Out)
			{
				if (portIndex < module->getSignalExits().size())
				{
					auto iter = module->getSignalExits().begin();
					std::advance(iter, portIndex);
					return NodeDataType {"signal",
					                     QString::fromUtf8(iter->first.value.c_str())};
				}
				else if (portIndex < module->getSignalExits().size() + module->getMemoryExits().size())
				{
					auto iter = module->getMemoryExits().begin();
					std::advance(iter, portIndex - module->getSignalExits().size());
					return NodeDataType {QString::fromUtf8((std::get<0>(iter->second)).value.c_str()),
					                     QString::fromUtf8((iter->first).value.c_str())};
				}
			}

			return NodeDataType();
		}

		ConnectionPolicy portInConnectionPolicy(PortIndex portIndex) const override
		{
			if (portIndex < module->getSignalEntries().size())
			{
				return ConnectionPolicy::Many;
			}
			return ConnectionPolicy::One;
		}

		ConnectionPolicy portOutConnectionPolicy(PortIndex) const override
		{
			return ConnectionPolicy::One;
		}

		std::shared_ptr<NodeData> outData(PortIndex) override
		{
			return nullptr;
		}

		void setInData(std::shared_ptr<NodeData>, int) override
		{
		}

		QWidget* embeddedWidget() override
		{
			return modelData.mainWidget;
		}

	private:
		cModelData modelData;
		const tLibraryName libraryName;
		const tModuleName moduleName;
		const cModule* module;
	};

	class cMemoryDataModel : public NodeDataModel
	{
	public:
		cMemoryDataModel(const tMemoryTypeName& memoryTypeName,
		                 const cMemory* memory) :
			memoryTypeName(memoryTypeName),
			memory(memory)
		{
			modelData.moduleTypeName = "memory";
			modelData.memoryTypeName = memoryTypeName;

			modelData.makeWidget(memory->getVariables());

			NodeStyle style = nodeStyle();
			style.GradientColor0 = QColor(0xd2a56d);
			style.GradientColor1 = QColor(0x83502e);
			style.GradientColor2 = QColor(0x83502e);
			style.GradientColor3 = QColor(0x83502e);
			setNodeStyle(style);
		}

		~cMemoryDataModel()
		{
		}

		QJsonObject save() const override
		{
			QJsonObject jsonObject = NodeDataModel::save();
			jsonObject["variables"] = modelData.save();
			jsonObject["moduleTypeName"] = QString::fromUtf8(modelData.moduleTypeName.value.c_str());
			jsonObject["memoryTypeName"] = QString::fromUtf8(modelData.memoryTypeName.value.c_str());
			return jsonObject;
		}

		void restore(const QJsonObject& jsonObject) override
		{
			modelData.restore(jsonObject["variables"]);
		}

	public:
		QString caption() const override
		{
			return QString::fromUtf8(("memory:" + memoryTypeName.value).c_str());
		}

		QString name() const override
		{
			return QString::fromUtf8((":memory:" + memoryTypeName.value).c_str());
		}

		std::unique_ptr<NodeDataModel> clone() const override
		{
			return std::make_unique<cMemoryDataModel>(memoryTypeName,
			                                          memory);
		}

		const void* getData() const override
		{
			return &modelData;
		}

		const bool canConnect(PortType portType,
		                      NodeDataModel* model,
		                      NodeDataType nodeDataType) const override
		{
			cModelData* data = (cModelData*)model->getData();
			if (data->moduleTypeName == modelData.moduleTypeName)
			{
				return false;
			}
			return true;
		}

	public:
		unsigned int nPorts(PortType portType) const override
		{
			if (portType == PortType::In)
			{
				return 1;
			}
			else if (portType == PortType::Out)
			{
				return 1;
			}

			return 0;
		}

		NodeDataType dataType(PortType portType, PortIndex portIndex) const override
		{
			if (portType == PortType::In)
			{
				if (portIndex == 0)
				{
					return NodeDataType {QString::fromUtf8((memoryTypeName.value).c_str()),
					                     ""};
				}
			}
			else if (portType == PortType::Out)
			{
				if (portIndex == 0)
				{
					return NodeDataType {QString::fromUtf8((memoryTypeName.value).c_str()),
					                     ""};
				}
			}

			return NodeDataType();
		}

		ConnectionPolicy portInConnectionPolicy(PortIndex) const override
		{
			return ConnectionPolicy::Many;
		}

		ConnectionPolicy portOutConnectionPolicy(PortIndex) const override
		{
			return ConnectionPolicy::Many;
		}

		bool portCaptionVisible(PortType, PortIndex) const override
		{
			return true;
		}

		std::shared_ptr<NodeData> outData(PortIndex) override
		{
			return nullptr;
		}

		void setInData(std::shared_ptr<NodeData>, int) override
		{
		}

		QWidget* embeddedWidget() override
		{
			return modelData.mainWidget;
		}

	private:
		cModelData modelData;
		const tMemoryTypeName memoryTypeName;
		const cMemory* memory;
	};

	class cSchemeSignalModuleDataModel : public NodeDataModel
	{
	public:
		cSchemeSignalModuleDataModel(const QString& moduleName,
		                             const PortType& direction) :
		        moduleName(moduleName),
		        direction(direction)
		{
			modelData.moduleTypeName = "scheme";

			NodeStyle style = nodeStyle();
			style.GradientColor0 = QColor(0xefbbff);
			style.GradientColor1 = QColor(0x800080);
			style.GradientColor2 = QColor(0x800080);
			style.GradientColor3 = QColor(0x800080);
			setNodeStyle(style);

			lineEdit = new QLineEdit();
			lineEdit->setPlaceholderText("signalName");
		}

		~cSchemeSignalModuleDataModel()
		{
		}

		QJsonObject save() const override
		{
			QJsonObject jsonObject = NodeDataModel::save();
			jsonObject["moduleTypeName"] = QString::fromUtf8(modelData.moduleTypeName.value.c_str());
			jsonObject["portName"] = lineEdit->text();
			jsonObject["direction"] = (direction == PortType::Out ? "inSignal" : "outSignal");
			return jsonObject;
		}

		void restore(const QJsonObject& jsonObject) override
		{
			lineEdit->setText(jsonObject["portName"].toString());
		}

	public:
		QString caption() const override
		{
			if (direction == PortType::Out)
			{
				return "inSignal";
			}
			return "outSignal";
		}

		QString name() const override
		{
			return moduleName;
		}

		std::unique_ptr<NodeDataModel> clone() const override
		{
			return std::make_unique<cSchemeSignalModuleDataModel>(moduleName,
			                                                      direction);
		}

		const void* getData() const override
		{
			return &modelData;
		}

		const bool canConnect(PortType portType,
		                      NodeDataModel* model,
		                      NodeDataType nodeDataType) const override
		{
			return true;
		}

	public:
		unsigned int nPorts(PortType portType) const override
		{
			if (portType == direction)
			{
				return 1;
			}

			return 0;
		}

		NodeDataType dataType(PortType portType, PortIndex portIndex) const override
		{
			if (portType == direction)
			{
				if (portIndex == 0)
				{
					if (lineEdit->text().isEmpty())
					{
						return NodeDataType {"signal", "signal"};
					}

					return NodeDataType {"signal",
					                     lineEdit->text()};
				}
			}

			return NodeDataType();
		}

		ConnectionPolicy portInConnectionPolicy(PortIndex portIndex) const override
		{
			return ConnectionPolicy::Many;
		}

		ConnectionPolicy portOutConnectionPolicy(PortIndex) const override
		{
			return ConnectionPolicy::One;
		}

		bool portCaptionVisible(PortType, PortIndex) const override
		{
			return true;
		}

		std::shared_ptr<NodeData> outData(PortIndex) override
		{
			return nullptr;
		}

		void setInData(std::shared_ptr<NodeData>, int) override
		{
		}

		QWidget* embeddedWidget() override
		{
			return lineEdit;
		}

	private:
		cModelData modelData;
		QLineEdit* lineEdit;
		const QString moduleName;
		const PortType direction;
	};

	class cSchemeMemoryModuleDataModel : public NodeDataModel
	{
	public:
		cSchemeMemoryModuleDataModel(const QString& moduleName,
		                             const PortType& direction) :
		        moduleName(moduleName),
		        direction(direction)
		{
			modelData.moduleTypeName = "scheme";

			NodeStyle style = nodeStyle();
			style.GradientColor0 = QColor(0xefbbff);
			style.GradientColor1 = QColor(0x800080);
			style.GradientColor2 = QColor(0x800080);
			style.GradientColor3 = QColor(0x800080);
			setNodeStyle(style);

			lineEditName = new QLineEdit();
			lineEditName->setPlaceholderText("memoryName");

			lineEditType = new QLineEdit();
			lineEditType->setPlaceholderText("memoryType");

			mainWidget = new QWidget();
			mainWidget->setAttribute(Qt::WA_NoSystemBackground, true);

			QVBoxLayout* layout = new QVBoxLayout(mainWidget);
			layout->setContentsMargins(0, 0, 0, 0);
			layout->addWidget(lineEditName);
			layout->addWidget(lineEditType);
		}

		~cSchemeMemoryModuleDataModel()
		{
		}

		QJsonObject save() const override
		{
			QJsonObject jsonObject = NodeDataModel::save();
			jsonObject["moduleTypeName"] = QString::fromUtf8(modelData.moduleTypeName.value.c_str());
			jsonObject["portName"] = lineEditName->text();
			jsonObject["portType"] = lineEditType->text();
			jsonObject["direction"] = (direction == PortType::Out ? "inMemory" : "outMemory");
			return jsonObject;
		}

		void restore(const QJsonObject& jsonObject) override
		{
			lineEditName->setText(jsonObject["portName"].toString());
			lineEditType->setText(jsonObject["portType"].toString());
		}

	public:
		QString caption() const override
		{
			if (direction == PortType::Out)
			{
				return "inMemory";
			}
			return "outMemory";
		}

		QString name() const override
		{
			return moduleName;
		}

		std::unique_ptr<NodeDataModel> clone() const override
		{
			return std::make_unique<cSchemeMemoryModuleDataModel>(moduleName,
			                                                      direction);
		}

		const void* getData() const override
		{
			return &modelData;
		}

		const bool canConnect(PortType portType,
		                      NodeDataModel* model,
		                      NodeDataType nodeDataType) const override
		{
			/** @todo */
			return true;
		}

	public:
		unsigned int nPorts(PortType portType) const override
		{
			if (portType == direction)
			{
				return 1;
			}

			return 0;
		}

		NodeDataType dataType(PortType portType, PortIndex portIndex) const override
		{
			if (portType == direction)
			{
				if (portIndex == 0)
				{
					if (lineEditName->text().isEmpty())
					{
						return NodeDataType {lineEditType->text(), lineEditType->text()};
					}

					return NodeDataType {lineEditType->text(),
					                     lineEditName->text()};
				}
			}

			return NodeDataType();
		}

		ConnectionPolicy portInConnectionPolicy(PortIndex portIndex) const override
		{
			return ConnectionPolicy::Many;
		}

		ConnectionPolicy portOutConnectionPolicy(PortIndex) const override
		{
			return ConnectionPolicy::Many;
		}

		bool portCaptionVisible(PortType, PortIndex) const override
		{
			return true;
		}

		std::shared_ptr<NodeData> outData(PortIndex) override
		{
			return nullptr;
		}

		void setInData(std::shared_ptr<NodeData>, int) override
		{
		}

		QWidget* embeddedWidget() override
		{
			return mainWidget;
		}

	private:
		cModelData modelData;
		QWidget* mainWidget;
		QLineEdit* lineEditName;
		QLineEdit* lineEditType;
		const QString moduleName;
		const PortType direction;
	};

	class cCustomModuleDataModel : public NodeDataModel
	{
	public:
		cCustomModuleDataModel(const tModuleName& moduleName,
		                       const tSchemeName& schemeName) :
		        moduleName(moduleName),
		        schemeName(schemeName)
		{
			modelData.moduleTypeName = "custom";
			modelData.moduleName = moduleName;
			modelData.schemeName = schemeName;

			NodeStyle style = nodeStyle();
			style.GradientColor0 = QColor(0xefbbff);
			style.GradientColor1 = QColor(0x800080);
			style.GradientColor2 = QColor(0x800080);
			style.GradientColor3 = QColor(0x800080);
			setNodeStyle(style);

			update();
		}

		~cCustomModuleDataModel()
		{
		}

		QJsonObject save() const override
		{
			QJsonObject jsonObject = NodeDataModel::save();
			jsonObject["variables"] = modelData.save();
			jsonObject["moduleTypeName"] = QString::fromUtf8(modelData.moduleTypeName.value.c_str());
			jsonObject["moduleName"] = QString::fromUtf8(modelData.moduleName.value.c_str());
			jsonObject["schemeName"] = QString::fromUtf8(modelData.schemeName.value.c_str());
			return jsonObject;
		}

		void restore(const QJsonObject& jsonObject) override
		{
			modelData.restore(jsonObject["variables"]);
		}

	public:
		QString caption() const override
		{
			return QString::fromUtf8((schemeName.value).c_str());
		}

		QString name() const override
		{
			return QString::fromUtf8((moduleName.value).c_str());
		}

		std::unique_ptr<NodeDataModel> clone() const override
		{
			return std::make_unique<cCustomModuleDataModel>(moduleName,
			                                                schemeName);
		}

		const void* getData() const override
		{
			return &modelData;
		}

		const bool canConnect(PortType portType,
		                      NodeDataModel* model,
		                      NodeDataType nodeDataType) const override
		{
			/** @todo */
			return true;
		}

	public:
		unsigned int nPorts(PortType portType) const override
		{
			if (portType == PortType::In)
			{
				return signalEntries.size() + memoryEntries.size();
			}
			else if (portType == PortType::Out)
			{
				return signalExits.size() + memoryExits.size();
			}

			return 0;
		}

		NodeDataType dataType(PortType portType, PortIndex portIndex) const override
		{
			if (portType == PortType::In)
			{
				if (portIndex < signalEntries.size())
				{
					auto iter = signalEntries.begin();
					std::advance(iter, portIndex);
					return NodeDataType {"signal",
					                     QString::fromUtf8(iter->first.value.c_str())};
				}
				else if (portIndex < signalEntries.size() + memoryEntries.size())
				{
					auto iter = memoryEntries.begin();
					std::advance(iter, portIndex - signalEntries.size());
					return NodeDataType {QString::fromUtf8((std::get<0>(iter->second)).value.c_str()),
					                     QString::fromUtf8((iter->first).value.c_str())};
				}
			}
			else if (portType == PortType::Out)
			{
				if (portIndex < signalExits.size())
				{
					auto iter = signalExits.begin();
					std::advance(iter, portIndex);
					return NodeDataType {"signal",
					                     QString::fromUtf8(iter->first.value.c_str())};
				}
				else if (portIndex < signalExits.size() + memoryExits.size())
				{
					auto iter = memoryExits.begin();
					std::advance(iter, portIndex - signalExits.size());
					return NodeDataType {QString::fromUtf8((std::get<0>(iter->second)).value.c_str()),
					                     QString::fromUtf8((iter->first).value.c_str())};
				}
			}

			return NodeDataType();
		}

		ConnectionPolicy portInConnectionPolicy(PortIndex portIndex) const override
		{
			return ConnectionPolicy::Many;
		}

		ConnectionPolicy portOutConnectionPolicy(PortIndex portIndex) const override
		{
			if (portIndex < signalExits.size())
			{
				return ConnectionPolicy::One;
			}
			return ConnectionPolicy::Many;
		}

		std::shared_ptr<NodeData> outData(PortIndex) override
		{
			return nullptr;
		}

		void setInData(std::shared_ptr<NodeData>, int) override
		{
		}

		QWidget* embeddedWidget() override
		{
			return modelData.mainWidget;
		}

	private:
		void update()
		{
			std::string filePath = getFilePathOfScheme(std::vector<std::string>(), schemeName);
			if (!QFileInfo::exists(QString::fromUtf8(filePath.c_str())))
			{
				printf("error: QFileInfo::exists()\n");
				return;
			}

			QFile file(QString::fromUtf8(filePath.c_str()));
			if (!file.open(QIODevice::ReadOnly))
			{
				printf("error: file.open()\n");
				return;
			}

			QByteArray wholeFile = file.readAll();

			QJsonObject const jsonDocument = QJsonDocument::fromJson(wholeFile).object();

			QJsonArray connectionJsonArray = jsonDocument["nodes"].toArray();
			for (int i = 0; i < connectionJsonArray.size(); ++i)
			{
				QJsonObject nodeJson = connectionJsonArray[i].toObject();
				if (nodeJson.find("model") != nodeJson.end())
				{
					QJsonObject modelJson = nodeJson["model"].toObject();
					if (modelJson.find("name") != modelJson.end())
					{
						if (modelJson["name"].toString() == ":scheme:inSignal")
						{
							QString portName = modelJson["portName"].toString();
							if (portName.isEmpty())
							{
								portName = "signal";
							}

							const auto value = std::make_tuple(signalEntries.size(),
							                                   nullptr);
							signalEntries[portName.toStdString()] = value;
						}
						else if (modelJson["name"].toString() == ":scheme:outSignal")
						{
							QString portName = modelJson["portName"].toString();
							if (portName.isEmpty())
							{
								portName = "signal";
							}

							const auto value = signalExits.size();
							signalExits[portName.toStdString()] = value;
						}
						else if (modelJson["name"].toString() == ":scheme:inMemory")
						{
							QString portName = modelJson["portName"].toString();
							QString portType = modelJson["portType"].toString();

							if (!portType.isEmpty())
							{
								if (portName.isEmpty())
								{
									portName = portType;
								}

								const auto value = std::make_tuple(portType.toStdString(),
								                                   0);
								memoryEntries[portName.toStdString()] = value;
							}
						}
						else if (modelJson["name"].toString() == ":scheme:outMemory")
						{
							QString portName = modelJson["portName"].toString();
							QString portType = modelJson["portType"].toString();

							if (!portType.isEmpty())
							{
								if (portName.isEmpty())
								{
									portName = portType;
								}

								const auto value = std::make_tuple(portType.toStdString(),
								                                   0);
								memoryExits[portName.toStdString()] = value;
							}
						}
					}
				}
			}
		}

	private:
		cModelData modelData;
		const tModuleName moduleName;
		const tSchemeName schemeName;
		cModule::tSignalEntries signalEntries;
		cModule::tMemoryEntries memoryEntries;
		cModule::tSignalExits signalExits;
		cModule::tMemoryExits memoryExits;
	};
};

class cIde : public QObject
{
public:
	cIde(const cVirtualMachine* virtualMachine,
	     const QString& titleName) :
		virtualMachine(virtualMachine),
		titleName(titleName)
	{
		mainWidget = new QWidget();

		QVBoxLayout* layout = new QVBoxLayout(mainWidget);

		{
			auto menuBar = new QMenuBar();

			auto fileMenu = menuBar->addMenu("&File");
			auto newAction = fileMenu->addAction("&New");
			fileMenu->addSeparator();
			auto openAction = fileMenu->addAction("&Open");
			fileMenu->addSeparator();
			auto saveAction = fileMenu->addAction("&Save");
			auto saveAsAction = fileMenu->addAction("Save &As");

			auto modulesMenu = menuBar->addMenu("&Modules");
			auto newCustomModuleAction = modulesMenu->addAction("&New custom module");

			QObject::connect(newAction, &QAction::triggered, this, &cIde::menuNew);
			QObject::connect(openAction, &QAction::triggered, this, &cIde::menuOpen);
			QObject::connect(saveAction, &QAction::triggered, this, &cIde::menuSave);
			QObject::connect(saveAsAction, &QAction::triggered, this, &cIde::menuSaveAs);
			QObject::connect(newCustomModuleAction, &QAction::triggered, this, &cIde::menuNewCustomModule);

			openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
			saveAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

			layout->addWidget(menuBar);
		}

		tabWidget = new QTabWidget();

		createSchemeEditor("main");
		tabWidget->setTabText(getMainIndex(), "untitled");

		layout->addWidget(tabWidget);

		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);

		ConnectionStyle::setConnectionStyle(
		R"(
		  {
		    "ConnectionStyle": {
		      "UseDataDefinedColors": true
		    }
		  }
		  )");

		mainWidget->setWindowTitle(this->titleName);
		mainWidget->showMaximized();

		currentFilePath = "";
	}

private:
	void updateCustoms(const QString& schemeName)
	{
		QTreeWidget* treeView = std::get<2>(schemes[schemeName]);
		QMap<tLibraryName, QTreeWidgetItem*>& libraryLevelItems = std::get<3>(schemes[schemeName]);

		customsLibraries.push_back(QDir::homePath().toStdString() + "/.config"); /**< @todo */

		QTreeWidgetItem* libraryItem = libraryLevelItems["custom"];

		for (const std::string& libraryPath : customsLibraries)
		{
			QDir dir(QString::fromUtf8(libraryPath.c_str()));
			QStringList filePaths = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
			for (const QString& filePath : filePaths)
			{
				if (!filePath.endsWith(".json"))
				{
					continue;
				}

				QFileInfo fileInfo(filePath);

				FlowScene* scene = std::get<4>(schemes[schemeName]);
				DataModelRegistry& dataModelRegistry = scene->registry();
				dataModelRegistry.registerModel<cGui::cCustomModuleDataModel>(std::make_unique<cGui::cCustomModuleDataModel>(":custom:" + fileInfo.completeBaseName().toStdString(),
				                                                                                                             fileInfo.completeBaseName().toStdString()));

				QTreeWidgetItem* customModuleItem = new QTreeWidgetItem(libraryItem);
				customModuleItem->setText(0, fileInfo.completeBaseName());
				customModuleItem->setData(0, Qt::UserRole, ":custom:" + fileInfo.completeBaseName());
			}
		}
	}

	QWidget* createSchemeEditor(const QString& schemeName)
	{
		QSplitter* splitter = new QSplitter();
		std::get<1>(schemes[schemeName]) = splitter;

		QTreeWidget* treeView = new QTreeWidget();
		std::get<2>(schemes[schemeName]) = treeView;

		QMap<tLibraryName, QTreeWidgetItem*>& libraryLevelItems = std::get<3>(schemes[schemeName]);

		FlowScene* scene = new FlowScene(cGui::makeDataModelRegistry(virtualMachine));
		std::get<4>(schemes[schemeName]) = scene;

		FlowView* flowView = new FlowView(scene);
		std::get<5>(schemes[schemeName]) = flowView;

		QLineEdit* filterEdit = new QLineEdit();
		std::get<6>(schemes[schemeName]) = filterEdit;

		splitter->setOrientation(Qt::Horizontal);

		flowView->setSceneRect(-20000, -20000, 40000, 40000);

		{
			QVBoxLayout* layout = new QVBoxLayout();

			treeView->header()->close();

			{
				const auto memoryTypes = virtualMachine->getGuiMemoryTypes();
				if (memoryTypes.size())
				{
					QTreeWidgetItem* libraryItem = new QTreeWidgetItem(treeView);
					libraryItem->setText(0, "memory");
					libraryItem->setData(0, Qt::UserRole, "");
					libraryLevelItems["memory"] = libraryItem;

					for (const auto& memoryType : memoryTypes)
					{
						QString memoryTypeName = QString::fromUtf8(memoryType.first.value.c_str());

						QTreeWidgetItem* memoryItem = new QTreeWidgetItem(libraryItem);

						memoryItem->setText(0, memoryTypeName);
						memoryItem->setData(0, Qt::UserRole, "");

						{
							QTreeWidgetItem* valueItem = new QTreeWidgetItem(memoryItem);

							valueItem->setText(0, "value");
							valueItem->setData(0, Qt::UserRole, ":memory:" + memoryTypeName);
						}

						QMap<tModuleTypeName, QTreeWidgetItem*> moduleTypeLevelItems;

						const auto memoryModules = virtualMachine->getGuiMemoryModules();
						for (const auto& memoryModule : memoryModules)
						{
							tMemoryTypeName iterMemoryTypeName = std::get<0>(memoryModule.first);
							tModuleTypeName moduleTypeName = memoryModule.second->getModuleTypeName();

							if (QString::fromUtf8(iterMemoryTypeName.value.c_str()) == memoryTypeName)
							{
								if (moduleTypeLevelItems.find(moduleTypeName) == moduleTypeLevelItems.end())
								{
									QTreeWidgetItem* moduleTypeItem = new QTreeWidgetItem(memoryItem);

									moduleTypeItem->setText(0, QString::fromUtf8(moduleTypeName.value.c_str()));
									moduleTypeItem->setData(0, Qt::UserRole, "");

									moduleTypeLevelItems[moduleTypeName] = moduleTypeItem;
								}

								QTreeWidgetItem* moduleItem = new QTreeWidgetItem(moduleTypeLevelItems[moduleTypeName]);

								QString moduleName = QString::fromUtf8(std::get<1>(memoryModule.first).value.c_str());

								moduleItem->setText(0, moduleName);
								moduleItem->setData(0, Qt::UserRole, ":memory:" + memoryTypeName + ":" + moduleName);
							}
						}
					}

					treeView->expandItem(libraryItem);
				}
			}

			{
				QTreeWidgetItem* customItem = new QTreeWidgetItem(treeView);
				customItem->setText(0, "custom");
				customItem->setData(0, Qt::UserRole, "");
				libraryLevelItems["custom"] = customItem;
				updateCustoms(schemeName);
				treeView->expandItem(customItem);
			}

			const auto rootModules = virtualMachine->getGuiRootModules();
			QMap<tLibraryName, QTreeWidgetItem*> rootLevelItems;
			for (const auto& rootModule : rootModules)
			{
				tLibraryName libraryName = std::get<0>(rootModule.first);
				tRootModuleName rootModuleName = std::get<1>(rootModule.first);

				if (libraryLevelItems.find(libraryName) == libraryLevelItems.end())
				{
					QTreeWidgetItem* item = new QTreeWidgetItem(treeView);

					item->setText(0, QString::fromUtf8(libraryName.value.c_str()));
					item->setData(0, Qt::UserRole, "");

					libraryLevelItems[libraryName] = item;

					treeView->expandItem(item);
				}

				if (rootLevelItems.find(libraryName) == rootLevelItems.end())
				{
					QTreeWidgetItem* item = new QTreeWidgetItem(libraryLevelItems[libraryName]);

					item->setText(0, "root");
					item->setData(0, Qt::UserRole, "");

					rootLevelItems[libraryName] = item;
				}

				QTreeWidgetItem* item = new QTreeWidgetItem(rootLevelItems[libraryName]);

				item->setText(0, QString::fromUtf8(rootModuleName.value.c_str()));
				item->setData(0, Qt::UserRole, QString::fromUtf8((libraryName.value + ":root:" + rootModuleName.value).c_str()));
			}

			QMap<std::tuple<tLibraryName,
			                tModuleTypeName>,
			     QTreeWidgetItem*> moduleTypeLevelItems;

			const auto modules = virtualMachine->getGuiModules();
			for (const auto& module : modules)
			{
				tLibraryName libraryName = std::get<0>(module.first);
				tModuleName moduleName = std::get<1>(module.first);
				tModuleTypeName moduleTypeName = module.second->getModuleTypeName();

				if (libraryLevelItems.find(libraryName) == libraryLevelItems.end())
				{
					QTreeWidgetItem* item = new QTreeWidgetItem(treeView);

					item->setText(0, QString::fromUtf8(libraryName.value.c_str()));
					item->setData(0, Qt::UserRole, "");

					libraryLevelItems[libraryName] = item;

					treeView->expandItem(item);
				}

				const auto key = std::make_tuple(libraryName,
				                                 moduleTypeName);
				if (moduleTypeLevelItems.find(key) == moduleTypeLevelItems.end())
				{
					QTreeWidgetItem* moduleTypeItem = new QTreeWidgetItem(libraryLevelItems[libraryName]);

					moduleTypeItem->setText(0, QString::fromUtf8(moduleTypeName.value.c_str()));
					moduleTypeItem->setData(0, Qt::UserRole, "");

					moduleTypeLevelItems[key] = moduleTypeItem;
				}

				QTreeWidgetItem* item = new QTreeWidgetItem(moduleTypeLevelItems[key]);

				item->setText(0, QString::fromUtf8(moduleName.value.c_str()));
				item->setData(0, Qt::UserRole, QString::fromUtf8((libraryName.value + ":" + moduleName.value).c_str()));
			}

			filterEdit->setPlaceholderText(QStringLiteral("Filter"));
			filterEdit->setClearButtonEnabled(true);

			layout->addWidget(filterEdit);
			layout->addWidget(treeView);

			layout->setContentsMargins(0, 0, 0, 0);
			layout->setSpacing(0);

			QWidget* widget = new QWidget();
			widget->setLayout(layout);

			splitter->addWidget(widget);
		}

		connect(treeView, &QTreeWidget::itemDoubleClicked, [scene, flowView](QTreeWidgetItem* item, int column)
		{
			QString modelName = item->data(0, Qt::UserRole).toString();

			if (modelName == "")
			{
				return;
			}

			auto type = scene->registry().create(modelName);
			if (type)
			{
				auto* node = scene->createNode(std::move(type));
				if (node)
				{
					QPoint pos = flowView->rect().center();
					QPointF posView = flowView->mapToScene(pos);
					node->nodeGraphicsObject().setPos(posView);
				}
			}
			else
			{
				std::cerr << "Model not found" << std::endl;
			}
		});

		connect(filterEdit, &QLineEdit::textChanged, [treeView, &libraryLevelItems](const QString& text)
		{
			std::function<bool(const QString&, QTreeWidgetItem*)> checkItems;
			checkItems = [&](const QString& text, QTreeWidgetItem* item) -> bool
			{
				if (item->childCount())
				{
					bool rc = false;

					for (int child_i = 0; child_i < item->childCount(); child_i++)
					{
						rc |= checkItems(text, item->child(child_i));
					}

					if (rc)
					{
						treeView->expandItem(item);
						item->setHidden(false);
					}
					else
					{
						item->setHidden(true);
					}

					return rc;
				}

				auto modelName = item->data(0, Qt::UserRole).toString();
				if (modelName.contains(text, Qt::CaseInsensitive))
				{
					item->setHidden(false);
					return true;
				}

				item->setHidden(true);
				return false;
			};

			if (text.isEmpty())
			{
				for (auto& libraryLevelItem : libraryLevelItems)
				{
					checkItems(text, libraryLevelItem); /**< show all items */
				}
				for (auto& libraryLevelItem : libraryLevelItems)
				{
					for (int child_i = 0; child_i < libraryLevelItem->childCount(); child_i++)
					{
						treeView->collapseItem(libraryLevelItem->child(child_i));
					}
				}
				return;
			}

			for (auto& libraryLevelItem : libraryLevelItems)
			{
				if (checkItems(text, libraryLevelItem))
				{
					libraryLevelItem->setHidden(false);
				}
				else
				{
					libraryLevelItem->setHidden(true);
				}
			}
		});

		splitter->addWidget(flowView);

		splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
		tabWidget->addTab(splitter, schemeName);
		return splitter;
	}

	int getMainIndex()
	{
		return tabWidget->indexOf(std::get<1>(schemes["main"]));
	}

	int getSchemeIndex(const QString& schemeName)
	{
		if (schemes.find(schemeName) == schemes.end())
		{
			return -1;
		}

		return tabWidget->indexOf(std::get<1>(schemes[schemeName]));
	}

	FlowScene* getMainFlowScene()
	{
		return std::get<4>(schemes["main"]);
	}

private:
	void saveProject(const QString& filePath)
	{
		if (filePath.isEmpty())
		{
			return;
		}

		currentFilePath = filePath;

		QFileInfo fileInfo(filePath);
		mainWidget->setWindowTitle(fileInfo.completeBaseName() + " - " + titleName);
		tabWidget->setTabText(getMainIndex(), fileInfo.completeBaseName());

		saveProjectConfig(filePath);
		QString baseFilePath = fileInfo.path() + "/" + fileInfo.completeBaseName();

		std::get<0>(schemes["main"]) = baseFilePath;

		for (const auto& iter : schemes)
		{
			saveToFile(std::get<4>(iter.second),
			           std::get<0>(iter.second) + ".json");
			saveScreenshot(std::get<4>(iter.second),
			               std::get<0>(iter.second) + ".png");
		}

		exportToFile(getMainFlowScene(),
		             baseFilePath + ".tfvm");
	}

	void openProject(const QString& filePath)
	{
		if (!openProjectConfig(filePath))
		{
			return;
		}

		currentFilePath = filePath;

		QFileInfo fileInfo(filePath);
		mainWidget->setWindowTitle(fileInfo.completeBaseName() + " - " + titleName);
		tabWidget->setTabText(getMainIndex(), fileInfo.completeBaseName());

		QString baseFilePath = fileInfo.path() + "/" + fileInfo.completeBaseName();

		openFromFile(getMainFlowScene(),
		             baseFilePath + ".json");
	}

	void saveProjectConfig(const QString& filePath)
	{
		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			printf("error: file.open()\n");
			return;
		}
	}

	void saveToFile(const FlowScene* scene,
	                const QString& filePath)
	{
		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			printf("error: file.open()\n");
			return;
		}

		file.write(scene->saveToMemory());
	}

	void saveScreenshot(FlowScene* scene,
	                    const QString& filePath)
	{
		scene->clearSelection();
		scene->setSceneRect(scene->itemsBoundingRect());
		QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		scene->render(&painter);
		image.save(filePath);
	}

	void exportToFile(const FlowScene* scene,
	                  const QString& filePath)
	{
		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			printf("error: file.open()\n");
			return;
		}

		QJsonObject jsonObject = QJsonDocument::fromJson(scene->saveToMemory()).object();
		std::vector<uint8_t> buffer = cGui::exportToMemory(jsonObject);

		file.write((char*)&buffer[0], buffer.size());
		file.flush();
	}

	bool openProjectConfig(const QString& filePath)
	{
		std::ifstream fileStream(filePath.toStdString(), std::ifstream::binary);
		if (!fileStream.is_open())
		{
			return false;
		}

		return true;
	}

	void openFromFile(FlowScene* scene,
	                  const QString& filePath)
	{
		if (!QFileInfo::exists(filePath))
		{
			printf("error: QFileInfo::exists()\n");
			return;
		}

		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly))
		{
			printf("error: file.open()\n");
			return;
		}

		QByteArray wholeFile = file.readAll();

		scene->clearScene();
		scene->loadFromMemory(wholeFile);
	}

	void createCustomModule(const QString& moduleName,
	                        const QString& filePath)
	{
		if (schemes.find(moduleName) != schemes.end())
		{
			return;
		}

		createSchemeEditor(moduleName);

		std::get<0>(schemes[moduleName]) = filePath;

		FlowScene* scene = std::get<4>(schemes[moduleName]);
		DataModelRegistry& dataModelRegistry = scene->registry();
		dataModelRegistry.registerModel<cGui::cSchemeSignalModuleDataModel>(std::make_unique<cGui::cSchemeSignalModuleDataModel>(":scheme:inSignal",
		                                                                                                                         PortType::Out));
		dataModelRegistry.registerModel<cGui::cSchemeSignalModuleDataModel>(std::make_unique<cGui::cSchemeSignalModuleDataModel>(":scheme:outSignal",
		                                                                                                                         PortType::In));
		dataModelRegistry.registerModel<cGui::cSchemeMemoryModuleDataModel>(std::make_unique<cGui::cSchemeMemoryModuleDataModel>(":scheme:inMemory",
		                                                                                                                         PortType::Out));
		dataModelRegistry.registerModel<cGui::cSchemeMemoryModuleDataModel>(std::make_unique<cGui::cSchemeMemoryModuleDataModel>(":scheme:outMemory",
		                                                                                                                         PortType::In));

		{
			QTreeWidget* treeView = std::get<2>(schemes[moduleName]);
			QTreeWidgetItem* schemeItem = new QTreeWidgetItem(treeView);

			schemeItem->setText(0, "scheme");
			schemeItem->setData(0, Qt::UserRole, "");

			{
				QTreeWidgetItem* signalItem = new QTreeWidgetItem(schemeItem);

				signalItem->setText(0, "inSignal");
				signalItem->setData(0, Qt::UserRole, ":scheme:inSignal");
			}

			{
				QTreeWidgetItem* signalItem = new QTreeWidgetItem(schemeItem);

				signalItem->setText(0, "outSignal");
				signalItem->setData(0, Qt::UserRole, ":scheme:outSignal");
			}

			{
				QTreeWidgetItem* memoryItem = new QTreeWidgetItem(schemeItem);

				memoryItem->setText(0, "inMemory");
				memoryItem->setData(0, Qt::UserRole, ":scheme:inMemory");
			}

			{
				QTreeWidgetItem* memoryItem = new QTreeWidgetItem(schemeItem);

				memoryItem->setText(0, "outMemory");
				memoryItem->setData(0, Qt::UserRole, ":scheme:outMemory");
			}

			treeView->expandItem(schemeItem);
		}
	}

private slots:
	void menuNew()
	{
		currentFilePath = "";
		mainWidget->setWindowTitle(this->titleName);
		tabWidget->setTabText(getMainIndex(), "untitled");
		getMainFlowScene()->clearScene();
	}

	void menuOpen()
	{
		QString filePath = QFileDialog::getOpenFileName(nullptr,
		                                                ("Open Project"),
		                                                QDir::homePath(),
		                                                ("TFVM Project (*.tfvmproject)"));

		if (filePath.isEmpty())
		{
			return;
		}

		openProject(filePath);
	}

	void menuSave()
	{
		QString filePath = currentFilePath;

		if (filePath.isEmpty())
		{
			menuSaveAs();
		}
		else
		{
			saveProject(filePath);
		}
	}

	void menuSaveAs()
	{
		QString filePath = QFileDialog::getSaveFileName(nullptr,
		                                                ("Save Project"),
		                                                QDir::homePath(),
		                                                ("TFVM Project (*.tfvmproject)"));

		if (filePath.isEmpty())
		{
			return;
		}

		saveProject(filePath);
	}

	void menuNewCustomModule()
	{
		QString moduleName = QInputDialog::getText(nullptr,
		                                         "New custom module",
		                                         "Module name:");

		if (moduleName.isEmpty())
		{
			return;
		}

		QString filePath = QDir::homePath() + "/.config/" + moduleName; /**< @todo */
		createCustomModule(moduleName, filePath);
		tabWidget->setCurrentIndex(getSchemeIndex(moduleName));
	}

private:
	const cVirtualMachine* virtualMachine;
	QString titleName;
	QWidget* mainWidget;
	QString currentFilePath;
	QTabWidget* tabWidget;
	std::map<QString, /**< schemeName */
	         std::tuple<QString, /**< filePath */
	                    QSplitter*,
	                    QTreeWidget*, /**< treeView */
	                    QMap<tLibraryName, QTreeWidgetItem*>, /**< libraryLevelItems */
	                    FlowScene*,
	                    FlowView*,
	                    QLineEdit* /**< filterEdit */
	                    >> schemes;
	std::vector<std::string> customsLibraries;
};

}

}

#endif // TFVM_GUI_H
