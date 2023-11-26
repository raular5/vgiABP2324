#pragma once

// Entorn VGI: Llibreries i constants Constants de l'aplicació EntornVGI
#include "stdafx.h"
#include <irrKlang.h>

/* ------------------------------------------------------------------------- */
/*							CONSTANTS										 */
/* ------------------------------------------------------------------------- */

// Inventory items
#define ITEM_NONE	0
#define ITEM_KEY	1
#define ITEM_NOTE	2
#define ITEM_CANDLE 3

// Scenes
#define SCENE_MAIN				1
#define SCENE_GAME				2
#define SCENE_TIMER_GAMEOVER	3
#define SCENE_DEBUG_TEST		10
#define SCENE_DEBUG_TEX			11
#define SCENE_PUZLE1			12
#define SCENE_BACKGROUND		13
#define SCENE_PUZLE2			14
#define SCENE_PUZLE3			15
#define SCENE_PUZLE4			16
#define SCENE_PUZLE5			17

// Models
#define MODEL_GOTHIC_BED 15
#define MODEL_GOTHIC_BENCH1 16
#define MODEL_GOTHIC_BENCH2 17
#define MODEL_GOTHIC_CANDLESTICK 18
#define MODEL_GOTHIC_CHAIR 19
#define MODEL_GOTHIC_DESK 20
#define MODEL_GOTHIC_DINNERTABLE 21
#define MODEL_GOTHIC_DRESSER 22

/* ------------------------------------------------------------------------- */
/*					          FUNCTIONS										 */
/* ------------------------------------------------------------------------- */
glm::vec3 screenToNDC(float screenX, float screenY, float screenWidth, float screenHeight);
glm::vec3 NDCToViewSpace(vec3 ndcCoords, mat4 ProjectionMatrix);
glm::vec3 ViewSpaceToWorld(vec3 viewSpaceCoords, mat4 ViewMatrix);

/* ------------------------------------------------------------------------- */
/*					          CLASE GAMESTATE								 */
/* ------------------------------------------------------------------------- */

class GameState {
public:

	GameState();

	
// CONEXION CON MAIN
	// For changing screen coords to world coords
	mat4 *m_ViewMatrix, *m_ProjectionMatrix;

	// Variables for camera movement
	double* n;					// Punt cap on mira.
	CPunt3D* opvN;				// Definició Orígen Punt de Vista (en coordenades món)
	double* angleZ;				// angle de rotació de la trajectòria.

	// Game Scene
	int *gameScene;	// current scene

// VARIABLES
	bool gameOver = false;
	float gameOverCountdownInSeconds = 3; // Cuando se acaba el tiempo (30s) es game over.

	float timeAcumm = 0;	// Useful for animations

	bool isMouseDown = false;
	bool firstMouseMovement = true;
	double previousMouse_xpos, previousMouse_ypos;
	double clickPosWorld_x, clickPosWorld_y, clickPosWorld_z;

	irrklang::ISoundEngine* audioEngine = nullptr;

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
	bool puz1_match = false;
	bool puz1_checkMatch(); // se podria usar tambien para el candado numerico

	// Puzle 2: joya en estatua
	bool puz2_hasPickedGem = false;
	bool puz2_complete = false;

	// Puzle 3: Cadenat amb simbols
	int puz3_n_Symbols = 4;
	int puz3_currentCombination[4] = { 0, 0, 0, 0 };
	int puz3_correctCombination[4] = { 1, 9, 6, 2 };
	bool puz3_match = false;
	bool checkMatch(int* currentCombination, int* correctCombination, int n_Symbols);

	// Puzle 4: Quadre
	bool puz4_hasMovedFrame = false;
	bool puz4_hasPickedKey = false;
// FUNCIONES

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

};


