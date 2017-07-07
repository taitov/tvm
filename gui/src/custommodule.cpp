#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include "custommodule.h"
#include "flowscene.h"
#include "toolboxmodules.h"

using namespace nVirtualMachine::nGui;

cCustomModuleWidget::cCustomModuleWidget(const cVirtualMachine* virtualMachine,
                                         const std::vector<QString>& customModulePaths) :
        cProjectWidget::cProjectWidget(virtualMachine, true, customModulePaths)
{
}

bool cCustomModuleWidget::save()
{
	if (filePath.isEmpty())
	{
		QString filePath = QFileDialog::getSaveFileName(nullptr,
		                                                ("Save Custom Module"),
		                                                QDir::homePath(), /**< @todo: last dir */
		                                                ("TVM Custom Module (*.tvmcustom)"));

		if (filePath.isEmpty())
		{
			return false;
		}

		if (!filePath.endsWith(".tvmcustom", Qt::CaseInsensitive))
		{
			filePath += ".tvmcustom";
		}

		return saveCustomModule(filePath);
	}

	return saveCustomModule(filePath);
}

bool cCustomModuleWidget::saveAs()
{
	QString filePath = QFileDialog::getSaveFileName(nullptr,
	                                                ("Save As Custom Module"),
	                                                QDir::homePath(), /**< @todo: last dir */
	                                                ("TVM Custom Module (*.tvmcustom)"));

	if (filePath.isEmpty())
	{
		return false;
	}

	if (!filePath.endsWith(".tvmcustom", Qt::CaseInsensitive))
	{
		filePath += ".tvmcustom";
	}

	return saveCustomModule(filePath);
}

void cCustomModuleWidget::setCustomModulePaths(const std::vector<QString>& customModulePaths)
{
	this->customModulePaths = customModulePaths;

	std::vector<QString> totalCustomModulePaths;

	if (!filePath.isEmpty())
	{
		QString rootPath = getRootDirectory(filePath);
		if (!rootPath.isEmpty())
		{
			totalCustomModulePaths.emplace_back(rootPath);
		}
	}

	totalCustomModulePaths.insert(totalCustomModulePaths.end(),
	                              customModulePaths.begin(),
	                              customModulePaths.end());

	/** @todo: delete duplicates */

	toolBox->setCustomModulePaths(totalCustomModulePaths);
	flowScene->setCustomModulePaths(totalCustomModulePaths);
}

bool cCustomModuleWidget::saveCustomModule(const QString& filePath)
{
	QByteArray byteArray = flowScene->saveToMemory();

	{
		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			printf("error: file.open()\n");
			return false;
		}

		file.write(byteArray);
	}

	QFileInfo fileInfo(filePath);

	this->filePath = filePath;
	flagHasChanges = false;

	emit projectChanged(flagHasChanges);
	emit documentSaved();

	return true;
}

QString cCustomModuleWidget::getRootDirectory(const QString& filePath)
{
	if (filePath.isEmpty() ||
	    filePath == "/")
	{
		return "";
	}

	QFileInfo fileInfo(filePath);
	if (fileInfo.path().endsWith("/customModules"))
	{
		return fileInfo.path();
	}

	return getRootDirectory(fileInfo.path());
}
