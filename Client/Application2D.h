#pragma once

#include "BaseApplication.h"
#include "Agent.h"
#include "CollisionBox.h"
#include "Client.h"
#include "fmod.hpp"
#include "fmod_errors.h"


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
	std::string newIP = "127.0.0.1";
	Client* client = nullptr;
	Agent* Agent1, *Agent2;
	std::vector<CollisionBox*> CollisionBoxes;
	SpriteBatch*	m_spriteBatch;
	Texture*		m_texture;
	Font*			m_font;
	Font* f;

	FMOD::System* fmodSystem;
	FMOD::Sound* pSound;
	FMOD::Channel* pChannel;
	FMOD::ChannelGroup* pChannelGroup;

	std::vector<char*> SongFiles;

	bool useAudio;

	UserInputs CurrInput = UserInputs::Idle;
	int Framerate = 0;
	bool Debug = false;
	bool StartGame = false;
	double frameRateCounter = 0;
};