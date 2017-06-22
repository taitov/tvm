// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_LIBRARY_RAWSOCKET_H
#define TFVM_LIBRARY_RAWSOCKET_H

#include <vector>
#include <map>
#include <algorithm>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <endian.h>

#include <tfvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cRawSocket : public cLibrary
{
	/** @todo: import cBase */

public:
	using tBoolean = bool;
	using tString = std::string;
	using tInteger = int64_t;
	using tPortId = int64_t;
	using tBuffer = std::vector<uint8_t>;
	using tEthernetType = uint16_t;
	using tEthernetAddress = std::array<uint8_t, 6>;

public:
	cRawSocket()
	{
	}

	cRawSocket(const std::vector<std::string>& exceptInterfaces) :
	        exceptInterfaces(exceptInterfaces)
	{
	}

	bool registerLibrary() override
	{
		setLibraryName("rawSocket");

		if (!registerMemory<tPortId>("portId", 0))
		{
			return false;
		}

		if (!registerMemoryArray<uint8_t,
		                         6,
		                         tInteger,
		                         tBoolean>("ethernetAddress",
		                                   "byte",
		                                   "integer",
		                                   "boolean"))
		{
			return false;
		}

		if (!registerMemoryStandart<tEthernetType,
		                            tBoolean>("ethernetType",
		                                      "boolean",
		                                      0))
		{
			return false;
		}

		if (!registerMemoryVector<tPortId,
		                          tInteger,
		                          tBoolean>("portId",
		                                    "integer",
		                                    "boolean"))
		{
			return false;
		}

		if (!registerMemoryMap<tPortId,
		                       tString,
		                       tInteger,
		                       tBoolean>("portId",
		                                 "string",
		                                 "integer",
		                                 "boolean"))
		{
			return false;
		}

		if (!registerMemoryModule("ethernetAddress",
		                          new cLogicConvert<tEthernetAddress,
		                                            tString>("toString",
		                                                     "ethernetAddress",
		                                                     "string",
			[](tEthernetAddress* from, tString* to)
			{
				char ethernetAddressStr[18];
				snprintf(ethernetAddressStr, 18, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
				         (*from)[0],
				         (*from)[1],
				         (*from)[2],
				         (*from)[3],
				         (*from)[4],
				         (*from)[5]);
				*to = ethernetAddressStr;
			})))
		{
			return false;
		}

		if (!registerMemoryTuple<tBoolean,
		                         tBoolean,
		                         tString>("boolean",
		                                  {"state", "name"},
		                                  {"boolean", "string"}))
		{
			return false;
		}

		if (!registerMemoryMap<tPortId,
		                       std::tuple<tBoolean,
		                                  tString>,
		                       tInteger,
		                       tBoolean>("portId",
		                                 "tuple<state,name>",
		                                 "integer",
		                                 "boolean"))
		{
			return false;
		}

		if (!registerRootModules(rootRecvPacket))
		{
			return false;
		}

		if (!registerModules(new cLogicSendPacket(this),
		                     new cLogicGetEthernetHeader(),
		                     new cLogicSendPacketBroadcast(this),
		                     new cLogicGetInterfaces(this)))
		{
			return false;
		}

		return true;
	}

	bool init() override
	{
		struct ifaddrs* networkInterfaces;
		struct ifaddrs* networkInterfaceNext;

		if (getifaddrs(&networkInterfaces))
		{
			return false;
		}

		networkInterfaceNext = networkInterfaces;
		while (networkInterfaceNext)
		{
			struct ifaddrs* networkInterface = networkInterfaceNext;
			networkInterfaceNext = networkInterfaceNext->ifa_next;

			if (networkInterface->ifa_flags & IFF_LOOPBACK)
			{
				continue;
			}

			if (networkInterface->ifa_addr->sa_family != AF_PACKET)
			{
				continue;
			}

			if (!checkExceptInterfaces(networkInterface->ifa_name))
			{
				continue;
			}

			int rawSocket = createSocket(networkInterface->ifa_name);
			if (rawSocket < 0)
			{
				return false;
			}

			interfaceNames.push_back(networkInterface->ifa_name);
			interfaces.push_back(rawSocket);
		}

		freeifaddrs(networkInterfaces);
		return true;
	}

	void run() override
	{
		std::vector<char> buffer;
		constexpr size_t bufferSize = 16384;

		while (!isStopped())
		{
			for (tPortId portId = 0; portId < (unsigned int)interfaces.size(); portId++)
			{
				int rawSocket = interfaces[portId];

				buffer.resize(bufferSize);

				const int recvLen = recv(rawSocket,
				                         &buffer[0],
				                         bufferSize,
				                         0);
				if (recvLen < 0)
				{
					const int errorNumber = errno;
					if (errorNumber == EAGAIN ||
					    errorNumber == EINTR ||
					    errorNumber == ENETDOWN)
					{
						continue;
					}

					return;
				}

				buffer.resize(recvLen);

				rootSetMemory(rootRecvPacket.memoryPortId, portId);
				rootSetMemory(rootRecvPacket.memoryPacket, buffer);
				rootSignalFlow(rootRecvPacket.signal);
			}

			sleep(0);
		}
	}

private:
	int createSocket(const std::string& interfaceName)
	{
		int rawSocket = -1;

		rawSocket = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(ETH_P_ALL));
		if (rawSocket < 0)
		{
			return -1;
		}

		int ifindex;

		{
			struct ifreq ifr;
			strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ);
			if (ioctl(rawSocket, SIOCGIFINDEX, &ifr) < 0)
			{
				close(rawSocket);
				return -1;
			}
			ifindex = ifr.ifr_ifindex;
		}

		{
			struct ifreq ifr;
			strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ);
			if (ioctl(rawSocket, SIOCGIFFLAGS, &ifr) < 0)
			{
				close(rawSocket);
				return -1;
			}

			ifr.ifr_flags |= IFF_PROMISC | IFF_UP;
			if (ioctl(rawSocket, SIOCSIFFLAGS, &ifr) < 0)
			{
				close(rawSocket);
				return -1;
			}
		}

		{
			sockaddr_ll sll;
			memset(&sll, 0, sizeof(sll));
			sll.sll_family = AF_PACKET;
			sll.sll_protocol = htons(ETH_P_ALL);
			sll.sll_ifindex = ifindex;

			if (bind(rawSocket, (sockaddr*)&sll, sizeof(sll)) < 0)
			{
				close(rawSocket);
				return -1;
			}
		}

		return rawSocket;
	}

	bool checkExceptInterfaces(const std::string& interface)
	{
		for (const auto& exceptInterface : exceptInterfaces)
		{
			if (interface == exceptInterface)
			{
				return false;
			}
		}
		return true;
	}

private:
	std::vector<tString> exceptInterfaces;
	std::vector<tString> interfaceNames;

	std::vector<int> interfaces;

private: /** rootModules */
	class cRootRecvPacket : public cRootModule
	{
	public:
		bool registerModule() override
		{
			setModuleName("recvPacket");

			if (!registerSignalExit("signal", signal))
			{
				return false;
			}

			if (!registerMemoryExit("portId", "portId", memoryPortId))
			{
				return false;
			}

			if (!registerMemoryExit("packetData", "buffer", memoryPacket))
			{
				return false;
			}

			return true;
		}

		tRootSignalExitId signal;
		tRootMemoryExitId memoryPortId;
		tRootMemoryExitId memoryPacket;
	};

private:
	cRootRecvPacket rootRecvPacket;

private: /** modules */
	class cLogicGetInterfaces : public cLogicModule
	{
	public:
		using tMap = std::map<tPortId,
		                      std::tuple<tBoolean,
		                                 tString>>;

	public:
		cLogicGetInterfaces(cRawSocket* library) :
		        library(library)
		{
		}

		cModule* clone() const override
		{
			return new cLogicGetInterfaces(library);
		}

		bool registerModule() override
		{
			setModuleName("getInterfaces");

			if (!registerSignalEntry("signal", &cLogicGetInterfaces::signalEntry))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			if (!registerMemoryExit("map<portId,tuple<state,name>>", "map<portId,tuple<state,name>>", map))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (map)
			{
				for (tPortId portId = 0; portId < (int64_t)library->interfaces.size(); portId++)
				{
					struct ifreq ifr;
					strncpy(ifr.ifr_name, library->interfaceNames[portId].c_str(), IFNAMSIZ);
					if (ioctl(library->interfaces[portId], SIOCGIFFLAGS, &ifr) < 0)
					{
						std::get<0>((*map)[portId]) = false;
						continue;
					}

					std::get<0>((*map)[portId]) = ifr.ifr_flags & IFF_UP;
				}

				for (tPortId portId = 0; portId < (int64_t)library->interfaceNames.size(); portId++)
				{
					std::get<1>((*map)[portId]) = library->interfaceNames[portId];
				}
			}
			return signalFlow(signalExit);
		}

	private:
		const tSignalExitId signalExit = 1;

	private:
		cRawSocket* library;

	private:
		tMap* map;
	};

	class cLogicSendPacket : public cLogicModule
	{
	public:
		cLogicSendPacket(cRawSocket* library) :
		        library(library)
		{
		}

		cModule* clone() const override
		{
			return new cLogicSendPacket(library);
		}

		bool registerModule() override
		{
			setModuleName("sendPacket");

			if (!registerSignalEntry("signal", &cLogicSendPacket::signalEntry))
			{
				return false;
			}

			if (!registerMemoryEntry("portId", "portId", portId))
			{
				return false;
			}

			if (!registerMemoryEntry("packetData", "buffer", packet))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if ((!portId) || (!packet))
			{
				return true;
			}

			if (*portId >= (unsigned int)library->interfaces.size())
			{
				return true;
			}

			int rawSocket = library->interfaces[*portId];
			const int sendLen = send(rawSocket, &(*packet)[0], packet->size(), 0);
			if (sendLen < 0)
			{
				/** @todo */
			}

			return true;
		}

	private:
		cRawSocket* library;

	private:
		tPortId* portId;
		tBuffer* packet;
	};

	class cLogicSendPacketBroadcast : public cLogicModule
	{
	public:
		cLogicSendPacketBroadcast(cRawSocket* library) :
		        library(library)
		{
		}

		cModule* clone() const override
		{
			return new cLogicSendPacketBroadcast(library);
		}

		bool registerModule() override
		{
			setModuleName("sendPacketBroadcast");

			if (!registerSignalEntry("signal", &cLogicSendPacketBroadcast::signalEntry))
			{
				return false;
			}

			if (!registerMemoryEntry("packetData", "buffer", packet))
			{
				return false;
			}

			if (!registerMemoryEntry("exceptPortId", "portId", exceptPortId))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (!packet)
			{
				return true;
			}

			for (tPortId portId = 0; portId < (unsigned int)library->interfaces.size(); portId++)
			{
				if (exceptPortId && portId == *exceptPortId)
				{
					continue;
				}

				int rawSocket = library->interfaces[portId];
				const int sendLen = send(rawSocket, &(*packet)[0], packet->size(), 0);
				if (sendLen < 0)
				{
					/** @todo */
				}
			}

			return true;
		}

	private:
		cRawSocket* library;

	private:
		tBuffer* packet;
		tPortId* exceptPortId;
	};

	class cLogicGetEthernetHeader : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicGetEthernetHeader();
		}

		bool registerModule() override
		{
			setModuleName("getEthernetHeader");

			if (!registerSignalEntry("signal", &cLogicGetEthernetHeader::signalEntry))
			{
				return false;
			}

			if (!registerMemoryEntry("packetData", "buffer", packet))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			if (!registerMemoryExit("destination", "ethernetAddress", destination))
			{
				return false;
			}

			if (!registerMemoryExit("source", "ethernetAddress", source))
			{
				return false;
			}

			if (!registerMemoryExit("type", "ethernetType", ethernetType))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (packet)
			{
				if (destination)
				{
					memcpy(destination, &(*packet)[0], 6);
				}

				if (source)
				{
					memcpy(source, &(*packet)[6], 6);
				}

				if (ethernetType)
				{
					uint16_t value;
					memcpy(&value, &(*packet)[12], sizeof(value));
					*ethernetType = be16toh(value);
				}
			}
			return signalFlow(signalExit);
		}

	private:
		const tSignalExitId signalExit = 1;

	private:
		tBuffer* packet;
		tEthernetAddress* destination;
		tEthernetAddress* source;
		tEthernetType* ethernetType;
	};
};

}

}

#endif // TFVM_LIBRARY_RAWSOCKET_H
