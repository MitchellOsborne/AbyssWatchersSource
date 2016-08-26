#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

#include "SpriteBatch.h"
#include "Texture.h"
#include "CollisionBox.h"
#include "fmod.hpp"
enum AgentState
{
	IDLE,
	FORWARD,
	BACK,
	ATTACK1,
	ATTACK2,
	HURT,
	STUNNED,
	DEAD,
};

class CollisionBox;

const glm::vec2 TexSize = glm::vec2(288, 128);

class Agent
{
public:
	Agent() {};
	Agent(glm::vec2 a_oInitPos, int a_iHealth, int a_iStamina, bool a_bIsLeft, std::vector<Texture*> a_oTextures, std::vector<CollisionBox*>&a_oCBoxes, FMOD::System* a_FMod);
	void Update(float timeScale);
	void Draw(SpriteBatch* sb);
	glm::vec2 m_oPos;
	AgentState m_oAgentState;
	std::vector<Texture*> m_oAnimStates;
	int m_iHealth;
	int m_iMaxStamina;
	int m_iCurrStamina;
	int m_iDir = 1;
	int m_iAnimIndex = 0;
	int m_iStateIndex = 0;
	int Score;

	FMOD::System* m_FMod;
private:

	FMOD::Sound* m_Sound;
	FMOD::Channel* m_Channel;

	std::vector<char*> HurtSFX;
	std::vector<char*> HitSFX;
	std::vector<char*> SwingSFX;
	char* ParrySFX;
	char* DieSFX;

	bool hasHurt = false;
	bool hasStunned = false;
	bool hasHit = false;
	bool hasParry = false;
	bool hasSwing = false;
	bool hasDied = false;
};