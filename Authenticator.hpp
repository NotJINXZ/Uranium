#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <string>
#include <iostream>

#include "skCryptor.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#pragma comment(lib, "Ws2_32.lib")

namespace Authenticator
{
	SOCKET WinSocket;

	static bool Authenticate(std::string Token)
	{
		sockaddr_in RemoteSock;
		WSAData Data;
		WSAStartup(MAKEWORD(2, 2), &Data);

		RemoteSock.sin_family = AF_INET;
		RemoteSock.sin_port = htons(5812);
		RemoteSock.sin_addr.s_addr = inet_addr(crypt(""));

		WinSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		connect(WinSocket, (SOCKADDR*)(&RemoteSock), sizeof(RemoteSock));

		HW_PROFILE_INFO hwProfileInfo;
		GetCurrentHwProfile(&hwProfileInfo);

		auto HWID = std::wstring(hwProfileInfo.szHwProfileGuid);

		json j;
		j[std::string(crypt("Token"))] = Token;
		j[std::string(crypt("PCDiscriminator"))] = std::string(HWID.begin(), HWID.end());

		auto JData = j.dump().c_str();

		send(WinSocket, JData, strlen(JData), 0);

		char Packet[4];
		recv(WinSocket, Packet, 4, 0);

		if (std::string(Packet).find(crypt("True")) != std::string::npos)
		{
			return true;
		}

		return false;
	}
}
