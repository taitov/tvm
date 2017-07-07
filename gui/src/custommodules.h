#ifndef CCUSTOMMODULESWIDGET_H
#define CCUSTOMMODULESWIDGET_H

#include "projects.h"

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cCustomModulesWidget : public cProjectsWidget
{
	Q_OBJECT

public:
	cCustomModulesWidget(const cVirtualMachine* virtualMachine,
	                     const std::vector<QString>& customModulePaths);

public:
	void setCustomModulePaths(const std::vector<QString>& customModulePaths);

Q_SIGNALS:
	void customModuleUpdated();

private:
	cDocumentWidget* createDocument() override;
};

}

}

#endif // CCUSTOMMODULESWIDGET_H
