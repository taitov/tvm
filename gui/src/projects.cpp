#include "projects.h"
#include "project.h"

using namespace nVirtualMachine::nGui;

cProjectsWidget::cProjectsWidget(const cVirtualMachine* virtualMachine) :
        cDocumentsWidget::cDocumentsWidget(),
        virtualMachine(virtualMachine)
{
}

cDocumentWidget* cProjectsWidget::createDocument()
{
	cProjectWidget* projectWidget = new cProjectWidget(virtualMachine, false);

	connect(projectWidget, &cProjectWidget::projectNameChanged, this, [this, projectWidget](QString projectName)
	{
		int index = indexOf(projectWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, projectName);
	});

	connect(projectWidget, &cProjectWidget::projectChanged, this, [this, projectWidget](bool flagHasChanges)
	{
		int index = indexOf(projectWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, projectWidget->getDocumentName() + (flagHasChanges ? "*" : ""));
	});

	connect(projectWidget, &cProjectWidget::openCustomModule, this, [this, projectWidget](QString filePath)
	{
		int index = indexOf(projectWidget);
		if (index < 0)
		{
			return;
		}

		emit openCustomModule(filePath);
	});

	return (cDocumentWidget*)projectWidget;
}
