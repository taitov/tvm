#ifndef MEMORY_STRING1_H
#define MEMORY_STRING1_H

#include <string>

#include <tvm/register.h>

namespace tvm
{

class cMemoryString1 : public std::string
{
/// @todo: cMemoryString1()
public:
	tvmRegisterMemoryModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_1_");
		registerModule.setMemoryAsString();
	}

public:
	bool restore(cStreamIn& stream);
	void save(cStreamOut& stream) const;

public:
	using std::string::string;

public:
	std::string functionOnlyForEngine() const;
};

}

#endif // MEMORY_STRING1_H
