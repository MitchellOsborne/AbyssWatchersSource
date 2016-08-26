#include "Agent.h"

Agent::Agent(glm::vec2 a_oInitPos, int a_iHealth, int a_iStamina, bool a_bIsLeft, std::vector<Texture*> a_oTextures, std::vector<CollisionBox*>&a_oCBoxes, FMOD::System* a_FMod)
{
	m_oAgentState = AgentState::IDLE;
	m_oPos = a_oInitPos;
	m_iHealth = a_iHealth;
	m_iMaxStamina = a_iStamina;
	m_iCurrStamina = a_iStamina;
	m_uiClientID = -1;
	Score = 0;
	if (!a_bIsLeft)
	{
		m_iDir = -1;
	}
	
	//Creates new Collision Boxes for body and attacking
	BodyBox = new CollisionBox(m_oPos, glm::vec2(0 * m_iDir,-5), glm::vec2(50, 120),  BoxType::BODY);
	BodyBox->SetOwner(this);
	BodyBox->Active = true;
	a_oCBoxes.push_back(BodyBox);
	AttackBox = new CollisionBox(m_oPos, glm::vec2(80, 0), glm::vec2(100, 120),  BoxType::ATTACK);
	AttackBox->SetOwner(this);
	a_oCBoxes.push_back(AttackBox);
	ParryBox = new CollisionBox(m_oPos, glm::vec2(40, 0), glm::vec2(40, 40),  BoxType::PARRY);
	ParryBox->SetOwner(this);
	a_oCBoxes.push_back(ParryBox);

	//Adds pointers to textures to a local list
	for each(Texture* tex in a_oTextures)
	{
		m_oAnimStates.push_back(tex);
	}

	//Initialising Audio
	
	m_FMod = a_FMod;

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

//Checks the agents current state and checks whethers its new state is valid for changing to
void Agent::ChangeState(AgentState state)
{
	if (state == ATTACK1)
	{
		if (m_iCurrStamina <= AttackStam * 0.75f)
		{
			state = IDLE;
		}
	}
	else if (state == ATTACK2)
	{
		if (m_iCurrStamina <= ParryStam* 0.75f)
		{
			state = IDLE;
		}
	}

	//Being Hurt or Stunned interrupts most states, and activates immediately
	if (state == HURT || state == STUNNED)
	{
		//Checks which audio it needs to play, and makes sure it doesn't play it multiple times
		if (state == STUNNED && m_oAgentState != STUNNED)
		{
			m_FMod->createStream(ParrySFX, FMOD_DEFAULT, 0, &m_Sound);
			m_FMod->playSound(m_Sound, 0, false, &m_Channel);
		}
		if (m_oAgentState != HURT)
		{
			m_FMod->createStream(HurtSFX.at(rand() % HurtSFX.size()), FMOD_DEFAULT, 0, &m_Sound);
			m_FMod->playSound(m_Sound, 0, false, &m_Channel);
		}

		//Updates state and resets hitbox data and state index
		m_oAgentState = state;
		m_oNextState = IDLE;
		m_iStateIndex = 0;
		AttackBox->Active = false;
		ParryBox->Active = false;
		BodyBox->Active = true;
	}
	else	//Activation of the rest of the states is handles differently to being stunned or hurt
	{

		//Checks the new state is not the same as its current state
		//resets state index
		switch (m_oAgentState)
		{
		case IDLE:
			if (state != IDLE)
			{
				m_oAgentState = state;
				m_iStateIndex = 0;
			}
			break;
		case FORWARD:
			if (state != FORWARD)
			{
				m_oAgentState = state;
				m_iStateIndex = 0;
			}
			break;
		case BACK:
			if (state != BACK)
			{
				m_oAgentState = state;
				m_iStateIndex = 0;
			}
			break;
		//The following states are activated after the current state has finished
		case ATTACK1:
			m_oNextState = state;
			break;
		case ATTACK2:
			m_oNextState = state;
			break;
		case HURT:
			m_oNextState = state;
			break;
		case STUNNED:
			m_oNextState = state;
			break;
		case DEAD:
			break;
		default:
			break;
		}
	}
	
}

void Agent::Update(float timeScale )
{
	if (m_iHealth <= 0)
	{
		m_oAgentState = DEAD;
	}
	
	//The core of the agent state machine (if you can call it that)
	switch (m_oAgentState)
	{
	case IDLE:
		hasDied = false;	//Avoids the player spawning into the next round dead
		//Checks the agents state index against the games timescale, 
		//adjusts the agent accordingly
		if (m_iStateIndex < 4 * timeScale)
		{
			m_iAnimIndex = 0;	//Which image/frame to draw when draw is called
		}
		else if (m_iStateIndex < 8 * timeScale)
		{
			m_iAnimIndex = 1;
		}
		else if (m_iStateIndex < 12 * timeScale)
		{
			m_iAnimIndex = 2;
		}
		else if (m_iStateIndex < 16 * timeScale)
		{
			m_iAnimIndex = 1;
		}
		else
		{
			//State index has reached max and resets to 0, or start of animation/state
			m_iAnimIndex = 0;
			m_iStateIndex = 0;
		}
		m_iCurrStamina += StamRegen * timeScale;
		break;
	case FORWARD:
		if (m_iStateIndex < 3 * timeScale)
		{
			m_iAnimIndex = 3;
		}
		else if (m_iStateIndex < 6 * timeScale)
		{
			m_iAnimIndex = 4;
		}
		else if (m_iStateIndex < 9 * timeScale)
		{
			m_iAnimIndex = 5;
		}
		else if (m_iStateIndex < 12 * timeScale)
		{
			m_iAnimIndex = 6;
		}
		else
		{
			m_iAnimIndex = 0;
			m_iStateIndex = 0;
		}
		m_iCurrStamina += StamRegen * timeScale;
		m_oPos.x += ((5 / timeScale) * m_iDir);
		break;
	case BACK:
		if (m_iStateIndex < 3 * timeScale)
		{
			m_iAnimIndex = 6;
		}
		else if (m_iStateIndex < 6 * timeScale)
		{
			m_iAnimIndex = 5;
		}
		else if (m_iStateIndex < 9 * timeScale)
		{
			m_iAnimIndex = 4;
		}
		else if (m_iStateIndex < 12 * timeScale)
		{
			m_iAnimIndex = 3;
		}
		else
		{
			m_iAnimIndex = 0;
			m_iStateIndex = 0;
		}
		m_iCurrStamina += StamRegen * timeScale;
		m_oPos.x += ((-5 / timeScale) * m_iDir);
		break;
	case ATTACK1:
		if (m_iStateIndex < 3 * timeScale)
		{
			//Checks if the HitSFX (usually the character grunting in some way) has been played, if not, play it
			if (!hasHit)
			{
				hasHit = true;
				//Randomly chooses from a list of HitSFX
				m_FMod->createStream(HitSFX.at(rand() % HitSFX.size()), FMOD_DEFAULT, 0, &m_Sound);
				m_FMod->playSound(m_Sound, 0, false, &m_Channel);
			}
			m_iAnimIndex = 7;
		}
		else if (m_iStateIndex < 9 * timeScale)
		{
			m_iAnimIndex = 8;
			//Plays the SwingSFX to match up with the visual effect of the sword being swung
			if (!hasSwing)
			{
				hasSwing = true;
				m_FMod->createStream(SwingSFX.at(0), FMOD_DEFAULT, 0, &m_Sound);
				m_FMod->playSound(m_Sound, 0, false, &m_Channel);
			}
			
		}
		//Activates Attack hit box, begins draining stamina
		else if (m_iStateIndex < 12 * timeScale)
		{
			m_iAnimIndex = 9;
			AttackBox->Active = true;
			m_iCurrStamina -= (AttackStam / 3) * timeScale;
			

		}
		//Attack swing has ended, hitbox disabled
		else if (m_iStateIndex < 18 * timeScale)
		{
			m_iAnimIndex =10;
			AttackBox->Active = false;
		}
		else if (m_iStateIndex < 21 * timeScale)
		{
			m_iAnimIndex = 7;
		}
		else
		{
			m_iAnimIndex = 0;
			m_iStateIndex = 0;
			m_oAgentState = m_oNextState;
			m_oNextState = IDLE;

			hasSwing = false;
			hasHit = false;

		}
		break;
	case ATTACK2:
		if (m_iStateIndex < 3 * timeScale)
		{
			m_iAnimIndex = 11;
			//Same deal as attack, plays parry sfx if it hasn't already been played
			if (!hasParry)
			{
				hasParry = true;
				m_FMod->createStream(SwingSFX.at(1), FMOD_DEFAULT, 0, &m_Sound);
				m_FMod->playSound(m_Sound, 0, false, &m_Channel);
			}
		}
		//Activates parry hitbox, 
		//during first few frames the players bodybox is still exposed, 
		//if the player is hit by an attack during this point, the parry will fail
		//good spacing however will allow the first 3 frames of the parry to work, making parrying easier
		else if (m_iStateIndex < 9 * timeScale)
		{
			m_iAnimIndex = 12;
			
			ParryBox->Active = true;
		}
		//Disables body hitbox, this is the parry's sweetspot where 
		//the player is invulnerable to attack and can parry uninterrupted
		else if (m_iStateIndex < 12 * timeScale)
		{
			m_iAnimIndex = 13;
			BodyBox->Active = false;
			m_iCurrStamina -= (ParryStam / 3) * timeScale;
		}
		//Ending frames for parry, Parry box disabled, body hitbox re-enabled
		else if (m_iStateIndex < 18 * timeScale)
		{
			m_iAnimIndex = 11;
			ParryBox->Active = false;
			BodyBox->Active = true;
		}
		else
		{
			m_iAnimIndex = 0;
			m_iStateIndex = 0;
			m_oAgentState = m_oNextState;
			m_oNextState = IDLE;
			hasParry = false;
		}
		break;
	case HURT:
		//If hurt, knocks the player back and deals damage
		m_iAnimIndex = 14;
		//Player is knocked further back per frame during first few frames
		if (m_iStateIndex < 3 * timeScale)
		{
			m_iHealth -= 2 * timeScale;
			m_oPos.x += ((-20 / timeScale) * m_iDir);
		}
		//Players knockback slowed during last few frames of state (gives visual effect of lerping)
		else if (m_iStateIndex < 5 * timeScale)
		{
			m_iHealth -= 2 * timeScale;
			m_oPos.x += ((-10 / timeScale) * m_iDir);
		}
		else
		{
			m_iAnimIndex = 0;
			m_iStateIndex = 0;
			m_oAgentState = m_oNextState;
			m_oNextState = IDLE;
		}
		break;
	case STUNNED:
		//If the players attack is parried, stuns the player for a period, allowing a free attack
		m_iAnimIndex = 15;
		if (m_iStateIndex < 45 * timeScale)
		{
			//45 Glorious frames of not doing anything
		}
		else
		{
			m_iAnimIndex = 0;
			m_iStateIndex = 0;
			m_oAgentState = m_oNextState;
			m_oNextState = IDLE;
		}
		break;
	case DEAD:
	//If your dead, disable hitboxes, and play the DyingSFX if it isn't already
		BodyBox->Active = false;
		AttackBox->Active = false;
		ParryBox->Active = false;
		m_iAnimIndex = 14;
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
	if (m_iCurrStamina > m_iMaxStamina)
	{
		m_iCurrStamina = m_iMaxStamina;
	}
}

void Agent::Draw(SpriteBatch* sb)
{
	//Makes the player flash red when hurt
	if (m_oAgentState == HURT)
	{
		sb->setSpriteColor(1, 0, 0, 1);
	}
	else
	{
		sb->setSpriteColor(1, 1, 1, 1);
	}
	//If dead, draws the player on this wonky angle to simulte lying on the ground dead (looks pretty cool)
	//Otherwsie just draw the player normally
	if (m_oAgentState == DEAD)
	{
		sb->drawSprite(m_oAnimStates[m_iAnimIndex], m_oPos.x, m_oPos.y, TexSize.x, TexSize.y,-1.5708f,2,0,0);
	}
	else
	{
		sb->drawSprite(m_oAnimStates[m_iAnimIndex], m_oPos.x, m_oPos.y, TexSize.x, TexSize.y);
	}
}