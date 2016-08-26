#pragma once

#include "BaseApplication.h"
#include "Agent.h"
#include "CollisionBox.h"
#include "Server.h"
#include"fmod.hpp"

class SpriteBatch;
class Texture;
class Font;

class Application2D : public BaseApplication {
public:

	Application2D();
	virtual ~Application2D();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

	bool InitAudio();

protected:
	Agent* Agent1, *Agent2;
	Server* server;
	std::vector<CollisionBox*> CollisionBoxes;
	SpriteBatch*	m_spriteBatch;
	Texture*		m_texture;
	Font*			m_font;
	FMOD::System* fmodSystem;
	FMOD::Sound* pSound;
	FMOD::Channel* pChannel;
	FMOD::ChannelGroup* pChannelGroup;

	std::vector<char*> SongFiles;


	bool useAudio;
	int Framerate = 0;
	bool Debug = false;
	double frameRateCounter = 0;
	bool GameOver = false;
	float RestartTimer = 0;
	int RestartTime = 3;
	int P1Score = 0, P2Score = 0;
};