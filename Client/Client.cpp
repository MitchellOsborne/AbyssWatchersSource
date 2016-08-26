#include "Client.h"
#include <iostream>
#include "GameMessages.h"



Client::Client(const char* a_IP, unsigned int a_PORT, Agent* a1, Agent* a2)
{
	IP = a_IP;
	PORT = a_PORT;
	Agent1 = a1;
	Agent2 = a2;
}

void Client::InitClient()
{
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	InitConnection();
}

//Initialises Client Connection
void Client::InitConnection()
{
	RakNet::SocketDescriptor sd;

	m_pPeerInterface->Startup(1, &sd, 1);

	std::cout << "Connecting to Server at: " << IP << std::endl;

	RakNet::ConnectionAttemptResult res = m_pPeerInterface->Connect(IP, PORT, nullptr, 0);

	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection, Error: " << res << std::endl;
	}
}

//Informs the server it is disconnected
void Client::Disconnect()
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)ID_DISCONNECTION_NOTIFICATION);
	m_pPeerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	
}

//Listens for packets from the Server, this is run in a seperate thread to the main game
void Client::ReceivePackets()
{
	for (packet = m_pPeerInterface->Receive(); packet; m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive())
	{
		RakNet::BitStream bsIn(packet->data, packet->length, false);
		RakNet::RakString str;
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			std::cout << "Another Client Disconnected" << std::endl;
			break;
		case ID_REMOTE_CONNECTION_LOST:
			std::cout << "Another Client Lost Connection" << std::endl;
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			std::cout << "Another Client Connected" << std::endl;
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Our connection request was accepted" << std::endl;
			IsConnected = true;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			std::cout << "Server Full" << std::endl;
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "We have been disconnected" << std::endl;
			break;
		case ID_CONNECTION_LOST:
			std::cout << "Connection to server lost" << std::endl;
			break;
		case GameMessages::ID_SERVER_TEXT_MESSAGE:
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(str);
			std::cout << str.C_String() << std::endl;
			break;
			
		//Recieves its unique CLient ID and the type of connection it has to the server, Player 1, 2 or Spectator
		case GameMessages::ID_SERVER_CLIENT_ID:
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(m_uiClientId);
			int type;
			bsIn.Read(type);

			switch (type)
			{
			case 0:
				isSpectator = true;
				break;
			case 1:
				isSpectator = false;
				isPlayer1 = true;
				break;
			case 2:
				isSpectator = false;
				isPlayer1 = false;
				break;
			}
			std::cout << "Client ID recieved: " << m_uiClientId << std::endl;
			break;
			
		//Recieves all necessary GameData from Server and updates game data to match
		case GameMessages::ID_SERVER_GAME_DATA:
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(Agent1->m_oPos);
			bsIn.Read(Agent1->m_oAgentState);
			bsIn.Read(Agent1->m_iStateIndex);
			bsIn.Read(Agent1->m_iAnimIndex);
			bsIn.Read(Agent1->m_iHealth);
			bsIn.Read(Agent1->m_iCurrStamina);
			bsIn.Read(Agent2->m_oPos);
			bsIn.Read(Agent2->m_oAgentState);
			bsIn.Read(Agent2->m_iStateIndex);
			bsIn.Read(Agent2->m_iAnimIndex);
			bsIn.Read(Agent2->m_iHealth);
			bsIn.Read(Agent2->m_iCurrStamina);
			bsIn.Read(Agent1->Score);
			bsIn.Read(Agent2->Score);
			break;
		case GameMessages::ID_SERVER_ASSIGNED_SPECTATOR:
			isSpectator = true;
			break;
		case GameMessages::ID_SERVER_ASSIGNED_PLAYER:
			isSpectator = false;
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(isPlayer1);
			break;
		default:
			std::cout << "Unknown message type received:" << packet->data[0] << std::endl;
			if (!IsConnected)
			{
				std::cout << "Unable to Connect to Server, Server not found?" << std::endl;
			}
			break;
		}
	}

}

//Sends the current user intpu to the server for processing
void Client::SendInput(UserInputs a_input)
{
		//std::cout << "Attempting to send input: " << a_input << std::endl;
		RakNet::BitStream bs;
		bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_INPUT_RECEIVED);
		bs.Write(a_input);

		m_pPeerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		//std::cout << "Input has been sent: " << a_input << std::endl;
}