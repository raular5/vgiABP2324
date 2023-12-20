//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn bàsic VS2022 MONOFINESTRA amb OpenGL 4.3+, interfície GLFW, ImGui i llibreries GLM
//******** Ferran Poveda, Marc Vivet, Carme Julià, Débora Gil, Enric Martí (Setembre 2023)
// escena.cpp : Aqui es on ha d'anar el codi de les funcions que 
//              dibuixin les escenes.
//
//    Versió 2.0:	- Objectes Cub, Esfera, Tetera (primitives libreria GLUT)
//
//	  Versió 2.2:	- Objectes Cub, Esfera, Tetera definides en fitxer font glut_geometry amb altres primitives GLUT
//
//	  Versió 2.5:	- Objectes cubRGB i Tie (nau Star Wars fet per alumnes)
//

#include "stdafx.h"
#include "material.h"
#include "visualitzacio.h"
#include "game.h"
#include "escena.h"

// Dibuixa Eixos Coordenades Món i Reixes, activant un shader propi.
void dibuixa_Eixos(GLuint ax_programID, bool eix, GLuint axis_Id, CMask3D reixa, CPunt3D hreixa, 
	glm::mat4 MatriuProjeccio, glm::mat4 MatriuVista)
{
// Visualització Eixos Coordenades Mòn
	glUseProgram(ax_programID);

// Pas Matrius Projecció i Vista Vista a shader
	glUniformMatrix4fv(glGetUniformLocation(ax_programID, "projectionMatrix"), 1, GL_FALSE, &MatriuProjeccio[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(ax_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);

// Attribute Locations must be setup before calling glLinkProgram()
	glBindAttribLocation(ax_programID, 0, "in_Vertex"); // Vèrtexs
	glBindAttribLocation(ax_programID, 1, "in_Color");	// Color

//  Dibuix dels eixos
	if (eix) draw_Eixos(axis_Id);

// Dibuixa el grid actiu
	if ((reixa.x) || (reixa.y) || (reixa.z) || (reixa.w)) draw_Grid(reixa, hreixa);
}

// Dibuixa Skybox en forma de Cub, activant un shader propi.
void dibuixa_Skybox(GLuint sk_programID, GLuint cmTexture, char eix_Polar, glm::mat4 MatriuProjeccio, glm::mat4 MatriuVista)
{
	glm::mat4 ModelMatrix(1.0);

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

// Activació shader per a cub skybox
	glUseProgram(sk_programID);

// Pas Matrius Projecció i Vista a shader
	glUniformMatrix4fv(glGetUniformLocation(sk_programID, "projectionMatrix"), 1, GL_FALSE, &MatriuProjeccio[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(sk_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);

// Rotar skyBox per a orientar sobre eix superior Z o X en Vista Esfèrica (POLARX, POLARY, POLARZ)
	if (eix_Polar == POLARZ) ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	else if (eix_Polar == POLARX) ModelMatrix = glm::rotate(ModelMatrix, radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));

// Escalar Cub Skybox a 5000 per encabir objectes escena a l'interior
	ModelMatrix = glm::scale(ModelMatrix, vec3(5000.0f, 5000.0f, 5000.0f));		//glScaled(5000.0, 5000.0, 5000.0);
	glUniformMatrix4fv(glGetUniformLocation(sk_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);

// Activar textura cubemaps del Skybox per encabir objectes escena
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cmTexture);

// Attribute Locations must be setup before calling glLinkProgram()
	glBindAttribLocation(sk_programID, 0, "in_Vertex"); // Vèrtexs

//  Dibuix del Skybox
	drawCubeSkybox();

	glDepthFunc(GL_LESS); // set depth function back to default
}


// dibuixa_EscenaGL: Dibuix de l'escena amb comandes GL
void dibuixa_EscenaGL(GLuint sh_programID, bool eix, GLuint axis_Id, CMask3D reixa, CPunt3D hreixa, char objecte, 
			CColor col_object, bool sw_mat[5],
			bool textur, GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, bool flagInvertY,
			int nptsU, CPunt3D PC_u[MAX_PATCH_CORBA], GLfloat pasCS, bool sw_PC, bool dib_TFrenet,
			COBJModel* objecteOBJ,
			glm::mat4 MatriuVista, glm::mat4 MatriuTG, GameState gameState, int gameScene, COBJModel modelos[NUM_MAX_MODELS])
{
	float altfar = 0;
	GLint npunts = 0, nvertexs = 0;
	int i, j, k;
	GLdouble tras[3] = { 0.0,0.0,0.0 }; //Sierpinski Sponge
	CColor color_vermell = { 0.0,0.0,0.0,1.0 }, color_Mar = { 0.0,0.0,0.0,0.0 };
	bool sw_material[5] = { 0.0,0.0,0.0,0.0,0.0 };
	
// Matrius de Transformació
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0), TransMatrix(1.0), ScaleMatrix(1.0), RotMatrix(1.0);

// VAO
	CVAO objectVAO = { 0,0,0,0,0 };
	objectVAO.vaoId = 0;	objectVAO.vboId = 0;	objectVAO.eboId = 0;	 objectVAO.nVertexs = 0; objectVAO.nIndices = 0;

	tras[0] = 0.0;	tras[1] = 0.0; tras[2] = 0.0;
	color_vermell.r = 1.0;	color_vermell.g = 0.0; color_vermell.b = 0.0; color_vermell.a = 1.0;
	sw_material[0] = false;	sw_material[1] = true; sw_material[2] = true; sw_material[3] = false;	sw_material[4] = true;

// Shader Visualització Objectes
	glUseProgram(sh_programID);

// Parametrització i activació/desactivació de textures
// ABP: IMPLEMENTADO EN LAS ESCENAS PARA CADA OBJETO!!!
/*
	if (texturID[0] != -1) SetTextureParameters(texturID[0], true, true, textur_map, false);
	if (textur) {	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
					glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
				}
		else {	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glDisable(GL_TEXTURE_2D);
				glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glDisable(GL_MODULATE);
			}
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), flagInvertY);
	*/

// Attribute Locations must be setup before calling glLinkProgram()
	glBindAttribLocation(sh_programID, 0, "in_Vertex");		// Vèrtexs
	glBindAttribLocation(sh_programID, 1, "in_Normal");		// Normals
	glBindAttribLocation(sh_programID, 2, "in_TexCoord");	// Textura
	glBindAttribLocation(sh_programID, 3, "in_Color");		// Color

// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

	// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color de l'objecte.
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	switch (gameScene) {
	case 1:
		escenaABP (sh_programID, MatriuVista, MatriuTG, sw_mat, gameState);
		break;
	case SCENE_GAME:
		escenaABP_antigua(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	case SCENE_ITEM_INSPECT:
		escenaItemInspect(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	case 3:
		escenaABP3(sh_programID, MatriuVista, MatriuTG, sw_mat, gameState);
		break;
	case 10:
		escenaDebug(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	case 11:
		escenaTextures(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	case SCENE_PUZLE1:
		escenaPuzle1(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	case SCENE_PUZLE2:
		escenaPuzle2(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);;
		break;
	case SCENE_PUZLE3:
		escenaPuzle3(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	case SCENE_PUZLE4:
		escenaPuzle4(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	case SCENE_WIN:
		escenaPuzle5(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	case SCENE_PUZLE6:
		escenaPuzle6(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, gameState, modelos);
		break;
	default:
		escenaABP(sh_programID, MatriuVista, MatriuTG, sw_mat, gameState);
		break;
	}

}


// dibuixa: Funció que dibuixa objectes simples de la llibreria GLUT segons obj
void dibuixa(GLuint sh_programID, char obj, glm::mat4 MatriuVista, glm::mat4 MatriuTG)
{
//	std::string String;
//	const char* string;
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0); // TransMatrix(1.0); // ScaleMatrix(1.0), RotMatrix(1.0);
	GLdouble tras[3] = { 0.0,0.0,0.0 }; //Sierpinski Sponge
	
	tras[0] = 0.0;	tras[1] = 0.0; tras[2] = 0.0;

	switch(obj)
	{

// Cub
	case CUB:
		//glPushMatrix();
		  //glScaled(5.0,5.0,5.0);
		ModelMatrix = glm::scale(MatriuTG, vec3(5.0f, 5.0f, 5.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLUT_CUBE);	//draw_TriVAO_Object(GLUT_CUBE);  //glutSolidCube(1.0);
		//glPopMatrix();

		break;

	case CUB_RGB:
		//glPushMatrix();
		  //glScaled(5.0,5.0,5.0);
		ModelMatrix = glm::scale(MatriuTG, vec3(5.0f, 5.0f, 5.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLUT_CUBE_RGB);	//draw_TriVAO_Object(GLUT_CUBE_RGB); // glutSolidCubeRGB(1.0);
		//glPopMatrix();
		break;

// Esfera
	case ESFERA:	
		//glPushMatrix();
		  //glScaled(5.0,5.0,5.0);
		ModelMatrix = glm::scale(MatriuTG, vec3(5.0f, 5.0f, 5.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLU_SPHERE); //draw_TriVAO_Object(GLU_SPHERE); //gluSphere(1.0, 40, 40);
		//glPopMatrix();
		break;

// Tetera
	case TETERA:
		//glPushMatrix();
		  //glScaled(5.0,5.0,5.0);
		  ModelMatrix = MatriuTG;
		  // Pas ModelView Matrix a shader
		  glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		  // Pas NormalMatrix a shader
		  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		  glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		  draw_TriVAO_Object(GLUT_TEAPOT); // glutSolidTeapot(1.0);
		//glPopMatrix();
		break;
	}
}

// Menu principal
void escenaABP(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GameState gameState)
{
	CColor col_object = { 1.0,1.0,0.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

	// cabeza
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 2.5f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.25f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	
	//modeloTest.draw_TriVAO_OBJ(sh_programID);

}

// Habitacion
void escenaABP_antigua(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{

	// Draw boundaries
	// debugDrawBoundaries(sh_programID, MatriuVista, MatriuTG, sw_mat, gameState);


	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	ModelMatrix = MatriuTG;
	//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(10.0f, 0.0f, -2.0f));
	//ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.5f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[39].draw_TriVAO_OBJ(sh_programID);


	if (gameState.drawParetFalsa)
	{
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
		ModelMatrix = MatriuTG;
		//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::translate(MatriuTG, vec3(-7.0f, 0.0f, -1.9f));
		ModelMatrix = glm::scale(ModelMatrix, vec3(0.9f, 0.9f, 0.9f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 0.0f, 1.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(270.0f), vec3(0.0f, 0.0f, 1.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[40].draw_TriVAO_OBJ(sh_programID);
	}

	if (!gameState.puz1_complete)
	{
		// CANDADO SIMBOLOS
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
		CColor col_object = { 0.0f, 0.0f, 0.0f, 0.0f };
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::translate(MatriuTG, vec3(13.0f, 7.8f, 0.13f));

		ModelMatrix = glm::rotate(ModelMatrix, radians(270.0f), vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));

		ModelMatrix = glm::scale(ModelMatrix, vec3(0.017f, 0.017f, 0.017f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[111].draw_TriVAO_OBJ(sh_programID);

		// MALETIN 1
		glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
		ModelMatrix = MatriuTG;
		//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::translate(MatriuTG, vec3(13.0f, 7.8f, -0.5f));

		ModelMatrix = glm::scale(ModelMatrix, vec3(0.3f, 0.3f, 0.3f));

		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.5f, 0.0f, 0.0f));

		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[46].draw_TriVAO_OBJ(sh_programID);
	}

	if (gameState.puz1_complete)
	{
		// MALETIN 2
		glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
		ModelMatrix = MatriuTG;
		//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::translate(MatriuTG, vec3(13.0f, 7.8f, -0.5f));

		ModelMatrix = glm::scale(ModelMatrix, vec3(0.3f, 0.3f, 0.3f));

		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.5f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.5f, 0.0f));

		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[47].draw_TriVAO_OBJ(sh_programID);
	}

	if (gameState.puz2_complete)
	{
		// GEMA
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
		ModelMatrix = MatriuTG;
		//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::translate(MatriuTG, vec3(10.0f, 0.0f, -2.0f));
		ModelMatrix = glm::translate(ModelMatrix, vec3(-5.04636f, -7.97467f, 3.52124f));
		//ModelMatrix = glm::scale(ModelMatrix, vec3(0.59f, 0.59f, 0.59f));
		ModelMatrix = glm::scale(ModelMatrix, vec3(0.1f, 0.1f, 0.1f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.5f, 0.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[10].draw_TriVAO_OBJ(sh_programID);

		// posters
		glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
		ModelMatrix = MatriuTG;
		//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::translate(MatriuTG, vec3(10.0f, 0.0f, -2.0f));
		//ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.5f, 0.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[45].draw_TriVAO_OBJ(sh_programID);
	}
	
	if (!gameState.puz3_complete)
	{
		CColor col_object = { 1.0f, 1.0f, 1.0f, 1.0f };

		// CANDADO NUMEROS
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::translate(MatriuTG, vec3(2.2f, -0.6f, -0.55f));

		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));

		ModelMatrix = glm::scale(ModelMatrix, vec3(0.1f, 0.1f, 0.1f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[11].draw_TriVAO_OBJ(sh_programID);

		// MANIVELA
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo

		SetTextureParameters(texturID[101], true, false, textur_map, false);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

		ModelMatrix = MatriuTG;
		ModelMatrix = glm::translate(MatriuTG, vec3(1.9f, -0.4f, -0.5f));

		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(270.0f), vec3(0.0f, 0.0f, 1.0f));

		ModelMatrix = glm::scale(ModelMatrix, vec3(0.03f, 0.03f, 0.03f));

		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[41].draw_TriVAO_OBJ(sh_programID);

		// CUBO
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo

		SetTextureParameters(texturID[49], true, false, textur_map, false);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

		ModelMatrix = MatriuTG;
		ModelMatrix = glm::translate(MatriuTG, vec3(1.9f, -0.6f, -0.5f));
		ModelMatrix = glm::scale(ModelMatrix, vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLUT_CUBE);
	}
}

/*
void escenaABP_antigua(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{	CColor col_object = { 1.0,1.0,1.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

	int width = 80.0f;
	int heigth = 80.0f;
	int depth = 12.0f;

	// Dibuja paredes, techo, suelo
	dibuixaHabitacio(sh_programID, MatriuVista, MatriuTG, sw_mat, texturID, textur_map, width, heigth, depth);

	
	// TABLE
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-30.0f, 0.0f, -6.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_GOTHIC_DINNERTABLE].draw_TriVAO_OBJ(sh_programID);

	// DRESSER
	//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	//glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-30.0f, -10.0f, -6.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_GOTHIC_DRESSER].draw_TriVAO_OBJ(sh_programID);

	

	// CUADRO
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-30.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_SPOOKY_PORTRAIT].draw_TriVAO_OBJ(sh_programID);
	

	//// CANDLE
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-30.0f, -15.0f, -6.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_GOTHIC_CANDLESTICK].draw_TriVAO_OBJ(sh_programID);

	// BOOK
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-30.0f, 0.0f, -2.5f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_SPOOKY_BOOK1].draw_TriVAO_OBJ(sh_programID);

	//// CANDLE
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-30.0f, 10.0f, -6.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_SPOOKY_CANDLE].draw_TriVAO_OBJ(sh_programID);

	//// CABINET
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-30.0f, -20.0f, -6.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_SPOOKY_CABINET].draw_TriVAO_OBJ(sh_programID);

	//// RUG
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-15.0f, 0.0f, -5.9f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_SPOOKY_RUG].draw_TriVAO_OBJ(sh_programID);

	//// PIANO
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-30.0f, 20.0f, -6.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_SPOOKY_PIANO].draw_TriVAO_OBJ(sh_programID);

	//// BED
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, -20.0f, -6.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[MODEL_GOTHIC_BED].draw_TriVAO_OBJ(sh_programID);

	// Item inspect
	if (gameState.showItemInspector)
	{
		ModelMatrix = glm::scale(MatriuTG, gameState.item_inspect_scale);
		ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(gameState.item_inspect_rotation.x), vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(gameState.item_inspect_rotation.y), vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(gameState.item_inspect_rotation.z), vec3(0.0f, 0.0f, 1.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		//draw_TriEBO_Object(GLUT_CUBE); // drawSolidCube();	//  glutSolidCube(1.0);

		switch (gameState.currentItem) {
		case ITEM_NONE:
			//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE);
			// Set texture
			//SetTextureParameters(texturID[20], true, true, textur_map, true);
			//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
			//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
			//glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
			modelos[10].draw_TriVAO_OBJ(sh_programID);
			//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
			//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
			break;
		case ITEM_KEY:
			dibuixa_Key(sh_programID, MatriuVista, ModelMatrix, sw_mat);
			break;
		case ITEM_NOTE:
			dibuixa_Note(sh_programID, MatriuVista, ModelMatrix, sw_mat, texturID, textur_map);
			break;
		case ITEM_CANDLE:
			dibuixa_Candle(sh_programID, MatriuVista, ModelMatrix, sw_mat);
			break;
		}
	}


}
*/

void escenaABP2(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GameState gameState)
{
	CColor col_object = { 0.0,0.0,0.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	
	//pata 1
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 0.0f, 2.5f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(1.0f, 1.0f, 10.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);
	

}

void escenaABP3(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GameState gameState)
{
	CColor col_object = { 0.0,0.0,0.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

	//pata 1
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 0.0f, 2.5f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(1.0f, 1.0f, 10.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE); // drawSolidCube();	//  glutSolidCube(1.0);
	//glPopMatrix();

	//pata 2
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 5.0f, 2.5f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(1.0f, 1.0f, 10.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE); //drawSolidCube();	//  glutSolidCube(1.0);



}

void escenaDebug(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{
	CColor col_object = { 0.0,0.0,0.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	// Cubo 1
	SeleccionaColorMaterial(sh_programID, gameState.cube_color, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(0,0,0));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	// Cubo 2
	SeleccionaColorMaterial(sh_programID, gameState.cube_color, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(0, -4, 0));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	// Cubo 1
	SeleccionaColorMaterial(sh_programID, gameState.cube_color, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(0, 4, 0));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	// Suelo
	col_object.r = 0.0f;
	col_object.g = 0.0f;
	col_object.b = 0.0f;
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);


	ModelMatrix = glm::scale(MatriuTG, vec3(10.0f, 10.0f, 0.25f));
	ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 0.0f, -5.0f));
	
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

}

void escenaTextures(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{
	CColor col_object = { 1.0,1.0,1.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	for (int i = 0; i < 0; i++) {
		SetTextureParameters(texturID[i], true, true, textur_map, true);
		
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::scale(MatriuTG, vec3(1.0f, 1.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, -5.0f + (1.0f * i), 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLUT_CUBE);

		//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
		//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
	}

	// Draw test models
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	for (int i = 0; i < 10; i++) {
		
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::scale(MatriuTG, vec3(1.0f, 1.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, -5.0f + (3.0f * i), 2.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);

		modelos[i].draw_TriVAO_OBJ(sh_programID);

		//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
		//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
	}


}

// CADENAT AMB NUMEROS
void escenaPuzle1(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{
	CColor col_object = { 1.0,1.0,1.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

	// BLOCS (TODO: LOOP)

	SetTextureParameters(texturID[7 + gameState.puz1_currentCombination[0]], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	
	ModelMatrix = glm::translate(MatriuTG, vec3(9.0f, -0.32f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.2f, 0.2f, 0.2f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	SetTextureParameters(texturID[7 + gameState.puz1_currentCombination[1]], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	ModelMatrix = glm::translate(MatriuTG, vec3(9.0f, -0.11f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.2f, 0.2f, 0.2f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);



	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	SetTextureParameters(texturID[7 + gameState.puz1_currentCombination[2]], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	ModelMatrix = glm::translate(MatriuTG, vec3(9.0f, 0.099f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.2f, 0.2f, 0.2f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	SetTextureParameters(texturID[7 + gameState.puz1_currentCombination[3]], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	ModelMatrix = glm::translate(MatriuTG, vec3(9.0f, 0.307f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.2f, 0.2f, 0.2f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

		//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
		//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);

	// MALETIN
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	ModelMatrix = MatriuTG;
	//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(8.5f, 0.1f, -0.6f));

	ModelMatrix = glm::scale(ModelMatrix, vec3(0.3f, 0.3f, 0.3f));

	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.5f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[46].draw_TriVAO_OBJ(sh_programID);

	// CADENAT
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	col_object = { 0.0f, 1.0f, 0.0f, 1.0f };
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(9.0f, -0.35f, -0.25f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.1f, 0.1f, 0.1f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[111].draw_TriVAO_OBJ(sh_programID);

	// FONS
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	ModelMatrix = MatriuTG;
	//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(16.5f, -2.9f, -2.1f));
	//ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[39].draw_TriVAO_OBJ(sh_programID);

}

// JOYA EN ESTATUA
void escenaPuzle2(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{
	CColor col_object = { 0.0,0.0,0.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);


	if (gameState.puz2_complete)
	{
		// GEMA
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
		ModelMatrix = MatriuTG;
		//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 0.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, vec3(1.0f, 1.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
		//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		modelos[10].draw_TriVAO_OBJ(sh_programID);
	}
	

	// FONDO HABITACION
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	ModelMatrix = MatriuTG;
	//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(15.0f, -5.2f, -3.0f));
	//ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(270.0f), vec3(0.0f, 1.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[39].draw_TriVAO_OBJ(sh_programID);
}

// CANDADO SÍMBOLOS
void escenaPuzle3(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{
	CColor col_object = { 1.0,1.0,1.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

	SetTextureParameters(texturID[30 + gameState.puz3_currentCombination[0]], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::scale(MatriuTG, vec3(1.0f, 1.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, -1.5f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	SetTextureParameters(texturID[30 + gameState.puz3_currentCombination[1]], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	ModelMatrix = glm::scale(MatriuTG, vec3(1.0f, 1.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, -0.5f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);



	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	SetTextureParameters(texturID[30 + gameState.puz3_currentCombination[2]], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	ModelMatrix = glm::scale(MatriuTG, vec3(1.0f, 1.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 0.5f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	SetTextureParameters(texturID[30 + gameState.puz3_currentCombination[3]], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	ModelMatrix = glm::scale(MatriuTG, vec3(1.0f, 1.0f, 1.0f));
	ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 1.5f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	// CADENAT
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	col_object = { 0.0f, 1.0f, 0.0f, 1.0f };
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-5.0f, 0.0f, -2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.5f, 2.5f, 2.5f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[11].draw_TriVAO_OBJ(sh_programID);

	// PARET
	col_object = { 1.0f, 1.0f, 1.0f, 1.0f };
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SetTextureParameters(texturID[49], true, false, textur_map, false);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = MatriuTG;
	ModelMatrix = glm::translate(MatriuTG, vec3(-10.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.1f, 60.0f, 30.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

}

// CUADRO CON LLAVE
void escenaPuzle4(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{
	CColor col_object = { 1.0,1.0,1.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	//SetTextureParameters(texturID[40], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = MatriuTG;
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	if (gameState.puz4_hasMovedFrame)
	{
		ModelMatrix = glm::translate(ModelMatrix, vec3(-2.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	}
		
		
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.0f, 2.0f, 2.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	//draw_TriEBO_Object(GLUT_CUBE);
	//modelos[12].draw_TriVAO_OBJ(sh_programID);
	modelos[MODEL_SPOOKY_PORTRAIT].draw_TriVAO_OBJ(sh_programID);
	

	// Key
	if (gameState.puz4_hasMovedFrame && !gameState.puz4_hasPickedKey)
	{
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo

		SetTextureParameters(texturID[102], true, false, textur_map, false);
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

		ModelMatrix = MatriuTG;

		ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 0.5f, -2.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, vec3(0.2f, 0.2f, 0.2f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		//dibuixa_Key(sh_programID, MatriuVista, ModelMatrix, sw_mat);
		modelos[42].draw_TriVAO_OBJ(sh_programID);
	}

	// PARET
	col_object = { 1.0f, 1.0f, 1.0f, 1.0f };
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	SetTextureParameters(texturID[49], true, false, textur_map, false);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = MatriuTG;
	ModelMatrix = glm::translate(MatriuTG, vec3(-10.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.1f, 60.0f, 30.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);
	

}

void escenaPuzle5(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{

}

// MANIVELA
void escenaPuzle6(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{
	CColor col_object = { 1.0,1.0,1.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo

	SetTextureParameters(texturID[101], true, false, textur_map, false);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

	ModelMatrix = MatriuTG;
	//ModelMatrix = MatriuTG;
	ModelMatrix = glm::rotate(MatriuTG, gameState.puz6_rotation, vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(270.0f), vec3(0.0f, 0.0f, 1.0f));
	//ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 2.5f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.5f, 0.5f, 0.5f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	modelos[41].draw_TriVAO_OBJ(sh_programID);

	////glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	////glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	////glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	//SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	//ModelMatrix = glm::scale(MatriuTG, vec3(2.0f, 1.0f, 1.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, gameState.puz6_rotation, vec3(1.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 5.0f, 0.0f));
	//// Pas ModelView Matrix a shader
	//glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	//// Pas NormalMatrix a shader
	//NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	//glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	//draw_TriEBO_Object(GLUT_CUBE);

	// PARET
	SetTextureParameters(texturID[49], true, false, textur_map, false);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	ModelMatrix = glm::translate(MatriuTG, vec3(-1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.1f, 25.0f, 20.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);
}

void escenaItemInspect(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, GameState gameState, COBJModel modelos[NUM_MAX_MODELS])
{
	CColor col_object = { 1.0,1.0,1.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	ModelMatrix = glm::scale(MatriuTG, gameState.item_inspect_scale);
	ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(gameState.item_inspect_rotation.x), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(gameState.item_inspect_rotation.y), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(gameState.item_inspect_rotation.z), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	//draw_TriEBO_Object(GLUT_CUBE); // drawSolidCube();	//  glutSolidCube(1.0);
	modelos[gameState.item_inspect_currentModel].draw_TriVAO_OBJ(sh_programID);

	//switch (gameState.item_inspectcurrentItem) {
	//case ITEM_NONE:
	//	//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE);
	//	// Set texture
	//	//SetTextureParameters(texturID[20], true, true, textur_map, true);
	//	//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	//	//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	//	//glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_FALSE);	// La textura esta en espejo
	//	modelos[10].draw_TriVAO_OBJ(sh_programID);
	//	//glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
	//	//glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
	//	break;
	//case ITEM_KEY:
	//	dibuixa_Key(sh_programID, MatriuVista, ModelMatrix, sw_mat);
	//	break;
	//case ITEM_NOTE:
	//	dibuixa_Note(sh_programID, MatriuVista, ModelMatrix, sw_mat, texturID, textur_map);
	//	break;
	//case ITEM_CANDLE:
	//	dibuixa_Candle(sh_programID, MatriuVista, ModelMatrix, sw_mat);
	//	break;
	//}
}

void dibuixa_Key(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5])
{
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
	CColor col_object = { 1.0,1.0,0.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	

	// cabeza
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 2.5f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.25f, 2.0f, 2.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	// palo
	ModelMatrix = glm::scale(MatriuTG, vec3(0.25f, 5.0f, 0.25f));
	ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	// punta
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, -2.25f, -0.5f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.25f, 0.25f, 0.75f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	// punta 2
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, -1.5f, -0.5f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.25f, 0.25f, 0.75f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);
}

void dibuixaHabitacio(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, float width, float height, float depth)
{
	CColor col_object = { 1.0, 1.0, 1.0, 1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);



	glm::vec3 dimensions[6] = {
		glm::vec3(width, 0.1f, depth),  // Pared izq 
		glm::vec3(width, 0.1f, depth),  // pared derecha
		glm::vec3(width, height, 0.1f), // techo
		glm::vec3(width, height, 0.1f), // suelo
		glm::vec3(0.1f, height, depth), // pared delante
		glm::vec3(0.1f, height, depth)  // pared detras
	};


	glm::vec3 positions[6] = {
		glm::vec3(0, -height / 2, 0),      // Pared izq 
		glm::vec3(0, height / 2, 0),       // pared derecha
		glm::vec3(0, 0, depth / 2),        // techo
		glm::vec3(0, 0, -depth / 2),       // suelo
		glm::vec3(-width / 2, 0, 0),       // pared delante
		glm::vec3(width / 2, 0, 0)         // pared detras
	};

	for (int i = 0; i < 6; ++i) 
	{
		ModelMatrix = glm::translate(MatriuTG, positions[i]);
		ModelMatrix = glm::scale(ModelMatrix, dimensions[i]);

		GLuint currentTextureID;
		if (i == 3) { // Suelo
			currentTextureID = texturID[52];
		}
		else if (i == 2) 
		{ // Techo
			currentTextureID = texturID[51];
		}
		else { // Paredes
			
			currentTextureID = texturID[50];
		}

		
		SetTextureParameters(currentTextureID, true, false, textur_map, false);

		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE);
		glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);
		

		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);

		NormalMatrix = glm::transpose(glm::inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);

		draw_TriEBO_Object(GLUT_CUBE);

		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE);
	}
}



void dibuixa_Note(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GLuint texturID[NUM_MAX_TEXTURES], bool textur_map)
{
	CColor col_object = { 1.0,1.0,1.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.1f, 3.0f, 4.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	// Set texture
	SetTextureParameters(texturID[0], true, true, textur_map, true);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), GL_TRUE);	// La textura esta en espejo
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);

	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);

	
}

void dibuixa_Candle(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5])
{
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
	CColor col_object = { 1.0,1.0,0.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

	// cabeza
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.25f, 0.25f, 0.25f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLU_DISK);

}

void debugDrawBoundaries(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5], GameState gameState)
{
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glEnable(GL_MODULATE);
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

	CColor col_object = { 1.0,1.0,0.0,1.0 };
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	for (ObjectBoundaries b : gameState.boundaries)
	{
		
		ModelMatrix = MatriuTG;
		ModelMatrix = glm::translate(ModelMatrix, b.position);
		ModelMatrix = glm::scale(ModelMatrix, vec3(b.radius, b.radius, b.radius));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLU_SPHERE);

	}

	
}

// Mar amb ondulacions
CVAO loadSea_VAO(CColor colorM)
{
	int i, j, step, it1, it2;
	double angle, delay, Nx;
	glm::mat4 ModelMatrix(1.0), NormalMatrix(1.0);

// VAO
	GLuint vaoId = 0; GLuint vboId = 0;
	CVAO seaVAO = { 0,0,0,0,0 };
	seaVAO.vaoId = 0;	seaVAO.vboId = 0;	seaVAO.nVertexs = 0;	seaVAO.eboId = 0; seaVAO.nIndices = 0;
	std::vector <double> vertices, normals, colors, textures;	// Definició vectors dinàmics per a vertexs, normals i textures 
	vertices.resize(0);		normals.resize(0);		colors.resize(0);	textures.resize(0);			// Reinicialitzar vectors

// Aigua amb ondulacions simulades a partir de normals sinusoidals
	step = 10; //step = 5;
	delay = 0;
	it1 = 0;

//   SeleccionaMaterial(MAT_DEFAULT);	
	float h = 2 * PI*step / 500;
	for (j = -250; j<250 - step; j = j + step)
	{	delay = 1.0*h*it1;

		it2 = 0;
		//glColor3f(0.5f, 0.4f, 0.9f);
		for (i = -250; i<250 - step; i = i + step)
		{	//glBegin(GL_POLYGON);
			  angle = 1.0*it2*h * 15;
			  Nx = -cos(angle);
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i, j, 0);			// Vèrtex P1
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(Nx);			normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(0.0);			textures.push_back(0.0);										// Vector Textures
			  vertices.push_back(i);			vertices.push_back(j);			vertices.push_back(0.0);		// Vector Vertices

			  angle = 1.0*(it2 + 1.0)*h * 15;
			  Nx = -cos(angle);
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i + step, j, 0);		// Vèrtex P2
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(Nx);			normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(1.0);			textures.push_back(0.0);										// Vector Textures
			  vertices.push_back(i+step);		vertices.push_back(j);			vertices.push_back(0.0);		// Vector Vertices

			  angle = 1.0*(it2 + 1.0)*h * 15;
			  Nx = -cos(angle);
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i + step, j + step, 0);// Vèrtex P3
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(Nx);			normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(1.0);			textures.push_back(1.0);										// Vector Textures
			  vertices.push_back(i+step);		vertices.push_back(j+step);		vertices.push_back(0.0);		// Vector Vertices

			//glEnd();
// ----------------------- VAO
			//std::vector <int>::size_type nv = vertices.size();	// Tamany del vector vertices en elements.
			//draw_GL_TRIANGLES_VAO(vertices, normals, colors, textures);
			//vertices.resize(0);	normals.resize(0);	colors.resize(0);	textures.resize(0);	// Reinicialitzar vectors

			//glBegin(GL_POLYGON);
			  angle = 1.0*it2*h * 15;
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i, j, 0);			// Vèrtex P1
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(-cos(angle));	normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(1.0);			textures.push_back(1.0);										// Vector Textures
			  vertices.push_back(i);			vertices.push_back(j);			vertices.push_back(0.0);		// Vector Vertices

			  angle = 1.0*(it2 + 1)*h * 15;
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i + step, j + step, 0);// Vèrtex P2
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(-cos(angle));	normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(1.0);			textures.push_back(1.0);										// Vector Textures
			  vertices.push_back(i + step);		vertices.push_back(j + step);	vertices.push_back(0.0);		// Vector Vertices

			  angle = 1.0*it2*h * 15;
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i, j + step, 0);		// Vèrtex P3
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(-cos(angle));	normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(0.0);			textures.push_back(1.0);										// Vector Textures
			  vertices.push_back(i);			vertices.push_back(j + step);		vertices.push_back(0.0);	// Vector Vertices

			//glEnd();

			it2++;
		}
		it1++;
	}
// ----------------------- VAO
	std::vector <int>::size_type nv = vertices.size();	// Tamany del vector vertices en elements.
	//draw_GL_TRIANGLES_VAO(vertices, normals, colors, textures);
	// 
// Creació d'un VAO i un VBO i càrrega de la geometria. Guardar identificador VAO identificador VBO a struct CVAO.
	seaVAO = load_TRIANGLES_VAO(vertices, normals, colors, textures);
	//seaVAO.vaoId = vaoId;
	//seaVAO.vboId = vboId;

	seaVAO.nVertexs = (int)nv / 3;
	return seaVAO;
}