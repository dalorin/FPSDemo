#include "glee.h"
#include "MD3Actor.h"
#include <fstream>
#include <sstream>

using namespace std;

MD3Actor::MD3Actor()
{
}

void MD3Actor::load(string modelPath)
{
	m_head.load(modelPath + "head.md3");
	m_upper.load(modelPath + "upper.md3");
	m_lower.load(modelPath + "lower.md3");
	//m_weapon.load("models/weapons/g_shot.md3");

	m_head.parseSkinFile(modelPath + "head_default.skin");
	m_upper.parseSkinFile(modelPath + "upper_default.skin");
	m_lower.parseSkinFile(modelPath + "lower_default.skin");
	//m_weapon.parseSkinFile("models/weapons/v_shot2_0.skin");

	m_lower.addLink("tag_torso", m_upper);
	m_upper.addLink("tag_head", m_head);
	//m_upper.addLink("tag_weapon", m_weapon);

	parseAnims(modelPath + "animation.cfg");

	setUpperAnimation(TORSO_STAND);
	setLowerAnimation(LEGS_WALK);
	setNextUpperAnimation(NONE);
	setNextLowerAnimation(NONE);
}

void MD3Actor::setMaterialProperties(MaterialProps props)
{
	m_head.setMaterialProperties(props);
	m_upper.setMaterialProperties(props);
	m_lower.setMaterialProperties(props);
}

void MD3Actor::parseAnims(string animFilename)
{
	// Open the file
	ifstream animFile(animFilename.c_str(), ios::binary);
	if (!animFile)
	{
		stringstream str;
		str << "Invalid filename: " << animFilename.c_str();
		MessageBox(NULL, str.str().c_str(), "Animation file error", MB_ICONERROR | MB_OK);
		exit(-1);
	}

	int animCount = 0;
	string tmp;
	while (!animFile.eof())
	{
		// Check that the line begins with numeric data. Parse it if so, otherwise
		// get next line.
		getline(animFile, tmp);
		if (!isdigit(tmp.c_str()[0]))
			continue;

		stringstream str(tmp);

		str >> m_animations[animCount].startFrame;
		str >> m_animations[animCount].numFrames;
		str >> m_animations[animCount].loopingFrames;
		str >> m_animations[animCount].fps;
		// Discard rest of line.

		animCount++;
	}

	// Recalculate LEGS_* starting frames
	int numTorsoFrames = m_animations[LEGS_WALKCR].startFrame - m_animations[TORSO_GESTURE].startFrame;
	for (int i = LEGS_WALKCR; i < MAX_ANIMATIONS; i++)
		m_animations[i].startFrame -= numTorsoFrames;
}

AnimationPhase MD3Actor::getUpperAnimation()
{
	return m_upperAnimPhase;
}

AnimationPhase MD3Actor::getLowerAnimation()
{
	return m_lowerAnimPhase;
}

AnimationPhase MD3Actor::getNextUpperAnimation()
{
	return m_nextUpperAnimPhase;
}

AnimationPhase MD3Actor::getNextLowerAnimation()
{
	return m_nextLowerAnimPhase;
}

void MD3Actor::setUpperAnimation(AnimationPhase phase)
{
	m_upperAnimPhase = phase;
	if (phase != NONE)
	{
		m_upper.setAnimationParams(
			m_animations[phase].startFrame,
			m_animations[phase].startFrame + m_animations[phase].numFrames,
			m_animations[phase].fps
		);
	}
}

void MD3Actor::setLowerAnimation(AnimationPhase phase)
{
	m_lowerAnimPhase = phase;
	if (phase != NONE)
	{
		m_lower.setAnimationParams(
			m_animations[phase].startFrame,
			m_animations[phase].startFrame + m_animations[phase].numFrames,
			m_animations[phase].fps
		);
	}
}

void MD3Actor::setNextUpperAnimation(AnimationPhase phase)
{
	m_nextUpperAnimPhase = phase;	
}

void MD3Actor::setNextLowerAnimation(AnimationPhase phase)
{
	m_nextLowerAnimPhase = phase;	
}

SimpleBox* MD3Actor::getCollider()
{
	GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
		
	// Get colliders of all component models and combine to form actor collider.
	SimpleBox* colliders[3];

	colliders[0] = m_head.getCollider();
	colliders[1] = m_upper.getCollider();
	colliders[2] = m_lower.getCollider();

	min_x = colliders[0]->getX1();
	max_x = colliders[0]->getX2();
	min_y = colliders[0]->getY1();
	max_y = colliders[0]->getY2();
	min_z = colliders[0]->getZ1();
	max_z = colliders[0]->getZ2();

	for (int i = 1; i < 3; i++)
	{
		if (colliders[i]->getX1() < min_x)
			min_x = colliders[i]->getX1();
		if (colliders[i]->getX2() > max_x)
			max_x = colliders[i]->getX2();

		if (colliders[i]->getY1() < min_y)
			min_y = colliders[i]->getY1();
		if (colliders[i]->getY2() > max_y)
			max_y = colliders[i]->getY2();

		if (colliders[i]->getZ1() < min_z)
			min_z = colliders[i]->getZ1();
		if (colliders[i]->getZ2() > max_z)
			max_z = colliders[i]->getZ2();
	}

	return new SimpleBox(min_x, max_x, min_y, max_y, min_z, max_z);
}

void MD3Actor::onPrepare(float dt)
{	
	m_head.onPrepare(dt);
	m_upper.onPrepare(dt);
	if (m_upper.hasAnimationFinished() && m_nextUpperAnimPhase != NONE)
	{
		setUpperAnimation(m_nextUpperAnimPhase);
		setNextUpperAnimation(NONE);
	}
	m_lower.onPrepare(dt);
	if (m_lower.hasAnimationFinished() && m_nextLowerAnimPhase != NONE)
	{
		setLowerAnimation(m_nextLowerAnimPhase);
		setNextLowerAnimation(NONE);
	}
}

void MD3Actor::onRender(ShaderProgram *shaderProgram)
{	
	m_lower.onRender(shaderProgram);
}

MD3Actor::~MD3Actor(void)
{	
}
