//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn bàsic VS2022 MONOFINESTRA amb OpenGL 4.3+, interfície GLFW, ImGui i llibreries GLM
//******** Ferran Poveda, Marc Vivet, Carme Julià, Débora Gil, Enric Martí (Setembre 2023)
// escena.h : interface de escena.cpp
//

#ifndef GLSOURCE_H
#define GLSOURCE_H

// Entorn VGI: OBJECTE OBJ. Include per la definició de l'objecte Obj_OBJ
#include "objLoader.h"	
#include "game.h"


/* ------------------------------------------------------------------------- */
/*                            Funcions de les escenes                        */
/* ------------------------------------------------------------------------- */
void dibuixa_Eixos(GLuint ax_programID, bool eix, GLuint axis_Id, CMask3D reixa, CPunt3D hreixa, glm::mat4 MatriuProjeccio, glm::mat4 MatriuVista);
void dibuixa_Skybox(GLuint sk_programID, GLuint cmTexture, char eix_Polar, glm::mat4 MatriuProjeccio, glm::mat4 MatriuVista);

// Entorn VGI: dibuixa_EscenaGL -> Dibuix de l'escena GL
void dibuixa_EscenaGL(GLuint sh_programID, bool eix, GLuint axis_Id, CMask3D reixa, CPunt3D hreixa, char objecte, CColor col_object, 
	bool sw_mat[5], 
	bool textur, GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, bool flagInvertY,
	int nptsU, CPunt3D PC_u[MAX_PATCH_CORBA], GLfloat pasCS, bool sw_PC, bool dib_TFrenet,
	COBJModel* objecteOBJ,
	glm::mat4 MatriuVista, glm::mat4 MatriuTG, GameState gameState,int gameScene, COBJModel modelos[NUM_MAX_MODELS]);

// Entorn VGI: dibuixa -> Funció que dibuixa objectes simples de la llibreria GLUT segons obj
void dibuixa(GLuint sh_programID, char obj, glm::mat4 MatriuVista, glm::mat4 MatriuTG);

// Entorn VGI: ABP
void dibuixaHabitacio(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, float width, float height, float depth);
void escenaABP_antigua(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS]);
void escenaABP(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GameState gameState);
void escenaABP2(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GameState gameState);
void escenaABP3(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GameState gameState);
void escenaDebug(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS]);

void escenaTextures(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS]);
void escenaPuzle1(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS]);
void escenaPuzle2(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS]);
void escenaPuzle3(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS]);
void escenaPuzle4(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS]);
void escenaPuzle5(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS]);
void dibuixa_Key(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5]);
void dibuixa_Candle(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5]);
void dibuixa_Note(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map);
//oid sea(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, CColor colorM);
CVAO loadSea_VAO(CColor colorM);


// Entorn VGI: Tie (Nau enemiga Star Wars) - Objecte fet per alumnes.
void tie(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4]);
void Alas(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4]);
void Motor(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4]);
void Canon(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4]);
void Cuerpo(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4]);
void Cabina(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4]);

// Entorn VGI: Funcions locals a escena.cpp
// CString2char: Funció de conversió de variable CString a char * per a noms de fitxers 
//char* CString2Char(CString entrada);

// ABP modelos
//COBJModel modeloTest;

#endif