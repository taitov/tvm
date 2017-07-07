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
public:
	cCustomModulesWidget(const cVirtualMachine* virtualMachine,
	                     const std::vector<QString>& customModulePaths);

private:
	cDocumentWidget* createDocument() override;
};

}

}

#endif // CCUSTOMMODULESWIDGET_H
