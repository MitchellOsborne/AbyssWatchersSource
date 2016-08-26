#include "Agent.h"

Agent::Agent(glm::vec2 a_oInitPos, int a_iHealth, int a_iStamina, bool a_bIsLeft, std::vector<Texture*> a_oTextures, std::vector<CollisionBox*>&a_oCBoxes, FMOD::System* a_FMod)
{
	m_oAgentState = AgentState::IDLE;
	m_oPos = a_oInitPos;
	m_iHealth = a_iHealth;
	m_iMaxStamina = a_iStamina;
	m_iCurrStamina = a_iStamina;
	Score = 0;

	if (!a_bIsLeft)
	{
		m_iDir = -1;
	}
	
	for each(Texture* tex in a_oTextures)
	{
		m_oAnimStates.push_back(tex);
	}

	//Gets pointer to FMOD object in main Application
	m_FMod = a_FMod;
	
	//Gets location of necessary SFX files
	ParrySFX = "./bin/sounds/SFX/Parry.ogg";
	DieSFX = "./bin/sounds/SFX/Die.ogg";

	HurtSFX.push_back("./bin/sounds/SFX/Hurt1.ogg");
	HurtSFX.push_back("./bin/sounds/SFX/Hurt2.ogg");

	HitSFX.push_back("./bin/sounds/SFX/Hit1.ogg");
	HitSFX.push_back("./bin/sounds/SFX/Hit2.ogg");
	HitSFX.push_back("./bin/sounds/SFX/Hit3.ogg");

	SwingSFX.push_back("./bin/sounds/SFX/Swing1.ogg");
	SwingSFX.push_back("./bin/sounds/SFX/Swing2.ogg");
	srand(time(NULL));
}

/*	
	The Client Agent's Update function is much smaller than the servers,
	as the server does alot of the processing, and this information is passed to the client
	Only the playing of Audio is handled by the client
*/
void Agent::Update(float timeScale )
{
	
	printf("StateIndex: %i AnimationIndex %i \n", m_iStateIndex, m_iAnimIndex);
	
	//Determines whether a SFX has been played, and reenables playing it if it hasn't
	if (m_oAgentState != STUNNED)
	{
		hasStunned = false;
	}
	if (m_oAgentState != ATTACK1)
	{
		hasSwing = false;
		hasHit = false;
	}
	if (m_oAgentState != ATTACK2)
	{
		hasParry = false;
	}
	if (m_oAgentState != HURT)
	{
		hasHurt = false;
	}
	if (m_oAgentState != DEAD)
	{
		hasDied = false;
	}
	
	//The majority of the Agent States are handled by the server and 
	//animation data is passed to the client to draw, etc.
	switch (m_oAgentState)
	{
	case IDLE:
		if (m_iStateIndex < 4 * timeScale)
		{
			//m_iAnimIndex = 0;
		}
		else if (m_iStateIndex < 8 * timeScale)
		{
			//m_iAnimIndex = 1;
		}
		else if (m_iStateIndex < 12 * timeScale)
		{
			//m_iAnimIndex = 2;
		}
		else if (m_iStateIndex < 16 * timeScale)
		{
			//m_iAnimIndex = 1;
		}
		else
		{
			//m_iAnimIndex = 0;
			//m_iStateIndex = 0;
		}
		break;
	case FORWARD:
		if (m_iStateIndex < 3 * timeScale)
		{
			//m_iAnimIndex = 3;
		}
		else if (m_iStateIndex < 6 * timeScale)
		{
			//m_iAnimIndex = 4;
		}
		else if (m_iStateIndex < 9 * timeScale)
		{
			//m_iAnimIndex = 5;
		}
		else if (m_iStateIndex < 12 * timeScale)
		{
			//m_iAnimIndex = 6;
		}
		else
		{
			//m_iAnimIndex = 0;
			//m_iStateIndex = 0;
		}
		//m_oPos.x += ((5 / timeScale) * m_iDir);
		break;
	case BACK:
		if (m_iStateIndex < 3 * timeScale)
		{
			//m_iAnimIndex = 6;
		}
		else if (m_iStateIndex < 6 * timeScale)
		{
			//m_iAnimIndex = 5;
		}
		else if (m_iStateIndex < 9 * timeScale)
		{
			//m_iAnimIndex = 4;
		}
		else if (m_iStateIndex < 12 * timeScale)
		{
			//m_iAnimIndex = 3;
		}
		else
		{
			//m_iAnimIndex = 0;
			//m_iStateIndex = 0;
		}
		//m_oPos.x += ((-5 / timeScale) * m_iDir);
		break;
	case ATTACK1:
	
	//Handles sound effect playing, makes sure it doesn't play it multiple times
		if (m_iStateIndex < 3 * timeScale)
		{
			if (!hasHit)
			{
				hasHit = true;
				m_FMod->createStream(HitSFX.at(rand() % HitSFX.size()), FMOD_DEFAULT, 0, &m_Sound);
				m_FMod->playSound(m_Sound, 0, false, &m_Channel);
			}
			//m_iAnimIndex = 7;
		}
		else if (m_iStateIndex < 9 * timeScale)
		{
			//m_iAnimIndex = 8;
			if (!hasSwing)
			{
				hasSwing = true;
				m_FMod->createStream(SwingSFX.at(0), FMOD_DEFAULT, 0, &m_Sound);
				m_FMod->playSound(m_Sound, 0, false, &m_Channel);
			}
		}
		else if (m_iStateIndex < 12 * timeScale)
		{
			hasSwing = false;
			hasHit = false;
			//m_iAnimIndex = 9;
		}
		else if (m_iStateIndex < 18 * timeScale)
		{
			//m_iAnimIndex =10;
		}
		else if (m_iStateIndex < 21 * timeScale)
		{
			//m_iAnimIndex = 7;
		
		}
		else
		{
			//m_iAnimIndex = 0;
			//m_iStateIndex = 0;
			//m_oAgentState = IDLE;
		}
		break;
	case ATTACK2:
		if (m_iStateIndex < 3 * timeScale)
		{
			//m_iAnimIndex = 11;
			if (!hasParry)
			{
				hasParry = true;
				m_FMod->createStream(SwingSFX.at(1), FMOD_DEFAULT, 0, &m_Sound);
				m_FMod->playSound(m_Sound, 0, false, &m_Channel);
			}
		}
		else if (m_iStateIndex < 9 * timeScale)
		{
			//m_iAnimIndex = 12;
		}
		else if (m_iStateIndex < 12 * timeScale)
		{
			//m_iAnimIndex = 13;
		}
		else if (m_iStateIndex < 18 * timeScale)
		{
			//m_iAnimIndex = 11;
		}
		else
		{
			//m_iAnimIndex = 0;
			//m_iStateIndex = 0;
			//m_oAgentState = IDLE;
		}
		break;
	case HURT:
		//m_iAnimIndex = 14;
		if (!hasHurt)
		{
			hasHurt = true;
			m_FMod->createStream(HurtSFX.at(rand() % HurtSFX.size()), FMOD_DEFAULT, 0, &m_Sound);
			m_FMod->playSound(m_Sound, 0, false, &m_Channel);
		}
		if (m_iStateIndex < 3 * timeScale)
		{
			//m_oPos.x += ((-20 / timeScale) * m_iDir);
		}
		else if (m_iStateIndex < 5 * timeScale)
		{
			//m_oPos.x += ((-10 / timeScale) * m_iDir);
		}
		else
		{
			//m_iAnimIndex = 0;
			//m_iStateIndex = 0;
			//m_oAgentState = IDLE;
		}
		break;
	case STUNNED:
		//m_iAnimIndex = 15;
		if (!hasStunned)
		{
			hasStunned;
			m_FMod->createStream(ParrySFX, FMOD_DEFAULT, 0, &m_Sound);
			m_FMod->playSound(m_Sound, 0, false, &m_Channel);
		}
		if (m_iStateIndex < 45 * timeScale)
		{
			
		}
		else
		{
			//m_iAnimIndex = 0;
			//m_iStateIndex = 0;
			//m_oAgentState = IDLE;
		}
		break;
	case DEAD:
		//m_iAnimIndex = 14;
		if (!hasDied)
		{
			hasDied = true;
			m_FMod->createStream(DieSFX, FMOD_DEFAULT, 0, &m_Sound);
			m_FMod->playSound(m_Sound, 0, false, &m_Channel);
		}
		break;
	default:
		break;
	}
	m_iStateIndex += 1;
}

void Agent::Draw(SpriteBatch* sb)
{
	//Draws the agent red if they are being hurt
	if (m_oAgentState == HURT)
	{
		sb->setSpriteColor(1, 0, 0, 1);
	}
	else
	{
		sb->setSpriteColor(1, 1, 1, 1);
	}
	//If the agent is dead, draws them at a rotated angle to visualise them lying on the ground
	if (m_oAgentState == DEAD)
	{
		sb->drawSprite(m_oAnimStates[m_iAnimIndex], m_oPos.x, m_oPos.y, TexSize.x, TexSize.y, -1.5708f, 2, 0, 0);
	}
	//Otherwise draws them normally
	else
	{
		sb->drawSprite(m_oAnimStates[m_iAnimIndex], m_oPos.x, m_oPos.y, TexSize.x, TexSize.y);
	}
}