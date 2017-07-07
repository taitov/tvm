#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include "custommodule.h"
#include "flowscene.h"

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
	emit projectNameChanged(fileInfo.completeBaseName());

	return true;
}
