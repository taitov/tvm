#ifndef CDATA_H
#define CDATA_H

#include <QtCore/QString>

#include <tvm/type.h>

namespace nVirtualMachine
{

namespace nGui
{

class cData
{
public:
	cData();

	tModuleTypeName moduleTypeName;
	QString filePath;
};

}

}

#endif // CDATA_H
