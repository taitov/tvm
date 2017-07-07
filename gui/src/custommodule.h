#ifndef CCUSTOMMODULEWIDGET_H
#define CCUSTOMMODULEWIDGET_H

#include "project.h"

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cCustomModuleWidget : public cProjectWidget
{
public:
	cCustomModuleWidget(const cVirtualMachine* virtualMachine);

	bool save() override;
	bool saveAs() override;

private:
	bool saveCustomModule(const QString& filePath);
};

}

}

#endif // CCUSTOMMODULEWIDGET_H
