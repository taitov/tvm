#include "custommodules.h"
#include "custommodule.h"

using namespace nVirtualMachine::nGui;

cCustomModulesWidget::cCustomModulesWidget(const cVirtualMachine* virtualMachine,
                                           const std::vector<QString>& customModulePaths) :
        cProjectsWidget::cProjectsWidget(virtualMachine, customModulePaths)
{
}

void cCustomModulesWidget::setCustomModulePaths(const std::vector<QString>& customModulePaths)
{
	this->customModulePaths = customModulePaths;

	for (int custom_i = 0; custom_i < count(); custom_i++)
	{
		cCustomModuleWidget* customWidget = (cCustomModuleWidget*)widget(custom_i);
		customWidget->setCustomModulePaths(customModulePaths);
	}
}

cDocumentWidget* cCustomModulesWidget::createDocument()
{
	cCustomModuleWidget* customWidget = new cCustomModuleWidget(virtualMachine,
	                                                            customModulePaths);

	connect(customWidget, &cCustomModuleWidget::documentSaved, this, [this, customWidget]()
	{
		int index = indexOf(customWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, customWidget->getDocumentName());
		emit customModuleUpdated();
	});

	connect(customWidget, &cCustomModuleWidget::projectChanged, this, [this, customWidget](bool flagHasChanges)
	{
		int index = indexOf(customWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, customWidget->getDocumentName() + (flagHasChanges ? "*" : ""));
	});

	connect(customWidget, &cCustomModuleWidget::openCustomModule, this, [this, customWidget](QString filePath)
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
