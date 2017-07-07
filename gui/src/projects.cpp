#include "projects.h"
#include "project.h"

using namespace nVirtualMachine::nGui;

cProjectsWidget::cProjectsWidget(const cVirtualMachine* virtualMachine,
                                 const std::vector<QString>& customModulePaths) :
        cDocumentsWidget::cDocumentsWidget(),
        virtualMachine(virtualMachine),
        customModulePaths(customModulePaths)
{
}

void cProjectsWidget::setCustomModulePaths(const std::vector<QString>& customModulePaths)
{
	this->customModulePaths = customModulePaths;

	for (int project_i = 0; project_i < count(); project_i++)
	{
		cProjectWidget* projectWidget = (cProjectWidget*)widget(project_i);
		projectWidget->setCustomModulePaths(customModulePaths);
	}
}

cDocumentWidget* cProjectsWidget::createDocument()
{
	cProjectWidget* projectWidget = new cProjectWidget(virtualMachine,
	                                                   false,
	                                                   customModulePaths);

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
