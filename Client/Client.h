#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "UserInputs.h"
#include "Agent.h"

namespace RakNet {
	class RakPeerInterface;
}

class Client
{
public:
	Client() {};
	Client(const char* a_IP, unsigned int a_PORT, Agent* a1, Agent* a2);
	void InitClient();
	void ReceivePackets();
	void SendInput(UserInputs a_input);
	void Disconnect();
	bool isSpectator = false;
	bool isPlayer1 = false;
	bool IsConnected = false;
private:
	Agent* Agent1, *Agent2;
	void InitConnection();
	RakNet::Packet* packet = nullptr;
	RakNet::RakPeerInterface* m_pPeerInterface;
	const char* IP = "127.0.0.1";
	unsigned short PORT = 5456;
	unsigned int m_uiClientId;
};
