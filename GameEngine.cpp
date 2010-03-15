#include <Windows.h>
#include "glee.h"
#include <gl/GLU.h>
#include <fstream>
#include <string>
#include <sstream>
#include "GameEngine.h"
#define _USE_MD3_MODELS
#include "Model.h"
#include "Fighter.h"
#include "Player.h"
#include "TexturedLandscape.h"
#include "SkyBox.h"
#include "Weapon.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

GLfloat rotation = 0.0f;


GLfloat dtf(GLfloat degrees)
{
	return degrees * (GLfloat)M_PI / 180.0f;
}
GameEngine::GameEngine(void)
{
}

bool GameEngine::init()
{
	Camera *mainCamera = new Camera(this);
	mainCamera->adjustYaw(180.0f);
	m_cameras.push_back(mainCamera);

	lightAngle = 0.0f;
	
	m_skybox = new SkyBox(this, 
						  "skybox/city_north.tga",
						  "skybox/city_south.tga",
						  "skybox/city_east.tga",
						  "skybox/city_west.tga",
						  "skybox/city_up.tga",
						  "skybox/city_down.tga");

	m_landscape = new TexturedLandscape(this);

	m_player = new Player(this);

	Fighter *fighter = new Fighter(this);
	fighter->setScale(1.0f, 1.0f, 1.0f);
	fighter->setPosition(0.0f, 10.0f, -10.0f);
	fighter->setAcceleration(0.0f, 0.0f, 0.0f);
	m_objects.push_back(fighter);

	/*Weapon *weapon = new Weapon(this);
	weapon->setScale(1.0f, 1.0f, 1.0f);
	weapon->setPosition(mainCamera->getPosition().x + 0.2f, mainCamera->getPosition().y - 0.2f, mainCamera->getPosition().z - 1.0f);
	weapon->setAcceleration(0.0f, 0.0f, 0.0f);
	m_objects.push_back(weapon);*/

	Crosshair *crosshair = new Crosshair(this);
	crosshair->setPosition(m_width / 2.0f, m_height / 2.0f, -1.0f);	
	m_HUD.push_back(crosshair);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Init model shader program.
	//m_modelProgram = new ShaderProgram("model.vert", "model.frag");
	m_modelProgram = new ShaderProgram("model_pixel_lighting.vert", "model_pixel_lighting.frag");
	
	m_modelProgram->bindAttrib(0, "a_Vertex");
	m_modelProgram->bindAttrib(1, "a_TexCoord0");
	m_modelProgram->bindAttrib(2, "a_Normal");

	m_modelProgram->link();	
		
	// Init basic shader program.
	m_basicProgram = new ShaderProgram("basic.vert", "basic.frag");

	m_basicProgram->bindAttrib(0, "a_Vertex");
	m_basicProgram->bindAttrib(1, "a_Color");

	m_basicProgram->link();

	// Init mouse.
	SetCursorPos(getWidth() / 2, getHeight() / 2);
	ShowCursor(FALSE);
	
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		MessageBox(NULL, "Error initializing game engine", "OpenGL Error!", MB_ICONERROR | MB_OK);
	}

	return true;
}

/* processInput()
   Gets activated keys and mouse buttons and performs relevant actions. */
void GameEngine::processInput()
{
	// Build a vector of activated keys. Shift MSB to LSB for easy boolean comparison.
	vector<int> activeKeys;
	for (int i = 0; i < 256; ++i)
		if (GetAsyncKeyState(i) >> 8)
			activeKeys.push_back(i);

	Camera* cam;
	cam = m_cameras[0];

	const GLfloat step = 3.0f;

	// Take action for activated keys.
	for (vector<int>::iterator key = activeKeys.begin();
		 key != activeKeys.end();
		 ++key)
	{
		switch (*key)
		{
		case 'W' :
		case VK_UP :
		{
			cam->moveForward();			
			break;
		}
		case 'A': 
			cam->strafeLeft(step);
			break;
		case 'S' :
		case VK_DOWN :
		{
			cam->moveBackward();
			break;
		}
		case 'D' :
			cam->strafeRight(step);
			break;
		case VK_LEFT :
			cam->adjustYaw(1.0f);
			cam->reposition();
			break;
		case VK_RIGHT :
			cam->adjustYaw(-1.0f);
			cam->reposition();
			break;
		case 'R' :
			cam->setPosition(0.0f, 10.0f, 4.0f);
			cam->setSubject(0.0f, 10.0f, 3.0f);
			cam->resetYaw();
			cam->resetPitch();
		case 'E' :
			cam->setPosition(cam->getPosition().x, cam->getPosition().y + step, cam->getPosition().z);
			cam->setSubject(cam->getSubject().x, cam->getSubject().y + step, cam->getSubject().z);
			break;
		case 'C' :
			cam->setPosition(cam->getPosition().x, cam->getPosition().y - step, cam->getPosition().z);
			cam->setSubject(cam->getSubject().x, cam->getSubject().y - step, cam->getSubject().z);
			break;
		case VK_LBUTTON :
			m_player->shoot();
		case VK_RBUTTON :
			//nothing at the moment
		default :
			break;
		}
	}
}

void GameEngine::processMouseInput()
{
	GLfloat sensitivity = 40.0f;

	// Get current mouse position.
	GLfloat x, y;
	POINT pos;
	GetCursorPos(&pos);	
	x = (GLfloat)pos.x / getWidth();
	y = 1.0f - ((GLfloat)pos.y / getHeight());

	// Subtract starting position (i.e. centre of viewport).
	x -= 0.5f;
	y -= 0.5f;
	
	// Adjust camera.
	Camera* cam;
	cam = m_cameras[0];
	cam->adjustYaw(-x * sensitivity);
	cam->adjustPitch(y * sensitivity);
	cam->reposition();

	// Return cursor to centre of viewport.
	SetCursorPos(getWidth() / 2, getHeight() / 2);
}

void GameEngine::spawnEntity(EntityType type)
{
	switch(type)
	{
	case BULLET :
	{
		Camera *cam = m_cameras[0];
		Vector3 subject = cam->getSubject();
		Vector3 subjectRel = cam->getSubjectRelative();
		Box *bullet = new Box(this, 0.3f);
		bullet->setPosition(subject.x, subject.y, subject.z);		
		bullet->setVelocity(subjectRel.x * 2.0f, subjectRel.y * 2.0f, subjectRel.z * 2.0f);
		m_objects.push_back(bullet);
	}
	}
}

void GameEngine::prepare(float dt)
{
	// Process keyboard and mouse button input.
	processInput();
	processMouseInput();
	std::vector<Object*>::iterator it = m_objects.begin();
	while (it != m_objects.end())
	{
		(*it)->onPrepare(dt);
		if (!(*it)->isAlive())
		{
			delete *it;
			it = m_objects.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void GameEngine::render()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Camera *mainCamera;
	mainCamera = m_cameras[0];
	gluLookAt((GLdouble)mainCamera->getPosition().x, (GLdouble)mainCamera->getPosition().y, (GLdouble)mainCamera->getPosition().z,
			  (GLdouble)mainCamera->getSubject().x, (GLdouble)mainCamera->getSubject().y, (GLdouble)mainCamera->getSubject().z,
			  0.0, 1.0, 0.0);

	// Disable depth buffer writes before rendering skybox.
	glDepthMask(0);
	
	// Render skybox first.
	m_modelProgram->bind();
	m_modelProgram->sendUniform("fog_density", 0.01f);
	m_modelProgram->sendUniform("light0.position", 50.0f, 100.0f, 120.0f, 0.0f);
	m_modelProgram->sendUniform("light0.ambient", 1.0f, 1.0f, 1.0f, 1.0f);
	m_modelProgram->sendUniform("light0.diffuse", 0.9f, 0.9f, 0.9f, 1.0f);
	m_modelProgram->sendUniform("light0.specular", 0.8f, 0.8f, 0.8f, 1.0f);
	m_skybox->onRender(m_modelProgram);

	// Re-enable depth buffer writes.
	glDepthMask(1);

	// Render landscape.
	//m_basicProgram->bind();
	m_landscape->onRender(m_modelProgram);

	// Render game entities.
	//m_modelProgram->bind();

	for (std::vector<Object*>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		(*it)->onRender(m_modelProgram);
		(*it)->onPostRender();
	}

	// Once the game world has rendered, we switch to an orthographic projection and draw the HUD.
	setupOrthoProj();
	m_modelProgram->sendUniform("fogDensity", 0.0f);

	for (std::vector<Object*>::iterator it = m_HUD.begin(); it != m_HUD.end(); ++it)
	{
		(*it)->onRender(m_modelProgram);
		(*it)->onPostRender();
	}

	// Return to perspective projection.
	setupPerspectiveProj();
}

void GameEngine::shutdown()
{
	for (std::vector<Camera*>::iterator it = m_cameras.begin(); it != m_cameras.end(); ++it)
		delete *it;
	m_cameras.clear();
	for (std::vector<Object*>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
		delete *it;
	m_objects.clear();
}

void GameEngine::onResize(int width, int height)
{
	m_width = width;
	m_height = height;
	glViewport(0, 0, width, height);

	setupPerspectiveProj();

	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		MessageBox(NULL, "Error resizing OpenGL window", "OpenGL Error!", MB_ICONERROR | MB_OK);
	}

}

void GameEngine::setupPerspectiveProj()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)m_width/(double)m_height, 1.0, 1500.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GameEngine::setupOrthoProj()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (double)m_width, 0.0, (double)m_height, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int GameEngine::getWidth()
{
	return m_width;
}

int GameEngine::getHeight()
{
	return m_height;
}

GameEngine::~GameEngine(void)
{
}
