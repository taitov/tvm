// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_LIBRARY_CURL_H
#define TFVM_LIBRARY_CURL_H

#include <vector>
#include <map>

#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>

#include <tfvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cCurl : public cLibrary
{
public:
	using tString = std::string;
	using tBuffer = std::vector<uint8_t>;

public:
	cCurl()
	{
	}

	bool registerLibrary() override
	{
		setLibraryName("curl");

		if (!registerModules(new cLogicGet()))
		{
			return false;
		}

		return true;
	}

private: /** modules */
	class cLogicGet : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicGet();
		}

		bool registerModule() override
		{
			setModuleName("get");

			if (!registerSignalEntry("signal", &cLogicGet::signalEntry))
			{
				return false;
			}

			if (!registerMemoryEntry("url", "string", url))
			{
				return false;
			}

			if (!registerSignalExit("done", signalExitDone))
			{
				return false;
			}

			if (!registerSignalExit("fail", signalExitFail))
			{
				return false;
			}

			if (!registerMemoryExit("asString", "string", string))
			{
				return false;
			}

			if (!registerMemoryExit("asBuffer", "buffer", buffer))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (!url)
			{
				return signalFlow(signalExitFail);
			}

			CURL* curl = curl_easy_init();
			if (!curl)
			{
				return signalFlow(signalExitFail);
			}

			curl_easy_setopt(curl, CURLOPT_URL, url->c_str());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

			CURLcode res = curl_easy_perform(curl);
			if(res != CURLE_OK)
			{
				curl_easy_cleanup(curl);
				return signalFlow(signalExitFail);
			}

			return signalFlow(signalExitDone);
		}

	private:
		static size_t curlWriteData(void* pointer, size_t size, size_t nmemb, void* args)
		{
			cLogicGet* module = (cLogicGet*)args;
			tString* string = module->string;
			tBuffer* buffer = module->buffer;

			if (string)
			{
				size_t length = string->size();
				string->resize(length + (size * nmemb));
				memcpy(&(*string)[length], pointer, (size * nmemb));
			}

			if (buffer)
			{
				size_t length = buffer->size();
				buffer->resize(length + (size * nmemb));
				memcpy(&(*buffer)[length], pointer, (size * nmemb));
			}

			return nmemb;
		}

	private:
		const tSignalExitId signalExitDone = 1;
		const tSignalExitId signalExitFail = 2;

	private:
		tString* url;
		tString* string;
		tBuffer* buffer;
	};
};

}

}

#endif // TFVM_LIBRARY_CURL_H
