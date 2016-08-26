#include "Server.h"
#include <UPnP.h>

Server::Server(Agent* a1, Agent* a2) {
	// initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	//m_pPeerInterface->ApplyNetworkSimulator(0.5f, 150, 50);
	m_uiConnectionCounter = 1;
	m_uiObjectCounter = 1;
	Agent1 = a1;
	Agent2 = a2;

}

Server::~Server() {

}

void Server::run() {
	// startup the server, and start it listening to clients
	std::cout << "Starting up the server..." << std::endl;
	
	// create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);

	// now call startup - max of 32 connections, on the assigned port
	m_pPeerInterface->Startup(32, &sd, 1);
	m_pPeerInterface->SetMaximumIncomingConnections(32);

	//handleNetworkMessages();
}

void Server::handleNetworkMessages() {
	RakNet::Packet* packet = nullptr;
	int senderId = -1;
	while (true) {
		for (packet = m_pPeerInterface->Receive();
		packet;
			m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive()) {
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			switch (packet->data[0]) {
			case ID_NEW_INCOMING_CONNECTION: {
				addNewConnection(packet->systemAddress);
				std::cout << "A connection is incoming.\n";
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected.\n";
				removeConnection(packet->systemAddress);
				break;
			case ID_CONNECTION_LOST:
				std::cout << "A client lost the connection.\n";
				removeConnection(packet->systemAddress);
				break;
			case GameMessages::ID_CLIENT_INPUT_RECEIVED:
				senderId = systemAddressToClientID(packet->systemAddress);
				//std::cout << "Input Received from user: " << senderId << std::endl;
				UserInputs input;
				if (Agent1->m_uiClientID == senderId)
				{
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(input);
					//printf("Client %i sent input %i\n", senderId, input);
					switch (input)
					{
					case Left:
						Agent1->ChangeState(AgentState::BACK);
						break;
					case Right:
						Agent1->ChangeState(AgentState::FORWARD);
						break;
					case Attack1:
						Agent1->ChangeState(AgentState::ATTACK1);
						break;
					case Attack2:
						Agent1->ChangeState(AgentState::ATTACK2);
						break;
					case Idle:
						Agent1->ChangeState(AgentState::IDLE);
						break;
					default:
						break;
					}
				}
				if (Agent2->m_uiClientID == senderId)
				{
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(input);
					//printf("Client %i sent input %i\n", senderId, input);
					switch (input)
					{
					case Left:
						Agent2->ChangeState(AgentState::FORWARD);
						break;
					case Right:
						Agent2->ChangeState(AgentState::BACK);
						break;
					case Attack1:
						Agent2->ChangeState(AgentState::ATTACK1);
						break;
					case Attack2:
						Agent2->ChangeState(AgentState::ATTACK2);
						break;
					case Idle:
						Agent2->ChangeState(AgentState::IDLE);
						break;
					default:
						break;
					}
				}
				break;
			default:
				std::cout << "Received a message with a unknown id: " << packet->data[0];
				break;
			}
		}
		//std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void Server::addNewConnection(RakNet::SystemAddress systemAddress) {
	ConnectionInfo info;
	info.sysAddress = systemAddress;
	info.uiConnectionID = m_uiConnectionCounter++;
	m_connectedClients[info.uiConnectionID] = info;
	int type = -1;
	if (Agent1->m_uiClientID == -1)
	{
		Agent1->m_uiClientID = info.uiConnectionID;
		type = 1;
		Agent1->Score = 0;
		Agent2->Score = 0;
	}
	else if (Agent2->m_uiClientID == -1)
	{
		Agent2->m_uiClientID = info.uiConnectionID;
		type = 2;
		Agent1->Score = 0;
		Agent2->Score = 0;
	}
	else
	{
		type = 0;
	}
	
	sendClientIDToClient(info.uiConnectionID, type);

}

void Server::removeConnection(RakNet::SystemAddress systemAddress) {
	int clientId = systemAddressToClientID(systemAddress);
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++) {
		if (it->second.sysAddress == systemAddress) {
			m_connectedClients.erase(it);
			if (Agent1->m_uiClientID == clientId)
			{
				Agent1->m_uiClientID = -1;
			}
			else if (Agent2->m_uiClientID == clientId)
			{
				Agent2->m_uiClientID = -1;
			}
			break;
		}
	}
}

unsigned int Server::systemAddressToClientID(RakNet::SystemAddress& systemAddress) {
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++) {
		if (it->second.sysAddress == systemAddress) {
			return it->first;
		}
	}

	return 0;
}

void Server::sendClientIDToClient(unsigned int uiClientID, int type) {
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_CLIENT_ID);
	bs.Write(uiClientID);
	bs.Write(type);

	m_pPeerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_connectedClients[uiClientID].sysAddress, false);
	
}

void Server::BroadcastGameData()
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_GAME_DATA);
	bs.Write(Agent1->m_oPos);
	bs.Write(Agent1->m_oAgentState);
	bs.Write(Agent1->m_iStateIndex);
	bs.Write(Agent1->m_iAnimIndex);
	bs.Write(Agent1->m_iHealth);
	bs.Write(Agent1->m_iCurrStamina);
	bs.Write(Agent2->m_oPos);
	bs.Write(Agent2->m_oAgentState);
	bs.Write(Agent2->m_iStateIndex);
	bs.Write(Agent2->m_iAnimIndex);
	bs.Write(Agent2->m_iHealth);
	bs.Write(Agent2->m_iCurrStamina);
	bs.Write(Agent1->Score);
	bs.Write(Agent2->Score);



	m_pPeerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

}
