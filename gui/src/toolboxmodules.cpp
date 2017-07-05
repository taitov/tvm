#include <tvm/vm.h>

#include "toolboxmodules.h"
#include "treemodules.h"

using namespace nVirtualMachine::nGui;

cToolBoxModulesWidget::cToolBoxModulesWidget(const cVirtualMachine* virtualMachine,
                                             bool addSchemeModules) :
        virtualMachine(virtualMachine)
{
	cTreeModulesWidget* treeModulesAll = makeTreeModulesAll(addSchemeModules);
	addItem(treeModulesAll, "All");
}

cTreeModulesWidget* cToolBoxModulesWidget::makeTreeModulesAll(bool addSchemeModules)
{
	cTreeModulesWidget* treeModules = new cTreeModulesWidget();

	if (addSchemeModules)
	{
		QTreeWidgetItem* libraryItem = new QTreeWidgetItem((QTreeWidget*)treeModules);
		libraryItem->setText(0, "scheme");
		libraryItem->setData(0, Qt::UserRole, "");

		{
			QTreeWidgetItem* item = new QTreeWidgetItem(libraryItem);
			item->setText(0, "inSignal");
			item->setData(0, Qt::UserRole, ":scheme:inSignal");
		}

		{
			QTreeWidgetItem* item = new QTreeWidgetItem(libraryItem);
			item->setText(0, "outSignal");
			item->setData(0, Qt::UserRole, ":scheme:outSignal");
		}

		{
			QTreeWidgetItem* item = new QTreeWidgetItem(libraryItem);
			item->setText(0, "inMemory");
			item->setData(0, Qt::UserRole, ":scheme:inMemory");
		}

		{
			QTreeWidgetItem* item = new QTreeWidgetItem(libraryItem);
			item->setText(0, "outMemory");
			item->setData(0, Qt::UserRole, ":scheme:outMemory");
		}

		treeModules->expandItem(libraryItem);
	}

	{ /** memory type */
		QTreeWidgetItem* libraryItem = new QTreeWidgetItem((QTreeWidget*)treeModules);
		libraryItem->setText(0, "memory");
		libraryItem->setData(0, Qt::UserRole, "");

		std::map<QString, QTreeWidgetItem*> memoryMap;

		const auto memoryTypes = virtualMachine->getGuiMemoryTypes();
		for (const auto& memoryType : memoryTypes)
		{
			QString memoryTypeName = QString::fromUtf8(memoryType.first.value.c_str());
			cMemory* memory = memoryType.second;

			QTreeWidgetItem* memoryItem = new QTreeWidgetItem(libraryItem);
			memoryItem->setText(0, memoryTypeName);
			memoryItem->setData(0, Qt::UserRole, "");

			{
				QTreeWidgetItem* valueItem = new QTreeWidgetItem(memoryItem);
				valueItem->setText(0, "value");
				valueItem->setData(0, Qt::UserRole, ":memory:" + memoryTypeName);
			}

			memoryMap[memoryTypeName] = memoryItem;
		}

		std::map<std::tuple<QString,
		                    QString>,
		         QTreeWidgetItem*> memoryModuleMap;

		const auto memoryModules = virtualMachine->getGuiMemoryModules();
		for (const auto& memoryModule : memoryModules)
		{
			QString memoryTypeName = QString::fromUtf8(std::get<0>(memoryModule.first).value.c_str());
			QString moduleName = QString::fromUtf8(std::get<1>(memoryModule.first).value.c_str());
			QString moduleTypeName = QString::fromUtf8(memoryModule.second->getModuleTypeName().value.c_str());

			if (memoryMap.find(memoryTypeName) == memoryMap.end())
			{
				QTreeWidgetItem* memoryItem = new QTreeWidgetItem(libraryItem);
				memoryItem->setText(0, memoryTypeName);
				memoryItem->setData(0, Qt::UserRole, "");
				memoryMap[memoryTypeName] = memoryItem;
			}

			if (memoryModuleMap.find(std::make_tuple(memoryTypeName, moduleTypeName)) == memoryModuleMap.end())
			{
				QTreeWidgetItem* moduleTypeItem = new QTreeWidgetItem(memoryMap[memoryTypeName]);
				moduleTypeItem->setText(0, moduleTypeName);
				moduleTypeItem->setData(0, Qt::UserRole, "");
				memoryModuleMap[std::make_tuple(memoryTypeName, moduleTypeName)] = moduleTypeItem;
			}

			QTreeWidgetItem* moduleItem = new QTreeWidgetItem(memoryModuleMap[std::make_tuple(memoryTypeName, moduleTypeName)]);
			moduleItem->setText(0, moduleName);
			moduleItem->setData(0, Qt::UserRole, ":memory:" + memoryTypeName + ":" + moduleTypeName + ":" + moduleName);
		}

		treeModules->expandItem(libraryItem);
	}

	{ /** custom modules */
		/** @todo */
	}

	std::map<QString, QTreeWidgetItem*> libraryMap;

	{ /** root modules */
		std::map<QString, QTreeWidgetItem*> rootMap;

		const auto rootModules = virtualMachine->getGuiRootModules();
		for (const auto& rootModule : rootModules)
		{
			QString libraryName = QString::fromUtf8(std::get<0>(rootModule.first).value.c_str());
			QString rootModuleName = QString::fromUtf8(std::get<1>(rootModule.first).value.c_str());

			if (libraryMap.find(libraryName) == libraryMap.end())
			{
				QTreeWidgetItem* libraryItem = new QTreeWidgetItem((QTreeWidget*)treeModules);
				libraryItem->setText(0, libraryName);
				libraryItem->setData(0, Qt::UserRole, "");
				libraryMap[libraryName] = libraryItem;
				treeModules->expandItem(libraryItem);
			}

			if (rootMap.find(libraryName) == rootMap.end())
			{
				QTreeWidgetItem* rootItem = new QTreeWidgetItem(libraryMap[libraryName]);
				rootItem->setText(0, "root");
				rootItem->setData(0, Qt::UserRole, "");
				rootMap[libraryName] = rootItem;
			}

			QTreeWidgetItem* rootItem = new QTreeWidgetItem(rootMap[libraryName]);
			rootItem->setText(0, rootModuleName);
			rootItem->setData(0, Qt::UserRole, libraryName + ":root:" + rootModuleName);
		}
	}

	{ /** modules */
		std::map<std::tuple<QString,
		                    QString>,
		         QTreeWidgetItem*> moduleTypeMap;

		const auto modules = virtualMachine->getGuiModules();
		for (const auto& module : modules)
		{
			QString libraryName = QString::fromUtf8(std::get<0>(module.first).value.c_str());
			QString moduleName = QString::fromUtf8(std::get<1>(module.first).value.c_str());
			QString moduleTypeName = QString::fromUtf8(module.second->getModuleTypeName().value.c_str());

			if (libraryMap.find(libraryName) == libraryMap.end())
			{
				QTreeWidgetItem* libraryItem = new QTreeWidgetItem((QTreeWidget*)treeModules);
				libraryItem->setText(0, libraryName);
				libraryItem->setData(0, Qt::UserRole, "");
				libraryMap[libraryName] = libraryItem;
				treeModules->expandItem(libraryItem);
			}

			if (moduleTypeMap.find(std::make_tuple(libraryName, moduleTypeName)) == moduleTypeMap.end())
			{
				QTreeWidgetItem* moduleTypeItem = new QTreeWidgetItem(libraryMap[libraryName]);
				moduleTypeItem->setText(0, moduleTypeName);
				moduleTypeItem->setData(0, Qt::UserRole, "");
				moduleTypeMap[std::make_tuple(libraryName, moduleTypeName)] = moduleTypeItem;
			}

			QTreeWidgetItem* moduleItem = new QTreeWidgetItem(moduleTypeMap[std::make_tuple(libraryName, moduleTypeName)]);
			moduleItem->setText(0, moduleName);
			moduleItem->setData(0, Qt::UserRole, libraryName + ":" + moduleTypeName + ":" + moduleName);
		}
	}

	connect(treeModules, &cTreeModulesWidget::itemDoubleClicked, [this](QTreeWidgetItem* item, int column)
	{
		QString modelFullName = item->data(0, Qt::UserRole).toString();

		if (modelFullName == "")
		{
			return;
		}

		emit moduleClicked(modelFullName, item->text(0));
	});

	return treeModules;
}
