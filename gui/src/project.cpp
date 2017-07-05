#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include "project.h"
#include "flowscene.h"
#include "flowview.h"
#include "toolboxmodules.h"

using namespace nVirtualMachine::nGui;

cProjectWidget::cProjectWidget(const cVirtualMachine* virtualMachine) :
        virtualMachine(virtualMachine)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QSplitter* splitter = new QSplitter();

	{ /** left panel */
		QVBoxLayout* layout = new QVBoxLayout();

		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);

		{ /** tool box */
			toolBox = new cToolBoxModulesWidget(virtualMachine, false);

			connect(toolBox, &cToolBoxModulesWidget::moduleClicked, [this](QString moduleFullName, QString moduleName)
			{
				auto type = flowScene->registry().create(moduleFullName);
				if (type)
				{
					auto* node = flowScene->createNode(std::move(type));
					if (node)
					{
						QPoint pos = flowView->rect().center();
						QPointF posView = flowView->mapToScene(pos);
						node->nodeGraphicsObject().setPos(posView);
					}
				}
			});

			layout->addWidget(toolBox);
		}

		QWidget* widget = new QWidget();
		widget->setLayout(layout);
		splitter->addWidget(widget);
	}

	{ /** graphics view */
		flowScene = new cFlowSceneWidget(virtualMachine);
		flowView = new cFlowViewWidget(flowScene);

		splitter->addWidget(flowView);
	}

	mainLayout->addWidget(splitter);
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

bool cProjectWidget::saveProject(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		printf("error: file.open()\n");
		return false;
	}

	file.write(flowScene->saveToMemory());

	this->filePath = filePath;

	QFileInfo fileInfo(filePath);
	emit projectNameChanged(fileInfo.completeBaseName());

	return true;
}
