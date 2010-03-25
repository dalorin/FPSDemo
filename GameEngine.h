#pragma once
#include <vector>
#include "Shader.h"
#include "Camera.h"

class Player; // Forward declaration of Player class.
class TexturedLandscape; // Forward declaration of Landscape class.
class SkyBox; // Forward declaration of Landscape class.

enum EntityType {LANDSCAPE, BULLET};

class GameEngine
{
public:
	GameEngine(void);
	bool init();
	void prepare(float dt);
	void render();
	void shutdown();
	void onResize(int width, int height);

	void processInput();
	void processMouseInput();

	void spawnEntity(EntityType type);

	void setupPerspectiveProj();
	void setupOrthoProj();

	int getWidth();
	int getHeight();

	~GameEngine(void);

private:
	//Entity vectors
	std::vector<Object*> m_objects;
	std::vector<Camera*> m_cameras;
	std::vector<Object*> m_HUD;

	//Player object
	Player* m_player;

	//Landscape object
	TexturedLandscape* m_landscape;

	//Skybox object
	SkyBox* m_skybox;

	//Shader programs
	ShaderProgram *m_basicProgram;
	ShaderProgram *m_modelProgram;
	ShaderProgram *m_hudProgram;

	GLfloat lightAngle;

	int m_width;
	int m_height;
};
