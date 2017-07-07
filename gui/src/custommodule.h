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
	cCustomModuleWidget(const cVirtualMachine* virtualMachine,
	                    const std::vector<QString>& customModulePaths);

	bool save() override;
	bool saveAs() override;

public:
	void setCustomModulePaths(const std::vector<QString>& customModulePaths) override;

private:
	bool saveCustomModule(const QString& filePath);
	QString getRootDirectory(const QString& filePath);
};

}

}

#endif // CCUSTOMMODULEWIDGET_H
