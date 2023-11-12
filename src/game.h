#pragma once

// Entorn VGI: Llibreries i constants Constants de l'aplicació EntornVGI
#include "stdafx.h"
#include <irrKlang.h>
// Inventory items
#define ITEM_NONE 0
#define ITEM_KEY 1
#define ITEM_NOTE 2
#define ITEM_CANDLE 3

// Scenes
#define SCENE_MAIN 1
#define SCENE_GAME 2
#define SCENE_DEBUG_TEST 10
#define SCENE_DEBUG_TEX 11
#define SCENE_PUZLE1 12


class GameState {
public:

	GameState();

	// ABP: Game control variables
	

	// ABP: Player current room
	int currentRoom = 1;

	void ChangeDebugCubePos(vec3 pos);

	// Executes on mouse/kb input
	void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods);
	void OnKeyUp(GLFWwindow* window, int key, int scancode, int action, int mods);
	void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
	void OnMouseButtonRelease(GLFWwindow* window, int button, int action, int mods);
	void OnMouseMove(GLFWwindow* window, double xpos, double ypos);
	void OnMouseWheel(GLFWwindow* window, double xoffset, double yoffset);

	// Executes every frame. Delta is the time difference between frames (e.g 0.16666 seconds if 60FPS)
	void UpdateGame(float delta);
	

//private:

	// For changing screen coords to world coords
	mat4 *m_ViewMatrix, *m_ProjectionMatrix;

	// Variables for camera movement
	double* n;				// Punt cap on mira.
	CPunt3D* opvN;				// Definició Orígen Punt de Vista (en coordenades món)
	double* angleZ;				// angle de rotació de la trajectòria.

	// Game Scene
	int *gameScene;	// current scene

	bool gameOver = false;
	float gameOverCountdownInSeconds = 3; // Cuando se acaba el tiempo (30s) es game over.

	float timeAcumm = 0;	// Useful for animations

	bool isMouseDown = false;
	double previousMouse_xpos, previousMouse_ypos;

	// Debug cube
	CColor cube_color;
	vec3 debug_cube_pos;
	vec3 debug_cube_rotation;
	vec3 debug_cube_scale;

	// item inspector
	bool showItemInspector = false;
	int currentItem = ITEM_NONE;
	vec3 item_inspect_rotation = vec3(0.0f, 0.0f, 0.0f);
	vec3 item_inspect_scale = vec3(1.0f, 1.0f, 1.0f);

	// Puzle 1: Cadenat amb simbols
	int puz1_n_Symbols = 4;
	int puz1_currentCombination[4] = { 0, 0, 0, 0 };
	int puz1_correctCombination[4] = { 1, 1, 1, 2 };

};


glm::vec3 screenToNDC(float screenX, float screenY, float screenWidth, float screenHeight);
glm::vec3 NDCToViewSpace(vec3 ndcCoords, mat4 ProjectionMatrix);
glm::vec3 ViewSpaceToWorld(vec3 viewSpaceCoords, mat4 ViewMatrix);