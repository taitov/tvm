#include <QtWidgets/QMessageBox>

#include "documents.h"
#include "document.h"

using namespace nVirtualMachine::nGui;

cDocumentsWidget::cDocumentsWidget()
{
	setMovable(true);
	setTabsClosable(true);

	connect(this, &cDocumentsWidget::tabCloseRequested, this, [this](int index)
	{
		cDocumentWidget* documentWidget = (cDocumentWidget*)widget(index);
		if (documentWidget->hasChanges())
		{
			QMessageBox messageBox;
			if (documentWidget->getDocumentName().isEmpty())
			{
				messageBox.setText("The document has been modified");
			}
			else
			{
				messageBox.setText("The document '" + documentWidget->getDocumentName() + "' has been modified");
			}
			messageBox.setInformativeText("Do you want to save your changes?");
			messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			messageBox.setDefaultButton(QMessageBox::Save);
			int rc = messageBox.exec();
			if (rc == QMessageBox::Save)
			{
				if (!documentWidget->save())
				{
					return;
				}
			}
			else if (rc == QMessageBox::Discard)
			{
			}
			else
			{
				return;
			}
		}
		filePaths.erase(documentWidget->getFilePath());
		removeTab(index);
	});

	connect(this, &cDocumentsWidget::currentChanged, this, [this](int index)
	{
		cDocumentWidget* documentWidget = (cDocumentWidget*)widget(index);
		emit currentDocumentChanged(documentWidget);
	});
}

void cDocumentsWidget::newDocument()
{
	cDocumentWidget* documentWidget = createDocument();
	addTab(documentWidget, documentWidget->getDocumentName());
	setCurrentWidget(documentWidget);
}

bool cDocumentsWidget::open(const QString& filePath)
{
	if (filePaths.find(filePath) != filePaths.end())
	{
		if (currentWidget() != filePaths[filePath])
		{
			setCurrentWidget(filePaths[filePath]);
		}
		else
		{
			emit currentDocumentChanged(filePaths[filePath]);
		}
		return true;
	}

	cDocumentWidget* documentWidget = createDocument();
	if (!documentWidget->open(filePath))
	{
		delete documentWidget;
		return false;
	}

	addTab(documentWidget, documentWidget->getDocumentName());
	setCurrentWidget(documentWidget);

	filePaths[filePath] = documentWidget;
	return true;
}

bool cDocumentsWidget::save()
{
	if (currentWidget())
	{
		cDocumentWidget* documentWidget = (cDocumentWidget*)currentWidget();
		if (!documentWidget->save())
		{
			return false;
		}
		filePaths[documentWidget->getFilePath()] = documentWidget;
		return true;
	}
	return false;
}

bool cDocumentsWidget::saveAs()
{
	if (currentWidget())
	{
		cDocumentWidget* documentWidget = (cDocumentWidget*)currentWidget();
		if (!documentWidget->saveAs())
		{
			return false;
		}
		filePaths[documentWidget->getFilePath()] = documentWidget;
		return true;
	}
	return false;
}

bool cDocumentsWidget::saveAllDocuments()
{
	for (int document_i = 0; document_i < count(); document_i++)
	{
		cDocumentWidget* documentWidget = (cDocumentWidget*)widget(document_i);
		setCurrentWidget(documentWidget);
		if (!documentWidget->save())
		{
			return false;
		}
	}
	return true;
}

bool cDocumentsWidget::closeAllDocuments()
{
	for (int document_i = 0; document_i < count(); document_i++)
	{
		cDocumentWidget* documentWidget = (cDocumentWidget*)widget(document_i);
		if (documentWidget->hasChanges())
		{
			setCurrentWidget(documentWidget);
			QMessageBox messageBox;
			if (documentWidget->getDocumentName().isEmpty())
			{
				messageBox.setText("The document has been modified");
			}
			else
			{
				messageBox.setText("The document '" + documentWidget->getDocumentName() + "' has been modified");
			}
			messageBox.setInformativeText("Do you want to save your changes?");
			messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			messageBox.setDefaultButton(QMessageBox::Save);
			int rc = messageBox.exec();
			if (rc == QMessageBox::Save)
			{
				if (!documentWidget->save())
				{
					return false;
				}
			}
			else if (rc == QMessageBox::Discard)
			{
			}
			else
			{
				return false;
			}
		}
	}

	for (int document_i = 0; document_i < count(); document_i++)
	{
		removeTab(document_i);
	}

	filePaths.clear();

	return true;
}

void cDocumentsWidget::undo()
{
	if (currentWidget())
	{
		cDocumentWidget* documentWidget = (cDocumentWidget*)currentWidget();
		return documentWidget->undo();
	}
}

void cDocumentsWidget::redo()
{
	if (currentWidget())
	{
		cDocumentWidget* documentWidget = (cDocumentWidget*)currentWidget();
		return documentWidget->redo();
	}
}
