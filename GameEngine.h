#pragma once
#include <vector>
#include "Shader.h"
#include "Camera.h"

class Player; // Forward declaration of Player class.
class TexturedLandscape; // Forward declaration of Landscape class.
class SkyBox; // Forward declaration of Landscape class.
class Weapon; // Forward declaration of Weapon class.

enum EntityType {LANDSCAPE, BULLET, CHAOS_MARINE, VARGE};

class GameEngine
{
public:
	GameEngine(void);
	bool init();
	void prepare(float dt);
	void render();
	void shutdown();
	void onResize(int width, int height);

	void processInputs();
	void processInput();
	void processMouseInput();

	void spawnEntity(EntityType type, Vector3 position, Vector3 velocity, Vector3 acceleration);

	void setupPerspectiveProj();
	void setupOrthoProj();

	int getWidth();
	int getHeight();

	Camera* getCamera();
	Vector3 getCameraPos();

	~GameEngine(void);

	//Shader programs
	ShaderProgram *m_basicProgram;
	ShaderProgram *m_modelProgram;
	ShaderProgram *m_hudProgram;
	ShaderProgram *m_skyboxProgram;
	ShaderProgram *m_particleProgram;

private:
	//Entity vectors
	std::vector<Object*> m_objects;
	std::vector<Camera*> m_cameras;
	std::vector<Object*> m_HUD;

	//Weapon object
	Weapon* m_weapon;

	//Player object
	Player* m_player;

	//Landscape object
	TexturedLandscape* m_landscape;

	//Skybox object
	SkyBox* m_skybox;

	GLfloat lightAngle;

	std::map<EntityType, DWORD> m_lastSpawnTime;

	int m_width;
	int m_height;

	static int colCount;
};
