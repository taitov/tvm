#include "custommodules.h"
#include "custommodule.h"

using namespace nVirtualMachine::nGui;

cCustomModulesWidget::cCustomModulesWidget(const cVirtualMachine* virtualMachine,
                                           const std::vector<QString>& customModulePaths) :
        cProjectsWidget::cProjectsWidget(virtualMachine, customModulePaths)
{
}

cDocumentWidget* cCustomModulesWidget::createDocument()
{
	cCustomModuleWidget* customWidget = new cCustomModuleWidget(virtualMachine,
	                                                            customModulePaths);

	connect(customWidget, &cProjectWidget::projectNameChanged, this, [this, customWidget](QString projectName)
	{
		int index = indexOf(customWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, projectName);
	});

	connect(customWidget, &cProjectWidget::projectChanged, this, [this, customWidget](bool flagHasChanges)
	{
		int index = indexOf(customWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, customWidget->getDocumentName() + (flagHasChanges ? "*" : ""));
	});

	connect(customWidget, &cProjectWidget::openCustomModule, this, [this, customWidget](QString filePath)
	{
		int index = indexOf(customWidget);
		if (index < 0)
		{
			return;
		}

		open(filePath);
	});

	return (cDocumentWidget*)customWidget;
}
