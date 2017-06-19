#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <tfvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cExample : public cLibrary
{
public:
	cExample()
	{
	}

	cExample(const std::string& ipAddress, uint16_t port)
	{
		this->ipAddress = ipAddress;
		this->port = port;

		serverSocket = -1;
		clientSocket = -1;
	}

	bool registerLibrary() override
	{
		setLibraryName("example");

		if (!registerMemory<std::string>("string"))
		{
			return false;
		}

		if (!registerMemory<uint32_t>("ipv4", 0))
		{
			return false;
		}

		if (!registerRootModules(rootGet))
		{
			return false;
		}

		if (!registerModules(new cLogicPrint(),
		                     new cLogicOk(this),
		                     new cLogicConvert(),
		                     new cLogicAppend()))
		{
			return false;
		}

		return true;
	}

	bool init() override
	{
		serverSocket = socket(PF_INET, SOCK_STREAM, 0);
		if (serverSocket < 0)
		{
			serverSocket = -1;
			return false;
		}

		if (fcntl(serverSocket, F_GETFL) & O_NONBLOCK)
		{
			if (fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL) & (~O_NONBLOCK)) < 0)
			{
				return false;
			}
		}

		int one = 1;
		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
		{
			return false;
		}

		return true;
	}

	void run() override
	{
		struct sockaddr_in address;
		memset((char*)&address, 0, sizeof(struct sockaddr_in));
		address.sin_family = PF_INET;
		address.sin_addr.s_addr = inet_addr(ipAddress.c_str());
		address.sin_port = htons(port);

		if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0)
		{
			return;
		}

		if (listen(serverSocket, 64) < 0)
		{
			return;
		}

		while (!isStopped())
		{
			struct sockaddr_in address;
			socklen_t addressLength = sizeof(address);
			clientSocket = accept(serverSocket, (struct sockaddr*)&address, &addressLength);
			if (clientSocket < 0)
			{
				continue;
			}

			std::string request;
			for (;;)
			{
				char buffer[8192];

				int recvLength = recv(clientSocket, buffer, sizeof(buffer), MSG_NOSIGNAL);
				if (recvLength <= 0)
				{
					break;
				}

				request.insert(request.length(), buffer, recvLength);
				if (request.substr(request.length() - 4, 4) == "\r\n\r\n")
				{
					break;
				}
			}

			if (request.substr(0, 3) == "GET")
			{
				std::string host = request.substr(request.find("Host: ") + 6);
				host = host.substr(0, host.find("\r\n"));
				std::string url = request.substr(4);
				url = url.substr(0, url.find("\r\n") - 9);

				rootSetMemory(rootGet.memoryIpAddress, address.sin_addr.s_addr);
				rootSetMemory(rootGet.memoryHost, host);
				rootSetMemory(rootGet.memoryUrl, url);
				if (!rootSignalFlow(rootGet.signal))
				{
					close(clientSocket);
					clientSocket = -1;
				}
			}
			else
			{
				close(clientSocket);
				clientSocket = -1;
			}
		}
	}

	void stop() override
	{
		if (serverSocket != -1)
		{
			shutdown(serverSocket, SHUT_RDWR);
			close(serverSocket);
			serverSocket = -1;
		}
	}

private:
	std::string ipAddress;
	uint16_t port;
	int serverSocket;
	int clientSocket;

private: /** rootModules */
	class cRootGet : public cRootModule
	{
	public:
		bool registerModule() override
		{
			setModuleName("GET");

			if (!registerSignalExit("signal", signal))
			{
				return false;
			}

			if (!registerMemoryExit("ipAddress", "ipv4", memoryIpAddress))
			{
				return false;
			}

			if (!registerMemoryExit("host", "string", memoryHost))
			{
				return false;
			}

			if (!registerMemoryExit("url", "string", memoryUrl))
			{
				return false;
			}

			return true;
		}

		tRootSignalExitId signal;
		tRootMemoryExitId memoryIpAddress;
		tRootMemoryExitId memoryHost;
		tRootMemoryExitId memoryUrl;
	};

private:
	cRootGet rootGet;

private: /** modules */
	class cLogicPrint : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicPrint();
		}

		bool registerModule() override
		{
			setModuleName("print");

			if (!registerSignalEntry("print", &cLogicPrint::signalEntryPrint))
			{
				return false;
			}

			if (!registerMemoryEntry("string", "string", string))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExitDone))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntryPrint()
		{
			if (string)
			{
				printf("%s\n", string->c_str());
			}
			return signalFlow(signalExitDone);
		}

	private:
		const tSignalExitId signalExitDone = 1;

	private:
		std::string* string;
	};

	class cLogicOk : public cLogicModule
	{
	public:
		cLogicOk(cExample* library)
		{
			this->library = library;
		}

		cModule* clone() const override
		{
			return new cLogicOk(library);
		}

		bool registerModule() override
		{
			setModuleName("OK");

			if (!registerSignalEntry("signal", &cLogicOk::signalEntrySignal))
			{
				return false;
			}

			if (!registerMemoryEntry("title", "string", title))
			{
				return false;
			}

			if (!registerMemoryEntry("body", "string", body))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntrySignal()
		{
			std::string response;
			response = "HTTP/1.1 200 OK\r\nServer: example\r\n\r\n";

			if (title)
			{
				response += "<HEAD><TITLE>" + *title + "</TITLE></HEAD>";
			}
			else
			{
				response += "<HEAD><TITLE></TITLE></HEAD>";
			}

			if (body)
			{
				response += "<BODY>" + *body + "</BODY>";
			}
			else
			{
				response += "<BODY></BODY>";
			}

			send(library->clientSocket, response.c_str(), response.length(), MSG_NOSIGNAL);

			close(library->clientSocket);
			library->clientSocket = -1;
			return true;
		}

	private:
		cExample* library;

	private:
		std::string* title;
		std::string* body;
	};

	class cLogicConvert : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicConvert();
		}

		bool registerModule() override
		{
			setModuleName("convert");

			if (!registerSignalEntry("signal", &cLogicConvert::signalEntrySignal))
			{
				return false;
			}

			if (!registerMemoryEntry("ipAddress", "ipv4", ipAddress))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExitDone))
			{
				return false;
			}

			if (!registerMemoryExit("string", "string", string))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntrySignal()
		{
			if (ipAddress && string)
			{
				char ipAddressStr[16];
				snprintf(ipAddressStr, 16, "%u.%u.%u.%u",
					 ((*ipAddress) & 0xFF),
					 (((*ipAddress) >> 8) & 0xFF),
					 (((*ipAddress) >> 16) & 0xFF),
					 (((*ipAddress) >> 24) & 0xFF));
				*string = std::string(ipAddressStr);
			}

			return signalFlow(signalExitDone);
		}

	private:
		const tSignalExitId signalExitDone = 1;

	private:
		uint32_t* ipAddress;
		std::string* string;
	};

	class cLogicAppend : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicAppend();
		}

		bool registerModule() override
		{
			setModuleName("append");

			if (!registerSignalEntry("signal", &cLogicAppend::signalEntrySignal))
			{
				return false;
			}

			if (!registerMemoryEntry("first", "string", first))
			{
				return false;
			}

			if (!registerMemoryEntry("second", "string", second))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExitSignal))
			{
				return false;
			}

			if (!registerMemoryExit("result", "string", result))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntrySignal()
		{
			if (result && first && second)
			{
				*result = *first + *second;
			}

			return signalFlow(signalExitSignal);
		}

	private:
		const tSignalExitId signalExitSignal = 1;

	private:
		std::string* first;
		std::string* second;
		std::string* result;
	};
};

}

}

#endif // EXAMPLE_H
