#ifndef CPROJECTSWIDGET_H
#define CPROJECTSWIDGET_H

#include <QtWidgets/QTabWidget>

#include "documents.h"

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cProjectWidget;

class cProjectsWidget : public cDocumentsWidget
{
	Q_OBJECT

public:
	cProjectsWidget(const cVirtualMachine* virtualMachine);

private:
	cDocumentWidget* createDocument() override;

private:
	const cVirtualMachine* virtualMachine;
};

}

}

#endif // CPROJECTSWIDGET_H
