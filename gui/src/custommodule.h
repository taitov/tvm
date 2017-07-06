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
	bool open(const QString& filePath) override;
};

}

}

#endif // CCUSTOMMODULEWIDGET_H
