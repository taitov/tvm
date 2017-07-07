#ifndef CDOCUMENTWIDGET_H
#define CDOCUMENTWIDGET_H

#include <QtWidgets/QWidget>

namespace nVirtualMachine
{

namespace nGui
{

class cDocumentWidget : public QWidget
{
public:
	cDocumentWidget();

	virtual bool save() = 0;
	virtual bool saveAs() = 0;
	virtual bool open(const QString& filePath) = 0;

	virtual QString getDocumentName() = 0;

	virtual bool hasChanges() = 0;

	virtual void undo() = 0;
	virtual void redo() = 0;

	virtual QString getFilePath() = 0;

Q_SIGNALS:
	virtual void documentSaved() = 0;
};

}

}

#endif // CDOCUMENTWIDGET_H
