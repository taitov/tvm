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
public:
	using tGuiRootSignalExits = std::vector<tSignalExitName>;

	using tGuiRootMemoryExits = std::vector<std::tuple<tMemoryExitName,
	                                                   tMemoryTypeName>>;

	using tGuiModuleIds = std::map<QUuid,
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

	static tGuiModuleIds getModuleIds(const FlowScene* flowScene)
	{
		tGuiModuleIds guiModuleIds;

		for (const auto& iter : flowScene->nodes())
		{
			const auto& node = iter.second;
			const NodeDataModel* nodeDataModel = node->nodeDataModel();

			cModelData* data = (cModelData*)nodeDataModel->getData();
			if (!data)
			{
				continue;
			}

			tModuleId moduleId = guiModuleIds.size() + 1;
			guiModuleIds[iter.first] = moduleId;
		}

		return guiModuleIds;
	}

	static std::vector<uint8_t> exportToMemory(const FlowScene* flowScene)
	{
		tGuiModuleIds moduleIds = getModuleIds(flowScene);

		cStreamOut stream;

		stream.push(fileHeaderMagic);

		uint32_t schemesCount = 1; /**< @todo */
		stream.push(schemesCount);

		{
			std::string schemeName = "main"; /**< @todo */
			stream.push(schemeName);

			cScheme::tLoadMemories memories;
			cScheme::tLoadModules modules;
			cScheme::tLoadRootSignalFlows rootSignalFlows;
			cScheme::tLoadRootMemoryExitFlows rootMemoryExitFlows;
			cScheme::tLoadSignalFlows signalFlows;
			cScheme::tLoadMemoryEntryFlows memoryEntryFlows;
			cScheme::tLoadMemoryExitFlows memoryExitFlows;
			cScheme::tLoadMemoryModuleVariables memoryModuleVariables;

			for (const auto& iter : flowScene->nodes())
			{
				const auto& node = iter.second;
				const NodeDataModel* nodeDataModel = node->nodeDataModel();

				cModelData* data = (cModelData*)nodeDataModel->getData();
				if (!data)
				{
					continue;
				}

				if (data->moduleTypeName.value == "memory")
				{
					memories[moduleIds[iter.first]] = data->memoryTypeName;
					data->exportVariables(memoryModuleVariables[moduleIds[iter.first]]);
				}
				else if (data->moduleTypeName.value == "logic" ||
				         data->moduleTypeName.value == "convert")
				{
					modules[moduleIds[iter.first]] = std::make_tuple(data->libraryName,
					                                                 data->moduleName);
				}
			}

			for (const auto& iter : flowScene->connections())
			{
				const auto& connection = iter.second;

				NodeDataModel* from = connection->getNode(PortType::Out)->nodeDataModel();
				cModelData* fromData = (cModelData*)from->getData();
				std::string fromConnectionName = from->dataType(PortType::Out, connection->getPortIndex(PortType::Out)).name.toUtf8().constData();
				NodeDataModel* to = connection->getNode(PortType::In)->nodeDataModel();
				cModelData* toData = (cModelData*)to->getData();
				std::string toConnectionName = to->dataType(PortType::In, connection->getPortIndex(PortType::In)).name.toUtf8().constData();
				std::string connectionType = to->dataType(PortType::In, connection->getPortIndex(PortType::In)).id.toUtf8().constData();

				if (!fromData || !toData)
				{
					continue;
				}

				if (connectionType == "signal")
				{
					if (fromData->moduleTypeName.value == "root")
					{
						auto key = std::make_tuple(fromData->libraryName,
						                           fromData->rootModuleName,
						                           fromConnectionName);
						auto value = std::make_tuple(moduleIds[connection->getNode(PortType::In)->id()],
						                             toConnectionName);
						rootSignalFlows[key] = value;
					}
					else
					{
						auto key = std::make_tuple(moduleIds[connection->getNode(PortType::Out)->id()],
						                           fromConnectionName);
						auto value = std::make_tuple(moduleIds[connection->getNode(PortType::In)->id()],
						                             toConnectionName);
						signalFlows[key] = value;
					}
				}
				else
				{
					if (fromData->moduleTypeName.value == "root")
					{
						auto key = std::make_tuple(fromData->libraryName,
						                           fromData->rootModuleName,
						                           fromConnectionName);
						auto value = moduleIds[connection->getNode(PortType::In)->id()];
						rootMemoryExitFlows[key] = value;
					}
					else if (fromData->moduleTypeName.value == "memory" &&
					    toData->moduleTypeName.value != "memory")
					{
						auto key = std::make_tuple(moduleIds[connection->getNode(PortType::In)->id()],
						                           toConnectionName);
						auto value = moduleIds[connection->getNode(PortType::Out)->id()];
						memoryEntryFlows[key] = value;
					}
					else if (fromData->moduleTypeName.value != "memory" &&
					         toData->moduleTypeName.value == "memory")
					{
						auto key = std::make_tuple(moduleIds[connection->getNode(PortType::Out)->id()],
						                           fromConnectionName);
						auto value = moduleIds[connection->getNode(PortType::In)->id()];
						memoryExitFlows[key] = value;
					}
				}
			}

			stream.push(memories);
			stream.push(modules);
			stream.push(rootSignalFlows);
			stream.push(rootMemoryExitFlows);
			stream.push(signalFlows);
			stream.push(memoryEntryFlows);
			stream.push(memoryExitFlows);
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

	static QJsonValue saveElementWidget(const std::type_index& type,
	                                    const QWidget* widget)
	{
		if (type == typeid(std::string))
		{
			QTextEditSizeHint* textEdit = (QTextEditSizeHint*)widget;
			return QJsonValue(textEdit->toPlainText());
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
			return QJsonValue(lineEdit->text());
		}
		else if (type == typeid(bool))
		{
			QCheckBox* checkBox = (QCheckBox*)widget;
			return QJsonValue(checkBox->isChecked());
		}

		return QJsonValue();
	}

	static void restoreElementWidget(const std::type_index& type,
	                                 const QWidget* widget,
	                                 QJsonValue jsonValue)
	{
		if (type == typeid(std::string))
		{
			QTextEditSizeHint* textEdit = (QTextEditSizeHint*)widget;
			textEdit->setPlainText(jsonValue.toString());
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
			lineEdit->setText(jsonValue.toString());
		}
		else if (type == typeid(bool))
		{
			QCheckBox* checkBox = (QCheckBox*)widget;
			checkBox->setChecked(jsonValue.toBool());
		}
	}

	static void exportElementWidget(std::vector<uint8_t>& buffer,
	                                const std::type_index& type,
	                                const QWidget* widget)
	{
		cStreamOut stream;

		if (type == typeid(std::string))
		{
			QTextEditSizeHint* textEdit = (QTextEditSizeHint*)widget;
			stream.push(textEdit->toPlainText().toStdString());
		}
		else if (type == typeid(uint64_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			uint64_t value = lineEdit->text().toULongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == typeid(uint32_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			uint32_t value = lineEdit->text().toULongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == typeid(uint16_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			uint16_t value = lineEdit->text().toULongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == typeid(uint8_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			uint8_t value = lineEdit->text().toULongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == typeid(int64_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			int64_t value = lineEdit->text().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == typeid(int32_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			int32_t value = lineEdit->text().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == typeid(int16_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			int16_t value = lineEdit->text().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == typeid(int8_t))
		{
			QLineEdit* lineEdit = (QLineEdit*)widget;
			int8_t value = lineEdit->text().toLongLong(nullptr, 0);
			stream.push(value);
		}
		else if (type == typeid(bool))
		{
			QCheckBox* checkBox = (QCheckBox*)widget;
			bool value = checkBox->isChecked();
			stream.push(value);
		}

		buffer.insert(buffer.end(), stream.getBuffer().begin(), stream.getBuffer().end());
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
				const QJsonValue& jsonItem = jsonValue.toObject()[key];
				tVariableName variableName = key.toStdString();
				if (widgets.find(variableName) != widgets.end())
				{
					restoreElementWidget(std::get<0>(widgets.find(variableName)->second),
					                     std::get<1>(widgets.find(variableName)->second),
					                     jsonItem);
				}
			}
		}

		void exportVariables(std::vector<uint8_t>& buffer)
		{
			for (const auto& widget : widgets)
			{
				exportElementWidget(buffer,
				                    std::get<0>(widget.second),
				                    std::get<1>(widget.second));
			}
		}

	public:
		bool makeWidget(const cModule::tVariables& variables)
		{
			mainWidget = new QWidget();
			mainWidget->setAttribute(Qt::WA_NoSystemBackground, true);
			QVBoxLayout* layout = new QVBoxLayout();

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

		scene = new FlowScene(cGui::makeDataModelRegistry(virtualMachine));

		flowView = new FlowView(scene);
		flowView->setSceneRect(-20000, -20000, 40000, 40000);

		QVBoxLayout* layout = new QVBoxLayout(mainWidget);

		{
			auto menuBar = new QMenuBar();
			auto fileMenu = menuBar->addMenu("&File");
			auto newAction = fileMenu->addAction("&New"); /**< @todo */
			fileMenu->addSeparator();
			auto openAction = fileMenu->addAction("&Open");
			fileMenu->addSeparator();
			auto saveAction = fileMenu->addAction("&Save");
			auto saveAsAction = fileMenu->addAction("Save &As");

			QObject::connect(newAction, &QAction::triggered, this, &cIde::menuNew);
			QObject::connect(openAction, &QAction::triggered, this, &cIde::menuOpen);
			QObject::connect(saveAction, &QAction::triggered, this, &cIde::menuSave);
			QObject::connect(saveAsAction, &QAction::triggered, this, &cIde::menuSaveAs);

			openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
			saveAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

			layout->addWidget(menuBar);
		}

		{
			QSplitter* splitter = new QSplitter();
			splitter->setOrientation(Qt::Horizontal);

			{
				QVBoxLayout* layout = new QVBoxLayout();

				treeView = new QTreeWidget();
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

				connect(treeView, &QTreeWidget::itemDoubleClicked, [&](QTreeWidgetItem* item, int column)
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

				QLineEdit *filterEdit = new QLineEdit();

				filterEdit->setPlaceholderText(QStringLiteral("Filter"));
				filterEdit->setClearButtonEnabled(true);

				connect(filterEdit, &QLineEdit::textChanged, [&](const QString& text)
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

				layout->addWidget(filterEdit);
				layout->addWidget(treeView);

				layout->setContentsMargins(0, 0, 0, 0);
				layout->setSpacing(0);

				QWidget* widget = new QWidget();
				widget->setLayout(layout);

				splitter->addWidget(widget);
			}

			{
				splitter->addWidget(flowView);
			}

			splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

			layout->addWidget(splitter);
		}

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
	void saveProject(const QString& filePath)
	{
		if (filePath.isEmpty())
		{
			return;
		}

		currentFilePath = filePath;

		QFileInfo fileInfo(filePath);
		mainWidget->setWindowTitle(fileInfo.completeBaseName() + " - " + titleName);

		saveProjectConfig(filePath);
		QString baseFilePath = fileInfo.path() + "/" + fileInfo.completeBaseName();
		saveToFile(baseFilePath + ".json");
		exportToFile(baseFilePath + ".tfvm");
		saveScreenshot(baseFilePath + ".png");
	}

	void openProject(const QString& filePath)
	{
		currentFilePath = filePath;

		QFileInfo fileInfo(filePath);
		mainWidget->setWindowTitle(fileInfo.completeBaseName() + " - " + titleName);

		QString baseFilePath = fileInfo.path() + "/" + fileInfo.completeBaseName();

		openFromFile(baseFilePath + ".json");
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

	void saveToFile(const QString& filePath)
	{
		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			printf("error: file.open()\n");
			return;
		}

		file.write(scene->saveToMemory());
	}

	void saveScreenshot(const QString& filePath)
	{
		scene->clearSelection();
		scene->setSceneRect(scene->itemsBoundingRect());
		QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		scene->render(&painter);
		image.save(filePath);
	}

	void exportToFile(const QString& filePath)
	{
		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			printf("error: file.open()\n");
			return;
		}

		std::vector<uint8_t> buffer = cGui::exportToMemory(scene);

		file.write((char*)&buffer[0], buffer.size());
		file.flush();
	}

	void openFromFile(const QString& filePath)
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

private slots:
	void menuNew()
	{
		currentFilePath = "";
		mainWidget->setWindowTitle(this->titleName);
		scene->clearScene();
	}

	void menuOpen()
	{
		QString fileName = QFileDialog::getOpenFileName(nullptr,
		                                                ("Open Project"),
		                                                QDir::homePath(),
		                                                ("TFVM Project (*.tfvmproject)"));

		openProject(fileName);
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

private:
	const cVirtualMachine* virtualMachine;
	QString titleName;
	QWidget* mainWidget;
	FlowScene* scene;
	FlowView* flowView;
	QString currentFilePath;
	QMap<tLibraryName, QTreeWidgetItem*> libraryLevelItems;
	QTreeWidget* treeView;
};

}

}

#endif // TFVM_GUI_H
