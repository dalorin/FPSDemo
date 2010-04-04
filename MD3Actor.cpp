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
	m_weapon.load("models/weapons/g_shot.md3");

	m_head.parseSkinFile(modelPath + "head_default.skin");
	m_upper.parseSkinFile(modelPath + "upper_default.skin");
	m_lower.parseSkinFile(modelPath + "lower_default.skin");
	//m_weapon.parseSkinFile("models/weapons/v_shot2_0.skin");

	m_lower.addLink("tag_torso", m_upper);
	m_upper.addLink("tag_head", m_head);
	m_upper.addLink("tag_weapon", m_weapon);

	parseAnims(modelPath + "animation.cfg");

	setUpperAnimation(TORSO_STAND);
	setLowerAnimation(LEGS_WALK);
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

void MD3Actor::setUpperAnimation(AnimationPhase phase)
{
	m_upper.setAnimationParams(
		m_animations[phase].startFrame,
		m_animations[phase].startFrame + m_animations[phase].numFrames,
		m_animations[phase].fps
	);
}

void MD3Actor::setLowerAnimation(AnimationPhase phase)
{
	m_lower.setAnimationParams(
		m_animations[phase].startFrame,
		m_animations[phase].startFrame + m_animations[phase].numFrames,
		m_animations[phase].fps
	);
}

void MD3Actor::onPrepare(float dt)
{	
	m_head.onPrepare(dt);
	m_upper.onPrepare(dt);
	m_lower.onPrepare(dt);
}

void MD3Actor::onRender(ShaderProgram *shaderProgram)
{	
	m_lower.onRender(shaderProgram);
}

MD3Actor::~MD3Actor(void)
{	
}
