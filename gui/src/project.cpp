#include "project.h"
#include "flowscene.h"
#include "flowview.h"

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

		{ /** tree view */
//			treeView = new cTreeViewModules(virtualMachine);
//			layout->addWidget(treeView);
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
