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
};

}

}

#endif // CCUSTOMMODULEWIDGET_H
