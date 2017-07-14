#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include <tvm/export/json.h>

#include "project.h"
#include "flowscene.h"
#include "flowview.h"
#include "toolboxmodules.h"
#include "data.h"

using namespace nVirtualMachine::nGui;

cProjectWidget::cProjectWidget(const cVirtualMachine* virtualMachine,
                               bool addSchemeModules,
                               const std::vector<QString>& customModulePaths) :
        virtualMachine(virtualMachine),
        customModulePaths(customModulePaths)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QSplitter* splitter = new QSplitter();

	{ /** left panel */
		QVBoxLayout* layout = new QVBoxLayout();

		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);

		{ /** tool box */
			toolBox = new cToolBoxModulesWidget(virtualMachine, addSchemeModules);

			connect(toolBox, &cToolBoxModulesWidget::moduleClicked, [this](QString moduleFullName, QString moduleName)
			{
				auto type = flowScene->registry().create(moduleFullName);
				if (type)
				{
					QPoint pos = flowView->rect().center();
					QPointF posView = flowView->mapToScene(pos);
					flowScene->createNode(std::move(type), posView);
				}
			});

			layout->addWidget(toolBox);
		}

		QWidget* widget = new QWidget();
		widget->setLayout(layout);
		splitter->addWidget(widget);
	}

	{ /** graphics view */
		flowScene = new cFlowSceneWidget(virtualMachine, addSchemeModules);
		flowView = new cFlowViewWidget(flowScene);

		connect(flowScene, &cFlowSceneWidget::nodeCreated, this, [this](QtNodes::Node& node)
		{
			addAction(cAction::nodeCreated, node);
			prevPositions[node.id()] = node.nodeGraphicsObject().pos();
			flagHasChanges = true;
			emit projectChanged(flagHasChanges);
		});

		connect(flowScene, &cFlowSceneWidget::nodeDeleted, this, [this](QtNodes::Node& node)
		{
			addAction(cAction::nodeDeleted, node);
			flagHasChanges = true;
			emit projectChanged(flagHasChanges);
		});

		connect(flowScene, &cFlowSceneWidget::connectionCreated, this, [this](QtNodes::Connection& connection)
		{
			connect(&connection, &QtNodes::Connection::updated, this, [this](QtNodes::Connection& connection)
			{
				flagHasChanges = true;
				emit projectChanged(flagHasChanges);
			});

			if (!connection.getNode(QtNodes::PortType::In) ||
			    !connection.getNode(QtNodes::PortType::Out))
			{
				return;
			}

			flagHasChanges = true;
			emit projectChanged(flagHasChanges);
		});

		connect(flowScene, &cFlowSceneWidget::connectionDeleted, this, [this](QtNodes::Connection& connection)
		{
			flagHasChanges = true;
			emit projectChanged(flagHasChanges);
		});

		connect(flowScene, &cFlowSceneWidget::nodeMoved, this, [this](QtNodes::Node& node, const QPointF& newLocation)
		{
			if (prevPositions[node.id()] == newLocation)
			{
				return;
			}
			addAction(cAction::nodeMoved, node, prevPositions[node.id()], newLocation);
			prevPositions[node.id()] = newLocation;
			flagHasChanges = true;
			emit projectChanged(flagHasChanges);
		});

		connect(flowScene, &cFlowSceneWidget::nodeDoubleClicked, this, [this](QtNodes::Node& node)
		{
			const cData* data = (const cData*)node.nodeDataModel()->getData();
			if (data && data->moduleTypeName.value == "custom")
			{
				emit openCustomModule(data->filePath);
			}
		});

		splitter->addWidget(flowView);
	}

	mainLayout->addWidget(splitter);

	setCustomModulePaths(customModulePaths);

	flagHasChanges = false;
	actionPosition = 0;
}

bool cProjectWidget::save()
{
	if (filePath.isEmpty())
	{
		QString filePath = QFileDialog::getSaveFileName(nullptr,
		                                                ("Save Project"),
		                                                QDir::homePath(), /**< @todo: last dir */
		                                                ("TVM Project (*.tvmproject)"));

		if (filePath.isEmpty())
		{
			return false;
		}

		if (!filePath.endsWith(".tvmproject", Qt::CaseInsensitive))
		{
			filePath += ".tvmproject";
		}

		return saveProject(filePath);
	}

	return saveProject(filePath);
}

bool cProjectWidget::saveAs()
{
	QString filePath = QFileDialog::getSaveFileName(nullptr,
	                                                ("Save As Project"),
	                                                QDir::homePath(), /**< @todo: last dir */
	                                                ("TVM Project (*.tvmproject)"));

	if (filePath.isEmpty())
	{
		return false;
	}

	if (!filePath.endsWith(".tvmproject", Qt::CaseInsensitive))
	{
		filePath += ".tvmproject";
	}

	return saveProject(filePath);
}

bool cProjectWidget::open(const QString& filePath)
{
	return openProject(filePath);
}

QString cProjectWidget::getDocumentName()
{
	if (filePath.isEmpty())
	{
		return "untitled";
	}

	QFileInfo fileInfo(filePath);
	return fileInfo.completeBaseName();
}

bool cProjectWidget::hasChanges()
{
	return flagHasChanges;
}

void cProjectWidget::undo()
{
	if (!actionPosition)
	{
		return;
	}

	const cAction& action = actions[actionPosition - 1];
	if (action.type == cAction::nodeCreated)
	{
		const auto& nodes = flowScene->nodes();

		if (nodes.find(action.id) == nodes.end())
		{
			printf("error: undo()\n");
			actionPosition = 0;
			actions.clear();
			return;
		}

		flowScene->removeNode(*(nodes.find(action.id)->second),
		                      false);
	}
	else if (action.type == cAction::nodeDeleted)
	{
		auto type = flowScene->registry().create(action.moduleFullName);
		if (type)
		{
			auto* node = flowScene->createNode(std::move(type),
			                                   action.id,
			                                   action.position,
			                                   false);
			prevPositions[node->id()] = node->nodeGraphicsObject().pos();
		}
	}
	else if (action.type == cAction::nodeMoved)
	{
		const auto& nodes = flowScene->nodes();

		if (nodes.find(action.id) == nodes.end())
		{
			printf("error: undo()\n");
			actionPosition = 0;
			actions.clear();
			return;
		}

		nodes.find(action.id)->second->nodeGraphicsObject().setPos(action.fromPosition);
		nodes.find(action.id)->second->nodeGraphicsObject().moveConnections();
		prevPositions[action.id] = action.fromPosition;
	}

	flagHasChanges = true;
	emit projectChanged(flagHasChanges);

	actionPosition--;
}

void cProjectWidget::redo()
{
	if (actionPosition >= actions.size())
	{
		return;
	}

	const cAction& action = actions[actionPosition];
	if (action.type == cAction::nodeCreated)
	{
		auto type = flowScene->registry().create(action.moduleFullName);
		if (type)
		{
			auto* node = flowScene->createNode(std::move(type),
			                                   action.id,
			                                   action.position,
			                                   false);
			prevPositions[node->id()] = node->nodeGraphicsObject().pos();
		}
	}
	else if (action.type == cAction::nodeDeleted)
	{
		const auto& nodes = flowScene->nodes();

		if (nodes.find(action.id) == nodes.end())
		{
			printf("error: redo()\n");
			actionPosition = 0;
			actions.clear();
			return;
		}

		flowScene->removeNode(*(nodes.find(action.id)->second),
		                      false);
	}
	else if (action.type == cAction::nodeMoved)
	{
		const auto& nodes = flowScene->nodes();

		if (nodes.find(action.id) == nodes.end())
		{
			printf("error: undo()\n");
			actionPosition = 0;
			actions.clear();
			return;
		}

		nodes.find(action.id)->second->nodeGraphicsObject().setPos(action.toPosition);
		nodes.find(action.id)->second->nodeGraphicsObject().moveConnections();
		prevPositions[action.id] = action.toPosition;
	}

	flagHasChanges = true;
	emit projectChanged(flagHasChanges);

	actionPosition++;
}

QString cProjectWidget::getFilePath()
{
	return filePath;
}

void cProjectWidget::setCustomModulePaths(const std::vector<QString>& customModulePaths)
{
	this->customModulePaths = customModulePaths;

	std::vector<QString> totalCustomModulePaths;

	if (!filePath.isEmpty())
	{
		QFileInfo fileInfo(filePath);
		totalCustomModulePaths.emplace_back(fileInfo.path() + "/customModules");
	}

	totalCustomModulePaths.insert(totalCustomModulePaths.end(),
	                              customModulePaths.begin(),
	                              customModulePaths.end());

	/** @todo: delete duplicates */

	toolBox->setCustomModulePaths(totalCustomModulePaths);
	flowScene->setCustomModulePaths(totalCustomModulePaths);
}

bool cProjectWidget::saveProject(const QString& filePath)
{
	QByteArray byteArray = flowScene->saveToMemory();

	{
		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			printf("error: file.open()\n");
			return false;
		}

		file.write(byteArray);
	}

	QFileInfo fileInfo(filePath);

	{
		QFile file(fileInfo.path() + "/" + fileInfo.completeBaseName() + ".tvm");
		if (!file.open(QIODevice::WriteOnly))
		{
			printf("error: file.open()\n");
			return false;
		}

		std::vector<QString> totalCustomModulePaths;

		if (!filePath.isEmpty())
		{
			QFileInfo fileInfo(filePath);
			totalCustomModulePaths.emplace_back(fileInfo.path() + "/customModules");
		}

		totalCustomModulePaths.insert(totalCustomModulePaths.end(),
		                              customModulePaths.begin(),
		                              customModulePaths.end());

		std::vector<uint8_t> buffer = nExport::cJson::exportToMemory(byteArray,
		                                                             totalCustomModulePaths);
		file.write((char*)&buffer[0], buffer.size());
	}

	{
		flowScene->clearSelection();
		flowScene->setSceneRect(flowScene->itemsBoundingRect());

		QImage image(flowScene->sceneRect().size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		flowScene->render(&painter);

		image.save(fileInfo.path() + "/" + fileInfo.completeBaseName() + ".png");
	}

	{
		QDir().mkdir(fileInfo.path() + "/customModules");
	}

	this->filePath = filePath;
	flagHasChanges = false;

	emit projectChanged(flagHasChanges);
	emit documentSaved();

	return true;
}

bool cProjectWidget::openProject(const QString& filePath)
{
	if (!QFileInfo::exists(filePath))
	{
		printf("error: QFileInfo::exists()\n");
		return false;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		printf("error: file.open()\n");
		return false;
	}

	this->filePath = filePath;

	setCustomModulePaths(customModulePaths);

	QByteArray wholeFile = file.readAll();
	flowScene->loadFromMemory(wholeFile);

	flagHasChanges = false;
	actionPosition = 0;
	actions.clear();

	for (const auto& iter : flowScene->nodes())
	{
		prevPositions[iter.first] = iter.second->nodeGraphicsObject().pos();
	}

	return true;
}

cProjectWidget::cAction::cAction() :
        type(none)
{
}

cProjectWidget::cAction::cAction(cProjectWidget::cAction::eType type, const QtNodes::Node& node) :
        type(type)
{
	id = node.id();
	moduleFullName = node.nodeDataModel()->name();
	position = node.nodeGraphicsObject().pos();
}

cProjectWidget::cAction::cAction(cProjectWidget::cAction::eType type, const QtNodes::Node& node,
                                 const QPointF& fromPosition, const QPointF& toPosition) :
        type(type),
        fromPosition(fromPosition),
        toPosition(toPosition)
{
	id = node.id();
	moduleFullName = node.nodeDataModel()->name();
}
