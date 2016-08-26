#include "Application2D.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <thread>

#include "SpriteBatch.h"
#include "Texture.h"
#include "Font.h"

Application2D::Application2D() {

}

Application2D::~Application2D() {

}

bool Application2D::InitAudio()
{
	FMOD_RESULT result;
	fmodSystem = NULL;

	result = FMOD::System_Create(&fmodSystem);

	srand(time(NULL));
	
	//Adds location of possible background music to list
	SongFiles.push_back("./bin/sounds/BKG/Maria.ogg");
	SongFiles.push_back("./bin/sounds/BKG/Souls.ogg");
	SongFiles.push_back("./bin/sounds/BKG/Cinder.ogg");

	//Initialisation of FMOD
	if (result != FMOD_OK)
	{
		printf("FMOD Error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	
	result = fmodSystem->init(512, FMOD_INIT_NORMAL, 0);

	if (result != FMOD_OK)
	{
		printf("FMOD Error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	//Creates stream using randomly picked background music
	result = fmodSystem->createStream(SongFiles.at(rand() % SongFiles.size()), FMOD_DEFAULT, 0, &pSound);
	if (result != FMOD_OK)
	{
		printf("FMOD Error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	
	//Creates a channel for the music and future loaded SFX, and sets it to half volume
	result = fmodSystem->createChannelGroup("LaChannelGroupe", &pChannelGroup);
	pChannelGroup->setVolume(.5f);
	pChannelGroup->setMode(FMOD_LOOP_NORMAL);
	if (result != FMOD_OK)
	{
		printf("FMOD Error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	
	//Plays the background music
	result = fmodSystem->playSound(pSound, pChannelGroup, false, &pChannel);
	if (result != FMOD_OK)
	{
		printf("FMOD Error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	return true;
}

//Alot of the code for the client is the same as the server, 
//minus the lack fo features the client doens't require
bool Application2D::startup() {
	
	Debug = true;

	client = nullptr;

	InitAudio();

	createWindow("A.I. Project", 1280, 720);

	//Loading Textures for Player 1 and 2
	m_spriteBatch = new SpriteBatch();
	std::vector<Texture*> tempVec;
	tempVec.push_back(new Texture("./bin/textures/Anims/Idle0.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Idle1.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Idle2.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Walk0.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Walk1.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Walk2.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Walk3.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack0.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack1.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack2.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack3.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Parry0.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Parry1.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Parry2.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack1 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack3 - Copy.png"));

	CollisionBoxes = std::vector<CollisionBox*>();

	Agent1 = new Agent(glm::vec2(100, 500), 100, 100, true, tempVec, CollisionBoxes, fmodSystem);

	tempVec.clear();

	tempVec.push_back(new Texture("./bin/textures/Anims/Idle0 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Idle1 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Idle2 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Walk0 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Walk1 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Walk2 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Walk3 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack0 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack1 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack2 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack3 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Parry0 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Parry1 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Parry2 - Copy.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack1.png"));
	tempVec.push_back(new Texture("./bin/textures/Anims/Attack3.png"));

	Agent2 = new Agent(glm::vec2(500, 500), 100, 100, false, tempVec, CollisionBoxes, fmodSystem);


	m_font = new Font("./bin/font/consolas.ttf", 32);
	f = new Font("./bin/font/consolas_italic.ttf", 32);
	
	return true;
}

void Application2D::shutdown() {

	//Disconnects the client from the server, and informs the server it has done so
	if (client != nullptr)
	{
		client->Disconnect();
		delete client;
	}

	delete m_font;
	delete m_spriteBatch;

	destroyWindow();
}

bool Application2D::update(float deltaTime) {

	Framerate = 1.0 / deltaTime;
	//If the client process has been initialised, begins listening for packets
	if (client != nullptr)
	{
		client->ReceivePackets();
	}
	//Sets artificial framerate to 30 if its outside these boundaries
	if (Framerate > 120 || Framerate <= 30)
	{
		Framerate = 30;
	}
	// close the application if the window closes or we press escape
	if (hasWindowClosed() || isKeyPressed(GLFW_KEY_ESCAPE))
		return false;

	//Pressing the Home key beings the process to connect to server, requests IP of server
	if (isKeyPressed(GLFW_KEY_HOME))
	{
		if (client != nullptr)
		{
			client->Disconnect();
			delete client;
		}
		std::cout << "Please enter IP address for game server:";
		std::getline(std::cin,newIP);
		char* temp = new char[newIP.length() + 1];
		std::strcpy(temp, newIP.c_str());
		client = new Client(temp, 5456, Agent1, Agent2);
		client->InitClient();
		//Waits for 1 second so it doesn't immediately try to 
		//reconnect if the player holds the home key for too long
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	//Begins the connection process, but to the last server connected to,
	//this is localhost at the start, running this while connected to server
	//will disconnect and immediately reconnect
	else if (isKeyPressed(GLFW_KEY_END))
	{
		if (client != nullptr)
		{
			client->Disconnect();
			delete client;
		}
		char* temp = new char[newIP.length() + 1];
		std::strcpy(temp, newIP.c_str());
		client = new Client(temp, 5456, Agent1, Agent2);
		client->InitClient();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	//If the client is connected and controls an agent, sends user input to the server for processing
	if (client != nullptr && !client->isSpectator)
	{
		//Informs the server it has released a key
		switch (CurrInput)
		{
		case Left:
			if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_RELEASE)
			{
				CurrInput == UserInputs::Idle;
				client->SendInput(UserInputs::Idle);
			}
			break;
		case Right:
			if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
			{
				CurrInput == UserInputs::Idle;
				client->SendInput(UserInputs::Idle);
			}
			break;
		case Attack1:
			if (glfwGetKey(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
			{
				CurrInput == UserInputs::Idle;
				client->SendInput(UserInputs::Idle);
			}
			break;
		case Attack2:
			if (glfwGetKey(m_window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
			{
				CurrInput == UserInputs::Idle;
				client->SendInput(UserInputs::Idle);
			}
			break;
		case Idle:
			break;
		default:
			break;
		}

		//Informs the server it has performed an action (attacking, moving, etc)
		if (isMouseButtonPressed(0))
		{
			CurrInput = UserInputs::Attack1;
			client->SendInput(CurrInput);
		}
		else if (isMouseButtonPressed(1))
		{
			CurrInput = UserInputs::Attack2;
			client->SendInput(CurrInput);
		}
		else if (isKeyPressed(GLFW_KEY_LEFT))
		{
			CurrInput = UserInputs::Left;
			client->SendInput(CurrInput);
		}
		else if (isKeyPressed(GLFW_KEY_RIGHT))
		{
			CurrInput = UserInputs::Right;
			client->SendInput(CurrInput);
		}
		else
		{
			//
		}
	}
	
	//Runs the agents update functions, in the event game 
	//data isn't recieved from the server, the game will continue to run smoothly
	Agent1->Update(Framerate / 30);
	Agent2->Update(Framerate / 30);
	fmodSystem->update();
	// the applciation closes if we return false
	return true;
}

void Application2D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_spriteBatch->begin();

	m_spriteBatch->setSpriteColor(1, 1, 1, 1);
	Agent1->Draw(m_spriteBatch);
	Agent2->Draw(m_spriteBatch);
	
	//Draws white background
	m_spriteBatch->setSpriteColor(1, 1, 1, 1);
	m_spriteBatch->drawSprite(nullptr, 1280, 720, 1280 * 2, 720 * 2, 0, 100);
	
	//Calculates and displays HP Bar
	float hpPercent = Agent1->m_iHealth * 0.01f;
	m_spriteBatch->setSpriteColor(1, 0.3f, 0.3f, 1);
	m_spriteBatch->drawLine(20, 700, 500 * hpPercent, 700, 10, 0);
	m_spriteBatch->setSpriteColor(0, 0, 0, 1);
	m_spriteBatch->drawLine(20, 700, 500, 700, 10, 1);
	
	//Likewise for player 2
	float a = (Agent2->m_iHealth * 0.01f);
	hpPercent = 1 - a;
	m_spriteBatch->setSpriteColor(1, 0.3f, 0.3f, 1);
	m_spriteBatch->drawLine(780 + (480 * hpPercent), 700, 1260, 700, 10, 0);
	m_spriteBatch->setSpriteColor(0, 0, 0, 1);
	m_spriteBatch->drawLine(780, 700, 1260, 700, 10, 1);

	//Calculates and displays stamina bar
	float stamPercent = (Agent1->m_iCurrStamina * 0.01f);
	m_spriteBatch->setSpriteColor(0.3, 1.f, 0.3f, 1);
	int MaxPos = 450 * stamPercent;
	//Stamina can go below zero, ensures bar doesn't draw outside boundaries
	if (MaxPos < 20)
	{
		MaxPos = 20;
	}
	m_spriteBatch->drawLine(20, 680, MaxPos, 680, 10, 0);
	m_spriteBatch->setSpriteColor(0, 0, 0, 1);
	m_spriteBatch->drawLine(20, 680, 450, 680, 10, 1);
	a = (Agent2->m_iCurrStamina * 0.01f);
	stamPercent = 1 - a;

	MaxPos = 830 + (430 * stamPercent);
	if (MaxPos > 1260)
	{
		MaxPos = 1260;
	}
	m_spriteBatch->setSpriteColor(0.3, 1.f, 0.3f, 1);
	m_spriteBatch->drawLine(830 + (430 * stamPercent), 680, 1260, 680, 10, 0);
	m_spriteBatch->setSpriteColor(0, 0, 0, 1);
	m_spriteBatch->drawLine(830, 680, 1260, 680, 10, 1);
	
	//Draws players score
	char score[3];
	itoa(Agent1->Score, score, 10);
	m_spriteBatch->drawText(m_font, score, 540, 680, 0);
	itoa(Agent2->Score, score, 10);
	m_spriteBatch->drawText(m_font, score, 720, 680, 0);
	
	//Draws text to the screen informing the player whether they are PLayer 1, 2 or spectating
	if (client != nullptr && client->IsConnected == true)
	{
		if (client->isSpectator)
		{
			m_spriteBatch->drawText(m_font, "You are spectating...", 200, 100);
		}
		else
		{
			if (client->isPlayer1)
			{
				m_spriteBatch->drawText(m_font, "You are Player 1", 200, 100);
			}
			else
			{
				m_spriteBatch->drawText(m_font, "You are Player 2", 200, 100);
			}
		}

		//Displays who has won the game
		if (Agent1->m_iHealth <= 0)
		{
			m_spriteBatch->drawText(m_font, "Player 2 Wins", 200, 300);
		}
		else if (Agent2->m_iHealth <= 0)
		{
			m_spriteBatch->drawText(m_font, "Player 1 Wins", 200, 300);
		}
	}
	//Draws information about how to connect to a server
	else
	{
		m_spriteBatch->drawText(f, "To connect to server, press 'Home' key", 200, 120);
		m_spriteBatch->drawText(f, "then input the IP address into the console", 200, 90);
		m_spriteBatch->drawText(f, "Press 'End' key to connect to", 200, 40);
		m_spriteBatch->drawText(f, "last server connected to (localhost by default)", 200, 10);
	}

	// done drawing sprites
	m_spriteBatch->end();	
}