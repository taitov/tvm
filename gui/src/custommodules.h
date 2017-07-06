#ifndef CCUSTOMMODULESWIDGET_H
#define CCUSTOMMODULESWIDGET_H

#include "documents.h"

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cCustomModulesWidget : public cDocumentsWidget
{
public:
	cCustomModulesWidget(const cVirtualMachine* virtualMachine);

private:
	cDocumentWidget* createDocument() override;

private:
	const cVirtualMachine* virtualMachine;
};

}

}

#endif // CCUSTOMMODULESWIDGET_H
