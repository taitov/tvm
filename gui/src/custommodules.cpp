#include "custommodules.h"
#include "custommodule.h"

using namespace nVirtualMachine::nGui;

cCustomModulesWidget::cCustomModulesWidget(const cVirtualMachine* virtualMachine) :
        cDocumentsWidget::cDocumentsWidget(),
        virtualMachine(virtualMachine)
{
}

cDocumentWidget* cCustomModulesWidget::createDocument()
{
	cCustomModuleWidget* customWidget = new cCustomModuleWidget(virtualMachine);

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

	return (cDocumentWidget*)customWidget;
}
