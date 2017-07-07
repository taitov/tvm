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
	cProjectsWidget(const cVirtualMachine* virtualMachine,
	                const std::vector<QString>& customModulePaths);

public:
	void setCustomModulePaths(const std::vector<QString>& customModulePaths);

Q_SIGNALS:
	void openCustomModule(QString filePath);

private:
	cDocumentWidget* createDocument() override;

protected:
	const cVirtualMachine* virtualMachine;
	std::vector<QString> customModulePaths;
};

}

}

#endif // CPROJECTSWIDGET_H
