// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_LIBRARY_HTTPSERVER_H
#define TVM_LIBRARY_HTTPSERVER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <tvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cHttpServer : public cLibrary
{
public:
	using tIpAddress = uint32_t;
	using tBoolean = bool;
	using tString = std::string;

public:
	cHttpServer(const std::string& ipAddress,
	            const uint16_t port) :
	        ipAddress(ipAddress),
	        port(port)
	{
		serverSocket = -1;
		clientSocket = -1;
	}

	bool registerLibrary() override
	{
		setLibraryName("httpServer");

		if (!registerMemory<tIpAddress>("ipAddress",
		                                0))
		{
			return false;
		}

		if (!registerMemoryModule("ipAddress",
		                          new cLogicConvert<tIpAddress,
		                                            tString>("toString",
		                                                     "ipAddress",
		                                                     "string",
			[](tIpAddress* from, tString* to)
			{
				char ipAddressStr[16];
				snprintf(ipAddressStr, 16, "%u.%u.%u.%u",
				         ((*from) & 0xFF),
				         (((*from) >> 8) & 0xFF),
				         (((*from) >> 16) & 0xFF),
				         (((*from) >> 24) & 0xFF));
				*to = ipAddressStr;
			})))
		{
			return false;
		}

		if (!registerRootModules(rootGet,
		                         rootPost))
		{
			return false;
		}

		if (!registerModules(new cLogicOk(this),
		                     new cLogicNotFound(this)))
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
				std::map<tString, tString> arguments;

				tString host = request.substr(request.find("Host: ") + 6);
				host = host.substr(0, host.find("\r\n"));
				tString url = request.substr(4);
				url = url.substr(0, url.find("\r\n") - 9);

				tString urlArguments = url;
				if (url.find('?') != tString::npos)
				{
					urlArguments = urlArguments.substr(url.find('?') + 1);
					while (urlArguments.length())
					{
						tString argument;

						if (urlArguments.find('&') != tString::npos)
						{
							argument = urlArguments.substr(0, urlArguments.find('&'));
							urlArguments = urlArguments.substr(urlArguments.find('&') + 1);
						}
						else
						{
							argument = urlArguments;
							urlArguments.clear();
						}

						if (argument.find('=') != tString::npos)
						{
							const tString argumentName = argument.substr(0, argument.find('='));
							const tString argumentValue = argument.substr(argument.find('=') + 1);

							arguments[argumentName] = argumentValue;
						}
					}

					url = url.substr(0, url.find('?'));
				}

				rootSetMemory(rootGet.memoryFromIpAddress, address.sin_addr.s_addr);
				rootSetMemory(rootGet.memoryHost, host);
				rootSetMemory(rootGet.memoryUrl, url);
				rootSetMemory(rootGet.memoryArguments, arguments);
				rootSignalFlow(rootGet.signal);
			}
			else if (request.substr(0, 4) == "POST")
			{
				/** @todo */
			}

			close(clientSocket);
			clientSocket = -1;
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
	const std::string ipAddress;
	const uint16_t port;
	int serverSocket;
	int clientSocket;

private: /** rootModules */
	class cRootGet : public cRootModule
	{
	public:
		bool registerModule() override
		{
			setModuleName("get");

			if (!registerSignalExit("signal", signal))
			{
				return false;
			}

			if (!registerMemoryExit("fromIpAddress", "ipAddress", memoryFromIpAddress))
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

			if (!registerMemoryExit("arguments", "map<string,string>", memoryArguments))
			{
				return false;
			}

			return true;
		}

		tRootSignalExitId signal;
		tRootMemoryExitId memoryFromIpAddress;
		tRootMemoryExitId memoryHost;
		tRootMemoryExitId memoryUrl;
		tRootMemoryExitId memoryArguments;
	};

	class cRootPost : public cRootModule
	{
	public:
		bool registerModule() override
		{
			setModuleName("post");

			if (!registerSignalExit("signal", signal))
			{
				return false;
			}

			/** @todo */

			return true;
		}

		tRootSignalExitId signal;
	};

private:
	cRootGet rootGet;
	cRootPost rootPost;

private: /** modules */
	class cLogicOk : public cLogicModule
	{
	public:
		cLogicOk(cHttpServer* library) :
		        library(library)
		{
		}

		cModule* clone() const override
		{
			return new cLogicOk(library);
		}

		bool registerModule() override
		{
			setModuleName("ok");

			if (!registerSignalEntry("signal", &cLogicOk::signalEntry))
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
		bool signalEntry()
		{
			std::string response;
			response = "HTTP/1.1 200 OK\r\nServer: tvm/library/httpserver\r\n\r\n";

			response += "<HTML>";
			response += "<HEAD><TITLE>";
			if (title)
			{
				response += *title;
			}
			response += "</TITLE></HEAD>";

			response += "<BODY>";
			if (body)
			{
				response += *body;
			}
			response += "</BODY>";
			response += "</HTML>";

			send(library->clientSocket, response.c_str(), response.length(), MSG_NOSIGNAL);

			return true;
		}

	private:
		cHttpServer* library;

	private:
		tString* title;
		tString* body;
	};

	class cLogicNotFound : public cLogicModule
	{
	public:
		cLogicNotFound(cHttpServer* library) :
		        library(library)
		{
		}

		cModule* clone() const override
		{
			return new cLogicNotFound(library);
		}

		bool registerModule() override
		{
			setModuleName("notFound");

			if (!registerSignalEntry("signal", &cLogicNotFound::signalEntry))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			std::string response;
			response = "HTTP/1.1 404 Not Found\r\nServer: tvm/library/httpserver\r\n\r\n";

			response += "<HTML>";
			response += "<HEAD><TITLE>404 Not Found</TITLE></HEAD>";
			response += "<BODY><CENTER><H1>404 Not Found</H1></CENTER></BODY>";
			response += "</HTML>";

			send(library->clientSocket, response.c_str(), response.length(), MSG_NOSIGNAL);

			return true;
		}

	private:
		cHttpServer* library;
	};
};

}

}

#endif // TVM_LIBRARY_HTTPSERVER_H
