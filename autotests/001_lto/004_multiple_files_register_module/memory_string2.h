#ifndef MEMORY_STRING2_H
#define MEMORY_STRING2_H

#include <string>

#include <tvm/register.h>

namespace tvm
{

class cMemoryString2 : public std::string
{
/// @todo: cMemoryString2()
public:
	tvmRegisterMemoryModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_3_");
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

#endif // MEMORY_STRING2_H
