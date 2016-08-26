#pragma once
#include <iostream>
#include <string>

#include <thread>
#include <chrono>
#include <unordered_map>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "GameMessages.h"
#include "UserInputs.h"
#include "Agent.h"

class Server {
public:

	Server(Agent* a1, Agent* a2);
	Server() {};
	~Server();

	void run();

	void handleNetworkMessages();

	void BroadcastGameData();
protected:

	unsigned int systemAddressToClientID(RakNet::SystemAddress& systemAddress);

	// connection functions
	void addNewConnection(RakNet::SystemAddress systemAddress);
	void removeConnection(RakNet::SystemAddress systemAddress);

	void sendClientIDToClient(unsigned int uiClientID, int type);

private:

	struct ConnectionInfo {
		unsigned int			uiConnectionID;
		RakNet::SystemAddress	sysAddress;
	};

	const unsigned short PORT = 5456;

	RakNet::RakPeerInterface*							m_pPeerInterface;

	unsigned int										m_uiConnectionCounter;
	std::unordered_map<unsigned int, ConnectionInfo>	m_connectedClients;
	Agent *Agent1, *Agent2;
	
	unsigned int										m_uiObjectCounter;
};