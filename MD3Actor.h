/**
 * MD3Actor
 * Defines a complete MD3 model by managing the head, upper and lower components.
 **/

#pragma once
#include "MD3Model.h"
#include <string>

enum AnimationPhase
{
	BOTH_DEATH1 = 0,
	BOTH_DEAD1  = 1,
	BOTH_DEATH2 = 2,
	BOTH_DEAD2  = 3,
	BOTH_DEATH3 = 4,
	BOTH_DEAD3  = 5,

	TORSO_GESTURE = 6,
	TORSO_ATTACK  = 7,
	TORSO_ATTACK2 = 8,
	TORSO_DROP    = 9,
	TORSO_RAISE   = 10,
	TORSO_STAND   = 11,
	TORSO_STAND2  = 12,

	LEGS_WALKCR   = 13,
	LEGS_WALK     = 14,
	LEGS_RUN      = 15,
	LEGS_BACK     = 16,
	LEGS_SWIM     = 17,
	LEGS_JUMP     = 18,
	LEGS_LAND     = 19,
	LEGS_JUMPB    = 20,
	LEGS_LANDB    = 21,
	LEGS_IDLE     = 22,
	LEGS_IDLECR   = 23,
	LEGS_TURN     = 24,

	MAX_ANIMATIONS = 25,
	NONE = 26
};

struct Animation
{	
	int startFrame;
	int numFrames;
	int loopingFrames;
	int fps;
};

class MD3Actor
{
public:
	MD3Actor();
	~MD3Actor(void);

	void load(std::string modelPath);
	void setMaterialProperties(MaterialProps props);
	void parseAnims(std::string modelPath);
	AnimationPhase getLowerAnimation();
	AnimationPhase getUpperAnimation();
	AnimationPhase getNextLowerAnimation();
	AnimationPhase getNextUpperAnimation();
	void setLowerAnimation(AnimationPhase phase);
	void setUpperAnimation(AnimationPhase phase);
	void setNextLowerAnimation(AnimationPhase phase);
	void setNextUpperAnimation(AnimationPhase phase);
	void onPrepare(float dt);
	void onRender(ShaderProgram *shaderProgram);

	SimpleBox* getCollider();

private:
	MD3Model m_head;
	MD3Model m_upper;
	MD3Model m_lower;
	MD3Model m_weapon;

	Animation m_animations[MAX_ANIMATIONS];
	AnimationPhase m_lowerAnimPhase;
	AnimationPhase m_upperAnimPhase;
	AnimationPhase m_nextLowerAnimPhase;
	AnimationPhase m_nextUpperAnimPhase;
};
