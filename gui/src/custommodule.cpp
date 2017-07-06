#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include "custommodule.h"

using namespace nVirtualMachine::nGui;

cCustomModuleWidget::cCustomModuleWidget(const cVirtualMachine* virtualMachine) :
        cProjectWidget::cProjectWidget(virtualMachine, true)
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

		return saveProject(filePath);
	}

	return saveProject(filePath);
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

	return saveProject(filePath);
}

bool cCustomModuleWidget::open(const QString& filePath)
{
	return openProject(filePath);
}
