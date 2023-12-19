//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn bàsic VS2022 MONOFINESTRA amb OpenGL 4.3+, interfície GLFW, ImGui i llibreries GLM
//******** Ferran Poveda, MABP_CUSTOM Vivet, Carme Julià, Débora Gil, Enric Martí (Setembre 2023)
// main.cpp : Definició de main
//    Versió 0.5:	- Interficie ImGui
//					- Per a dialeg de cerca de fitxers, s'utilitza la llibreria NativeFileDialog


// Entorn VGI.ImGui: Includes llibreria ImGui
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_glfw.h"
#include "ImGui\imgui_impl_opengl3.h"
#include "ImGui\nfd.h" // Native File Dialog


#include "stdafx.h"
#include "shader.h"
#include <algorithm>
#include "visualitzacio.h"
#include "escena.h"
#include "main.h"

#include <tuple>

#include <chrono>
#include <thread>

void InitGL()
{
	// TODO: agregar aquí el código de construcción
	gameState.gameScene = &gameScene;
	//------ Entorn VGI: Inicialització de les variables globals de CEntornVGIView
	int i;

	// Entorn VGI: Variable de control per a Status Bar (consola) 
	statusB = false;

	// Entorn VGI: Variables de control per Menú Càmera: Esfèrica, Navega, Mòbil, Zoom, Satelit, Polars... 
	camera = CAM_NAVEGA;
	//camera = CAM_ESFERICA;
	mobil = true;	zzoom = true;		zzoomO = false;		satelit = false;

	// Entorn VGI: Variables de control de l'opció Càmera->Navega?
	n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
	opvN.x = 10.0;	opvN.y = 0.0;		opvN.z = 0.0;
	angleZ = 0.0;
	ViewMatrix = glm::mat4(1.0);		// Inicialitzar a identitat
	gameState.n = n;
	gameState.opvN = &opvN;
	gameState.angleZ = &angleZ;
	gameState.m_ViewMatrix = &ViewMatrix;

	// Entorn VGI: Variables de control de l'opció Càmera->Geode?
	OPV_G.R = 15.0;		OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;	// Origen PV en esfèriques per a Vista_Geode

	// Entorn VGI: Variables de control per Menú Vista: Pantalla Completa, Pan, dibuixar eixos i grids 
	fullscreen = false;
	pan = false;
	eixos = true;	eixos_programID = 0;  eixos_Id = 0;
	sw_grid = false;
	grid.x = false;	grid.y = false;		grid.z = false;		grid.w = false;
	hgrid.x = 0.0;	hgrid.y = 0.0;		hgrid.z = 0.0;		hgrid.w = 0.0;

	// Entorn VGI: Variables opció Vista->Pan
	fact_pan = 1;
	tr_cpv.x = 0;	tr_cpv.y = 0;	tr_cpv.z = 0;		tr_cpvF.x = 0;	tr_cpvF.y = 0;	tr_cpvF.z = 0;

	// Entorn VGI: Variables de control per les opcions de menú Projecció, Objecte
	projeccio = PERSPECT;	// projeccio = PERSPECT;
	ProjectionMatrix = glm::mat4(1.0);	// Inicialitzar a identitat
	gameState.m_ProjectionMatrix = &ProjectionMatrix;
	objecte = ABP_CUSTOM;		// objecte = TETERA;

	// Entorn VGI: Variables de control Skybox Cube
	SkyBox = 0;		skC_programID = 0;
	skC_VAOID.vaoId = 0;	skC_VAOID.vboId = 0;	skC_VAOID.nVertexs = 0;
	cubemapTexture[0] = 0;

	// Entorn VGI: Variables de control del menú Transforma
	transf = false;		trasl = false;		rota = false;		escal = false;
	fact_Tras = 1;		fact_Rota = 90;
	TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0;	TGF.VTras.x = 0.0;	TGF.VTras.y = 0.0;	TGF.VTras.z = 0;
	TG.VRota.x = 0;		TG.VRota.y = 0;		TG.VRota.z = 0;	TGF.VRota.x = 0;	TGF.VRota.y = 0;	TGF.VRota.z = 0;
	TG.VScal.x = 1;		TG.VScal.y = 1;		TG.VScal.z = 1;	TGF.VScal.x = 1;	TGF.VScal.y = 1;	TGF.VScal.z = 1;

	transX = false;		transY = false;		transZ = false;
	GTMatrix = glm::mat4(1.0);		// Inicialitzar a identitat

	// Entorn VGI: Variables de control per les opcions de menú Ocultacions
	front_faces = true;	test_vis = false;	oculta = true;		back_line = false;

	// Entorn VGI: Variables de control del menú Iluminació		
	ilumina = SUAU;			ifixe = false;					ilum2sides = false;
	// Reflexions actives: Ambient [1], Difusa [2] i Especular [3]. No actives: Emission [0]. 
	sw_material[0] = false;			sw_material[1] = true;			sw_material[2] = true;			sw_material[3] = true;	sw_material[4] = true;
	sw_material_old[0] = false;		sw_material_old[1] = true;		sw_material_old[2] = true;		sw_material_old[3] = true;	sw_material_old[4] = true;
	textura = false;				t_textura = CAP;				textura_map = true;
	for (i = 0; i < NUM_MAX_TEXTURES; i++) texturesID[i] = -1;
	tFlag_invert_Y = false;

	// Entorn VGI: Variables de control del menú Llums
	// Entorn VGI: Inicialització variables Llums
	llum_ambient = true;
	for (i = 1; i < NUM_MAX_LLUMS; i++) llumGL[i].encesa = false;
	for (i = 0; i < NUM_MAX_LLUMS; i++) {
		llumGL[i].encesa = false;
		llumGL[i].difusa[0] = 1.0f;	llumGL[i].difusa[1] = 1.0f;	llumGL[i].difusa[2] = 1.0f;	llumGL[i].difusa[3] = 1.0f;
		llumGL[i].especular[0] = 1.0f; llumGL[i].especular[1] = 1.0f; llumGL[i].especular[2] = 1.0f; llumGL[i].especular[3] = 1.0f;
	}

	// LLum 0: Atenuació constant (c=1), sobre l'eix Z, no restringida.
	llumGL[0].encesa = true;
	llumGL[0].difusa[0] = 1.0f;			llumGL[0].difusa[1] = 1.0f;			llumGL[0].difusa[2] = 1.0f;		llumGL[0].difusa[3] = 1.0f;
	llumGL[0].especular[0] = 1.0f;		llumGL[0].especular[1] = 1.0f;		llumGL[0].especular[2] = 1.0f;	llumGL[0].especular[3] = 1.0f;

	llumGL[0].posicio.R = 200.0;		llumGL[0].posicio.alfa = 90.0;		llumGL[0].posicio.beta = 0.0;		// Posició llum (x,y,z)=(0,0,200)
	llumGL[0].atenuacio.a = 0.0;		llumGL[0].atenuacio.b = 0.0;		llumGL[0].atenuacio.c = 1.0;		// Llum sense atenuació per distància (a,b,c)=(0,0,1)
	llumGL[0].restringida = false;
	llumGL[0].spotdirection[0] = 0.0;	llumGL[0].spotdirection[1] = 0.0;	llumGL[0].spotdirection[2] = 0.0;
	llumGL[0].spotcoscutoff = 0.0;		llumGL[0].spotexponent = 0.0;

	// LLum 1: Atenuació constant (c=1), sobre l'eix X, no restringida.
	llumGL[1].encesa = false;
	llumGL[1].difusa[0] = 1.0f;			llumGL[1].difusa[1] = 1.0f;			llumGL[1].difusa[2] = 1.0f;		llumGL[1].difusa[3] = 1.0f;
	llumGL[1].especular[0] = 1.0f;		llumGL[1].especular[1] = 1.0f;		llumGL[1].especular[2] = 1.0f;	llumGL[1].especular[3] = 1;

	llumGL[1].posicio.R = 75.0;			llumGL[1].posicio.alfa = 0.0;		llumGL[1].posicio.beta = 0.0;// (x,y,z)=(75,0,0)
	llumGL[1].atenuacio.a = 0.0;		llumGL[1].atenuacio.b = 0.0;		llumGL[1].atenuacio.c = 1.0;
	llumGL[1].restringida = false;
	llumGL[1].spotdirection[0] = 0.0;	llumGL[1].spotdirection[1] = 0.0;	llumGL[1].spotdirection[2] = 0.0;
	llumGL[1].spotcoscutoff = 0.0;		llumGL[1].spotexponent = 0.0;

	// LLum 2: Atenuació constant (c=1), sobre l'eix Y, no restringida.
	llumGL[2].encesa = false;
	llumGL[2].difusa[1] = 1.0f;			llumGL[2].difusa[1] = 1.0f;			llumGL[2].difusa[2] = 1.0f;		llumGL[2].difusa[3] = 1.0f;
	llumGL[2].especular[1] = 1.0f;		llumGL[2].especular[1] = 1.0f;		llumGL[2].especular[2] = 1.0f;	llumGL[2].especular[3] = 1;

	llumGL[2].posicio.R = 75.0;			llumGL[2].posicio.alfa = 0.0;		llumGL[2].posicio.beta = 90.0;	// (x,y,z)=(0,75,0)
	llumGL[2].atenuacio.a = 0.0;		llumGL[2].atenuacio.b = 0.0;		llumGL[2].atenuacio.c = 1.0;
	llumGL[2].restringida = false;
	llumGL[2].spotdirection[0] = 0.0;	llumGL[2].spotdirection[1] = 0.0;	llumGL[2].spotdirection[2] = 0.0;
	llumGL[2].spotcoscutoff = 0.0;		llumGL[2].spotexponent = 0.0;

	// LLum 3: Atenuació constant (c=1), sobre l'eix Y=X, no restringida.
	llumGL[3].encesa = false;
	llumGL[3].difusa[0] = 1.0f;			llumGL[2].difusa[1] = 1.0f;			llumGL[3].difusa[2] = 1.0f;		llumGL[3].difusa[3] = 1.0f;
	llumGL[3].especular[0] = 1.0f;		llumGL[2].especular[1] = 1.0f;		llumGL[3].especular[2] = 1.0f;	llumGL[3].especular[3] = 1;

	llumGL[3].posicio.R = 75.0;			llumGL[3].posicio.alfa = 45.0;		llumGL[3].posicio.beta = 45.0;// (x,y,z)=(75,75,75)
	llumGL[3].atenuacio.a = 0.0;		llumGL[3].atenuacio.b = 0.0;		llumGL[3].atenuacio.c = 1.0;
	llumGL[3].restringida = false;
	llumGL[3].spotdirection[0] = 0.0;	llumGL[3].spotdirection[1] = 0.0;	llumGL[3].spotdirection[2] = 0.0;
	llumGL[3].spotcoscutoff = 0.0;		llumGL[3].spotexponent = 0.0;

	// LLum 4: Atenuació constant (c=1), sobre l'eix -Z, no restringida.
	llumGL[4].encesa = false;
	llumGL[4].difusa[0] = 1.0f;			llumGL[4].difusa[1] = 1.0f;			llumGL[4].difusa[2] = 1.0f;		llumGL[4].difusa[3] = 1.0f;
	llumGL[4].especular[0] = 1.0f;		llumGL[4].especular[1] = 1.0f;		llumGL[4].especular[2] = 1.0f;	llumGL[4].especular[3] = 1;

	llumGL[4].posicio.R = 75.0;			llumGL[4].posicio.alfa = -90.0;		llumGL[4].posicio.beta = 0.0;// (x,y,z)=(0,0,-75)
	llumGL[4].atenuacio.a = 0.0;		llumGL[4].atenuacio.b = 0.0;		llumGL[4].atenuacio.c = 1.0;
	llumGL[4].restringida = false;
	llumGL[4].spotdirection[0] = 0.0;	llumGL[4].spotdirection[1] = 0.0;	llumGL[4].spotdirection[2] = 0.0;
	llumGL[4].spotcoscutoff = 0.0;		llumGL[4].spotexponent = 0.0;

	// LLum #5:
	llumGL[5].encesa = false;
	llumGL[5].difusa[0] = 1.0f;			llumGL[5].difusa[1] = 1.0f;			llumGL[5].difusa[2] = 1.0f;		llumGL[5].difusa[3] = 1.0f;
	llumGL[5].especular[0] = 1.0f;		llumGL[5].especular[1] = 1.0f;		llumGL[5].especular[2] = 1.0f;	llumGL[5].especular[3] = 1;

	llumGL[5].posicio.R = 75.0;			llumGL[5].posicio.alfa = 0.0;		llumGL[5].posicio.beta = -90.0; // (x,y,z)=(0,-75,0)
	llumGL[5].atenuacio.a = 0.0;		llumGL[5].atenuacio.b = 0.0;		llumGL[5].atenuacio.c = 1.0;
	llumGL[5].restringida = false;
	llumGL[5].spotdirection[0] = 0.0;	llumGL[5].spotdirection[1] = 0.0;	llumGL[5].spotdirection[2] = 0.0;
	llumGL[5].spotcoscutoff = 0.0;		llumGL[5].spotexponent = 0.0;

	// LLum #6: Llum Vaixell, configurada a la funció vaixell() en escena.cpp.
	llumGL[6].encesa = false;
	llumGL[6].difusa[0] = 1.0f;			llumGL[6].difusa[1] = 1.0f;			llumGL[6].difusa[2] = 1.0f;		llumGL[6].difusa[3] = 1.0f;
	llumGL[6].especular[0] = 1.0f;		llumGL[6].especular[1] = 1.0f;		llumGL[6].especular[2] = 1.0f;	llumGL[6].especular[3] = 1;

	llumGL[6].posicio.R = 0.0;			llumGL[6].posicio.alfa = 0.0;		llumGL[6].posicio.beta = 0.0; // Cap posició definida, definida en funció vaixell() en escena.cpp
	llumGL[6].atenuacio.a = 0.0;		llumGL[6].atenuacio.b = 0.0;		llumGL[6].atenuacio.c = 1.0;
	llumGL[6].restringida = false;
	llumGL[6].spotdirection[0] = 0.0;	llumGL[6].spotdirection[1] = 0.0;	llumGL[6].spotdirection[2] = 0.0;
	llumGL[6].spotcoscutoff = 0.0;		llumGL[6].spotexponent = 0.0;

	// LLum #7: Llum Far, configurada a la funció faro() en escena.cpp.
	llumGL[7].encesa = false;
	llumGL[7].difusa[0] = 1.0f;			llumGL[7].difusa[1] = 1.0f;			llumGL[7].difusa[2] = 1.0f;		llumGL[7].difusa[3] = 1.0f;
	llumGL[7].especular[0] = 1.0f;		llumGL[7].especular[1] = 1.0f;		llumGL[7].especular[2] = 1.0f;	llumGL[7].especular[3] = 1;

	llumGL[7].posicio.R = 0.0;			llumGL[7].posicio.alfa = 0.0;		llumGL[7].posicio.beta = 0.0; // Cap posició definida, definida en funció faro() en escena.cpp
	llumGL[7].atenuacio.a = 0.0;		llumGL[7].atenuacio.b = 0.0;		llumGL[7].atenuacio.c = 1.0;
	llumGL[7].restringida = false;
	llumGL[7].spotdirection[0] = 0.0;	llumGL[7].spotdirection[1] = 0.0;	llumGL[7].spotdirection[2] = 0.0;
	llumGL[7].spotcoscutoff = 0.0;		llumGL[7].spotexponent = 0.0;

	// Entorn VGI: Variables de control del menú Shaders
	shader = CAP_SHADER;  shader_programID = 0;
	shaderLighting.releaseAllShaders();
	fprintf(stderr, "Gouraud_shdrML: \n");
	if (!shader_programID) shader_programID = shaderLighting.loadFileShaders(".\\shaders\\gouraud_shdrML.vert", ".\\shaders\\gouraud_shdrML.frag");
	shader = GOURAUD_SHADER;


	// Càrrega SHADERS
	// Càrrega Shader Eixos
	fprintf(stderr, "Eixos: \n");
	if (!eixos_programID) eixos_programID = shaderEixos.loadFileShaders(".\\shaders\\eixos.VERT", ".\\shaders\\eixos.FRAG");

	// Càrrega Shader Skybox
	fprintf(stderr, "SkyBox: \n");
	if (!skC_programID) skC_programID = shader_SkyBoxC.loadFileShaders(".\\shaders\\skybox.VERT", ".\\shaders\\skybox.FRAG");

	// Càrrega VAO Skybox Cube
	if (skC_VAOID.vaoId == 0) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);
	printf("asddfgakfjasd \n");
	if (!cubemapTexture[0])
	{	// load Skybox textures
		// -------------
		std::vector<std::string> faces =
		{ ".\\textures\\skybox\\right.jpg",
					".\\textures\\skybox\\left.jpg",
					".\\textures\\skybox\\top.jpg",
					".\\textures\\skybox\\bottom.jpg",
					".\\textures\\skybox\\front.jpg",
					".\\textures\\skybox\\back.jpg"
		};
		cubemapTexture[0] = loadCubemap(faces);
		std::vector<std::string> faces4 =
		{ ".\\textures\\skyboxes\\4\\4_right.png",
			".\\textures\\skyboxes\\4\\4_left.png",
			".\\textures\\skyboxes\\4\\4_up.png",
			".\\textures\\skyboxes\\4\\4_down.png",
			".\\textures\\skyboxes\\4\\4_front.png",
			".\\textures\\skyboxes\\4\\4_back.png"
		};
		cubemapTexture[4] = loadCubemap(faces4);
		std::vector<std::string> faces5 =
		{ ".\\textures\\skyboxes\\5\\5_right.png",
					".\\textures\\skyboxes\\5\\5_left.png",
					".\\textures\\skyboxes\\5\\5_up.png",
					".\\textures\\skyboxes\\5\\5_down.png",
					".\\textures\\skyboxes\\5\\5_front.png",
					".\\textures\\skyboxes\\5\\5_back.png"
		};
		cubemapTexture[5] = loadCubemap(faces5);
		std::vector<std::string> faces2 =
		{ ".\\textures\\skyboxes\\15\\15_right.png",
					".\\textures\\skyboxes\\15\\15_left.png",
					".\\textures\\skyboxes\\15\\15_up.png",
					".\\textures\\skyboxes\\15\\15_down.png",
					".\\textures\\skyboxes\\15\\15_front.png",
					".\\textures\\skyboxes\\15\\15_back.png"
		};
		cubemapTexture[2] = loadCubemap(faces2);
	}

	// Entorn VGI: Variables de control dels botons de mouse
	m_PosEAvall.x = 0;			m_PosEAvall.y = 0;			m_PosDAvall.x = 0;			m_PosDAvall.y = 0;
	m_ButoEAvall = false;		m_ButoDAvall = false;
	m_EsfeEAvall.R = 0.0;		m_EsfeEAvall.alfa = 0.0;	m_EsfeEAvall.beta = 0.0;
	m_EsfeIncEAvall.R = 0.0;	m_EsfeIncEAvall.alfa = 0.0;	m_EsfeIncEAvall.beta = 0.0;

	// Entorn VGI: Variables que controlen paràmetres visualització: Mides finestra Windows i PV
	w = 1280;			h = 720;			// Mides de la finestra Windows (w-amplada,h-alçada)
	width_old = 1280;	height_old = 720;	// Mides de la resolució actual de la pantalla (finestra Windows)
	w_old = 1280;		h_old = 720;		// Mides de la finestra Windows (w-amplada,h-alçada) per restaurar Finestra des de fullscreen
	OPV.R = cam_Esferica[0];	OPV.alfa = cam_Esferica[1];		OPV.beta = cam_Esferica[2];		// Origen PV en esfèriques
	//OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;										// Origen PV en esfèriques
	Vis_Polar = POLARZ;

	// Entorn VGI: Color de fons i de l'objecte
	fonsR = true;		fonsG = true;		fonsB = true;
	c_fons.r = clear_colorB.x;		c_fons.g = clear_colorB.y;		c_fons.b = clear_colorB.z;			c_fons.b = clear_colorB.w;
	//c_fons.r = 0.0;		c_fons.g = 0.0;		c_fons.b = 0.0;			c_fons.b = 1.0;
	sw_color = false;
	col_obj.r = clear_colorO.x;	col_obj.g = clear_colorO.y;	col_obj.b = clear_colorO.z;		col_obj.a = clear_colorO.w;
	//col_obj.r = 1.0;	col_obj.g = 1.0;	col_obj.b = 1.0;		col_obj.a = 1.0;

// Entorn VGI: Objecte OBJ
	ObOBJ = NULL;		vao_OBJ.vaoId = 0;		vao_OBJ.vboId = 0;		vao_OBJ.nVertexs = 0;

	// Entorn VGI: OBJECTE --> Corba B-Spline i Bezier
	npts_T = 0;
	for (i = 0; i < MAX_PATCH_CORBA; i = i++)
	{
		PC_t[i].x = 0.0;
		PC_t[i].y = 0.0;
		PC_t[i].z = 0.0;
	}

	pas_CS = PAS_BSPLINE;
	sw_Punts_Control = false;

	// TRIEDRE DE FRENET / DARBOUX: VT: vector Tangent, VNP: Vector Normal Principal, VBN: vector BiNormal
	dibuixa_TriedreFrenet = false;		dibuixa_TriedreDarboux = false;
	VT = { 0.0, 0.0, 1.0 };		VNP = { 1.0, 0.0, 0.0 };	VBN = { 0.0, 1.0, 0.0 };

	// Entorn VGI: Variables del Timer
	t = 0;			anima = false;

	// Entorn VGI: Variables de l'objecte FRACTAL
	t_fractal = CAP;	soroll = 'C';
	pas = 64;			pas_ini = 64;
	sw_il = true;		palcolFractal = false;

	// Entorn VGI: Altres variables
	mida = 1.0;			nom = "";		buffer = "";
	initVAOList();	// Inicialtzar llista de VAO'S.

	// Init audio
	irrklang::ISoundEngine* audioEngine;
	audioEngine = irrklang::createIrrKlangDevice();
	gameState.audioEngine = audioEngine;
}


void InitAPI()
{
	// Vendor, Renderer, Version, Shading Laguage Version i Extensions suportades per la placa gràfica gravades en fitxer extensions.txt
	std::string nomf = "extensions.txt";
	char const* nomExt = "";
	const char* nomfitxer;
	nomfitxer = nomf.c_str();	// Conversió tipus string --> char *
	int num_Ext;

	char* str = (char*)glGetString(GL_VENDOR);
	FILE* f = fopen(nomfitxer, "w");
	if (f) {
		fprintf(f, "VENDOR: %s\n", str);
		fprintf(stderr, "VENDOR: %s\n", str);
		str = (char*)glGetString(GL_RENDERER);
		fprintf(f, "RENDERER: %s\n", str);
		fprintf(stderr, "RENDERER: %s\n", str);
		str = (char*)glGetString(GL_VERSION);
		fprintf(f, "VERSION: %s\n", str);
		fprintf(stderr, "VERSION: %s\n", str);
		str = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		fprintf(f, "SHADING_LANGUAGE_VERSION: %s\n", str);
		fprintf(stderr, "SHADING_LANGUAGE_VERSION: %s\n", str);
		glGetIntegerv(GL_NUM_EXTENSIONS, &num_Ext);
		fprintf(f, "EXTENSIONS: \n");
		fprintf(stderr, "EXTENSIONS: \n");
		for (int i = 0; i < num_Ext; i++)
		{
			nomExt = (char const*)glGetStringi(GL_EXTENSIONS, i);
			fprintf(f, "%s \n", nomExt);
			//fprintf(stderr, "%s", nomExt);	// Displaiar extensions per pantalla
		}
		//fprintf(stderr, "\n");				// Displaiar <cr> per pantalla després extensions
//				str = (char*)glGetString(GL_EXTENSIONS);
//				fprintf(f, "EXTENSIONS: %s\n", str);
				//fprintf(stderr, "EXTENSIONS: %s\n", str);
		fclose(f);
	}

	// Program
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");

	// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");

	// VBO
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
}


void GetGLVersion(int* major, int* minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"

	*major = ver[0] - '0';
	if (*major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, major);		// major = 3
		glGetIntegerv(GL_MINOR_VERSION, minor);		// minor = 2
	}
	else
	{
		*minor = ver[2] - '0';
	}

	// GLSL
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);	// 1.50 NVIDIA via Cg compiler
}

void OnSize(GLFWwindow* window, int width, int height)
{
	// TODO: Agregue aquí su código de controlador de mensajes

	// A resize event occured; cx and cy are the window's new width and height.
	// Find the OpenGL change size function given in the Lab 1 notes and call it here

	// Entorn VGI: MODIFICACIÓ ->Establim les mides de la finestra actual
	w = width;	h = height;

	// Crida a OnPaint per a redibuixar la pantalla
	//	OnPaint();
}


// OnPaint: Funció de dibuix i visualització en frame buffer del frame
void OnPaint(GLFWwindow* window)
{
	// TODO: Agregue aquí su código de controlador de mensajes
	GLdouble vpv[3] = { 0.0, 0.0, 1.0 };


	// Entorn VGI: Cridem a les funcions de l'escena i la projecció segons s'hagi 
	// seleccionat una projecció o un altra
	switch (projeccio)
	{
	case AXONOM:
		// Entorn VGI: PROJECCIÓ AXONOMÈTRICA
		// Entorn VGI: Activació del retall de pantalla
		glEnable(GL_SCISSOR_TEST);

		// Entorn VGI: Retall
		glScissor(0, 0, w, h);
		glViewport(0, 0, w, h);

		// Aquí farem les crides per a definir Viewport, Projecció i Càmara: INICI -------------------------

		// Aquí farem les cridesper a definir Viewport, Projecció i Càmara:: FI -------------------------
		// Entorn VGI: Dibuixar Model (escena)
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

		// Entorn VGI: Transferència del buffer OpenGL a buffer de pantalla
				//glfwSwapBuffers(window);
		break;

	case PERSPECT:
		// Entorn VGI: PROJECCIÓ PERSPECTIVA
				//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Set Perspective Calculations To Most Accurate
		glDisable(GL_SCISSOR_TEST);		// Desactivació del retall de pantalla

		// Entorn VGI: Activar shader Visualització Escena
		glUseProgram(shader_programID);

		// Entorn VGI: Definició de Viewport, Projecció i Càmara
		ProjectionMatrix = Projeccio_Perspectiva(shader_programID, 0, 0, w, h, OPV.R);

		// Entorn VGI: Definició de la càmera.
		if (camera == CAM_ESFERICA) {
			n[0] = 0;		n[1] = 0;		n[2] = 0;
			ViewMatrix = Vista_Esferica(shader_programID, OPV, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida, pas,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
				eixos, grid, hgrid);
		}
		else if (camera == CAM_NAVEGA) {
			if (Vis_Polar == POLARZ) {
				vpv[0] = 0.0;	vpv[1] = 0.0;	vpv[2] = 1.0;
			}
			else if (Vis_Polar == POLARY) {
				vpv[0] = 0.0;	vpv[1] = 1.0;	vpv[2] = 0.0;
			}
			else if (Vis_Polar == POLARX) {
				vpv[0] = 1.0;	vpv[1] = 0.0;	vpv[2] = 0.0;
			}
			if (gameState.enableCameraRotation) {
				ViewMatrix = Vista_Navega(shader_programID, opvN, //false, 
					n, vpv, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, true, pas,
					front_faces, oculta, test_vis, back_line,
					ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
					eixos, grid, hgrid);
			}
			else {
				CPunt3D opvN_fixed = { 10.0f, 0.0f, 0.0f };
				GLdouble n_fixed[3] = { 0.0f, 0.0f, 0.0f };
				ViewMatrix = Vista_Navega(shader_programID, opvN_fixed, //false, 
					n_fixed, vpv, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, true, pas,
					front_faces, oculta, test_vis, back_line,
					ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
					eixos, grid, hgrid);
			}
		}

		else if (camera == CAM_GEODE) {
			ViewMatrix = Vista_Geode(shader_programID, OPV_G, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida, pas,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
				eixos, grid, hgrid);
		}

		// Entorn VGI: Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

		// Entorn VGI: Transferència del buffer OpenGL a buffer de pantalla
				//glfwSwapBuffers(window);
		break;

	default:
		// Entorn VGI: Càrrega SHADERS
		// Entorn VGI: Càrrega Shader Eixos
		if (!eixos_programID) eixos_programID = shaderEixos.loadFileShaders(".\\shaders\\eixos.VERT", ".\\shaders\\eixos.FRAG");

		// Entorn VGI: Càrrega Shader de Gouraud
		if (!shader_programID) shader_programID = shaderLighting.loadFileShaders(".\\shaders\\gouraud_shdrML.vert", ".\\shaders\\gouraud_shdrML.frag");

		// Entorn VGI: Creació de la llista que dibuixarà els eixos Coordenades Món. Funció on està codi per dibuixar eixos	
		if (!eixos_Id) eixos_Id = deixos();						// Funció que defineix els Eixos Coordenades Món com un VAO.

		// Entorn VGI: Crida a la funció Fons Blanc
		FonsB();

		// Entorn VGI: Transferència del buffer OpenGL a buffer de pantalla
				//glfwSwapBuffers(window);
		break;
	}

	//  Actualitzar la barra d'estat de l'aplicació amb els valors R,A,B,PVx,PVy,PVz
	if (statusB) Barra_Estat();
}

// configura_Escena: Funcio que configura els parametres de Model i dibuixa les
//                   primitives OpenGL dins classe Model
void configura_Escena() {

	// Aplicar Transformacions Geometriques segons persiana Transformacio i Quaternions
	GTMatrix = instancia(transf, TG, TGF);


}

// dibuixa_Escena: Funcio que crida al dibuix dels diferents elements de l'escana
void dibuixa_Escena() {

	//glUseProgram(shader_programID);

//	Dibuix SkyBox Cúbic.
	//printf("SkyBox= %d \n", SkyBox);
	dibuixa_Skybox(skC_programID, cubemapTexture[SkyBox], Vis_Polar, ProjectionMatrix, ViewMatrix);

	//	Dibuix Coordenades Món i Reixes.
	dibuixa_Eixos(eixos_programID, eixos, eixos_Id, grid, hgrid, ProjectionMatrix, ViewMatrix);



	//	Dibuix geometria de l'escena amb comandes GL.
	dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
		textura, texturesID, textura_map, tFlag_invert_Y,
		npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
		ObOBJ,				// Classe de l'objecte OBJ que conté els VAO's
		ViewMatrix, GTMatrix, gameState, gameScene, modelos);

	draw_Menu_ABP();

}




void RenderUI() {
	// Comienzo del frame de ImGui


	ImGui::SetNextWindowPos(ImVec2(600, ImGui::GetIO().DisplaySize.y - 100), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(1200, 100), ImGuiCond_Always);
	ImGui::Begin("Inventario", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

	ImVec2 minBound = ImGui::GetWindowContentRegionMin();
	ImVec2 maxBound = ImGui::GetWindowContentRegionMax();
	ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)texturesID[11], minBound, maxBound);
	//ImGui::Image((void*)(intptr_t)texturesID[1], minBound, maxBound);


	// Muestra los slots del inventario
	for (auto& slot : gameState.inventory) {
		ImGui::BeginGroup();

		// Muestra la imagen del ítem en el slot
		ImGui::Image((void*)(intptr_t)texturesID[slot.imageID], ImVec2(100, 84));

		// Muestra el nombre y la cantidad del ítem
		/*
			ImGui::Text("%s", slot.itemName.c_str());
			ImGui::Text("Cantidad: %d", slot.quantity);
		*/


		// Verifica si el slot está siendo clickeado
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			// Reduz la cantidad del ítem al clickear
			if (slot.quantity > 0) {
				slot.quantity--;
			}
		}

		ImGui::EndGroup();
		ImGui::SameLine();
	}

	// Termina el inventario
	ImGui::End();

	// Fin del frame de ImGui

}


void draw_Menu_ABP()
{
	bool pulsado = false;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	time_t elapsedTimer;
	int elapsedS, elapsedM;
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGuiCond escalado = 0;
	ImVec2 inventoryPosition(900, 100);
	ImVec2 imageSize(ImGui::GetIO().DisplaySize.x * 1.04f, ImGui::GetIO().DisplaySize.y * 1.04f);
	float verticalSpacing = 20.0f;
	bool menuActivate = false;
	static bool showMenu = false;

	static bool showOptions = false;
	float yOffset = ImGui::GetIO().DisplaySize.y * 0.7f;
	ImVec2 menuSize = ImVec2(700.0f, 450.0f);
	ImVec2 menuPos = ImVec2(
		(ImGui::GetIO().DisplaySize.x - menuSize.x) * 0.5f,
		yOffset
	);
	float buttonWidth = 300;
	float buttonHeight = 75;
	float largerVerticalSpacing = 500.0f;

	const char* textLines[] = {
			"...",
			"The faint hum of the lock precedes the decisive click...",
			"The door swings open, revealing a dimly lit corridor...",
			"Congratulations, adventurer",
			"You've conquered the escape room",
			"",
			"Press Enter to continue"
	};

	const char* intro[] = {
		"",
		"Awakening in a dimly lit, vintage - designed room,",
		"your surroundings are shrouded in mystery.",
		"No recollection of your arrival, only the weight of an unsettling atmosphere.",
		"Now, you must find the exit before the time runs out.",
		"",
		"Press Enter"
	};

	static int currentLine = 0;
	static bool showNextLine = true;
	static std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();

	auto currentTime = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);

	switch (gameScene) {
	case SCENE_INTRO:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowSize(ImVec2(1200.0f, 1000.0f));
		ImGui::SetNextWindowPos(ImVec2(((ImGui::GetIO().DisplaySize.x / 2) - 300), ((ImGui::GetIO().DisplaySize.y / 2) - 200)));
		ImGui::Begin("Escape Room Intro", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);


		//ImGui::SetNextWindowPos(ImVec2(200, 300));
		
		currentTime = std::chrono::steady_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
		ImGui::PushFont(font2);

		for (int i = 0; i <= currentLine; ++i) {

			ImGui::Text("%s", intro[i]);

		}
		
		if (duration.count() > 2000 * currentLine && currentLine < sizeof(intro) / sizeof(intro[0]) - 1) {
			currentLine++;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
			gameScene = 2;
		}
		
		ImGui::PopFont();
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_START:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Pantalla completa
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

		ImGui::Begin("Imagen a Pantalla Completa",
			nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs);


		ImGui::Image((void*)(intptr_t)texturesID[80], imageSize);

		ImGui::End();

		// Cambio de imagen al presionar enter
		if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
			gameScene = 1;
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_SKYBOXES:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Pantalla completa
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

		ImGui::Begin("Change skybox",
			nullptr, ImGuiWindowFlags_NoBackground);

		ImGui::Text("Pulsa '-' para cambiar skybox");


		ImGui::End();

		// Cambio de imagen al presionar enter
		if (ImGui::IsKeyPressed(ImGuiKey_Minus)) {
			if (SkyBox == 15) {
				SkyBox = 0;
			}
			else {
				SkyBox++;
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		break;
	case SCENE_TIMER_GAMEOVER:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::Begin("Game-Over", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
		ImGui::Image((void*)(intptr_t)texturesID[82], imageSize);
		ImGui::End();


		ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 150, (ImGui::GetIO().DisplaySize.y / 2) + 100));
		ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f), escalado);
		if (false) flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Menu", nullptr, flags);

		ImGui::PushFont(fontMenu);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.0f, 0.0f, 0.3f));

		ImGui::TextColored(ImVec4(0.5, 0, 0, 1), "Retry?");

		if (ImGui::Button("Yes")) {
			gameScene = 1;
		}

		if (ImGui::Button("No")) {
			glfwSetWindowShouldClose(window, true);
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;

	case SCENE_MAIN:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
		ImGui::Image((void*)(intptr_t)texturesID[81], imageSize);
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		if (false) flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Menu", nullptr, flags);

		ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.0f;
		ImGui::GetStyle().WindowBorderSize = 0.0f;

		ImGui::PushFont(fontMenu);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 12.0f);
		ImGui::Dummy(ImVec2(0.0f, largerVerticalSpacing));
		if (ImGui::Button("Play", ImVec2(buttonWidth, buttonHeight))) {
			gameState.audioEngine->setSoundVolume(0.1f);
			gameState.audioEngine->play2D("media\\MUSICA DE TERROR.ogg", true, false, true);
			gameState.enableCameraRotation = true;
			gameScene = 19;
			printf("gameScene= %d \n", gameScene);
			gameTimer = time(NULL);
			startTime = std::chrono::steady_clock::now();
		}

		ImGui::Dummy(ImVec2(0.0f, verticalSpacing));
		if (ImGui::Button("Options", ImVec2(buttonWidth, buttonHeight))) {
			showOptions = !showOptions;
		}

		if (showOptions) {
			ImGui::SliderFloat("Volume (Music)", &volumeMusic, 0.0f, 1.0f);
			ImGui::SliderFloat("Volume (Sound effects)", &volumeSfx, 0.0f, 1.0f);

			ImGui::Checkbox("Fullscreen", &fullscreen);
			ImGui::Checkbox("Vsync", &vsync);

			if (ImGui::Button("Apply Settings", ImVec2(buttonWidth, buttonHeight)))
			{
				printf("Apply settings\n");
				wglSwapIntervalEXT(vsync);
				OnFull_Screen(primary, window);

				showOptions = false;
			}
		}

		ImGui::Dummy(ImVec2(0.0f, verticalSpacing));
		if (ImGui::Button("Exit Game", ImVec2(buttonWidth, buttonHeight))) {
			glfwSetWindowShouldClose(window, true);
		}
		ImGui::PopStyleColor(6);
		ImGui::PopStyleVar(2);
		ImGui::PopFont();

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_GAME:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (false) flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Game timer", nullptr, flags);

		ImGui::Text("Time till game over");
		//<<<<<<< inventory&menu
		elapsedTimer = 180 - (time(NULL) - gameTimer);
		elapsedM = (elapsedTimer / 60) % 60;
		//=======
				//elapsedTimer = 5 - (time(NULL) - gameTimer);

				//elapsedM = (elapsedTimer / 60);
		//>>>>>>> main
		elapsedS = elapsedTimer % 60;
		ImGui::Text("%02d:%02d\n", elapsedM, elapsedS);

		if (elapsedM == 0 && elapsedS == 0) {
			gameScene = 3;
		}

		RenderUI();

		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {

			showMenu = !showMenu;
		}


		if (showMenu) {
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
			ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
			ImGui::Image((void*)(intptr_t)texturesID[81], imageSize);
			ImGui::End();


			ImGui::SetNextWindowPos(ImVec2(200, 300));
			ImGui::SetNextWindowSize(ImVec2(700.0f, 450.0f), escalado);
			if (false) flags |= ImGuiWindowFlags_NoBackground;
			ImGui::Begin("Menu", nullptr, flags);

			ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.0f;
			ImGui::GetStyle().WindowBorderSize = 0.0f;

			ImGui::PushFont(fontMenu);

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 12.0f);

			if (ImGui::Button("Return to Menu", ImVec2(buttonWidth, buttonHeight))) {
				gameScene = 1;
				showMenu = !showMenu;
			}

			ImGui::Dummy(ImVec2(0.0f, verticalSpacing));
			if (ImGui::Button("Options", ImVec2(buttonWidth, buttonHeight))) {
				showOptions = !showOptions;
			}

			if (showOptions) {
				ImGui::SliderFloat("Volume (Music)", &volumeMusic, 0.0f, 1.0f);
				ImGui::SliderFloat("Volume (Sound effects)", &volumeSfx, 0.0f, 1.0f);

				ImGui::Checkbox("Fullscreen", &fullscreen);
				ImGui::Checkbox("Vsync", &vsync);

				if (ImGui::Button("Apply Settings", ImVec2(buttonWidth, buttonHeight)))
				{
					printf("Apply settings\n");
					wglSwapIntervalEXT(vsync);
					OnFull_Screen(primary, window);

					showOptions = false;
				}
			}

			ImGui::Dummy(ImVec2(0.0f, verticalSpacing));
			if (ImGui::Button("Exit Game", ImVec2(buttonWidth, buttonHeight))) {
				glfwSetWindowShouldClose(window, true);

			}
			ImGui::PopStyleColor(6);
			ImGui::PopStyleVar(2);
			ImGui::PopFont();
			ImGui::End();

		}

		ImGui::End();
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_DEBUG_TEST:
		break;
	case SCENE_DEBUG_TEX:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (false) flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Game timer", nullptr, flags);

		ImGui::Text("debug scene");
		elapsedTimer = 100000 - (time(NULL) - gameTimer);
		elapsedM = (elapsedTimer / 60) % 60;
		elapsedS = elapsedTimer % 60;
		ImGui::Text("%02d:%02d\n", elapsedM, elapsedS);
		if (elapsedM == 0 && elapsedS == 0) {
			gameScene = 3;
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_PUZLE1:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (false) flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Game timer", nullptr, flags);

		ImGui::Text("Puzzle 1 - CADENAT AMB SIMBOLS");
		ImGui::Text("Current combination: %d%d%d%d", gameState.puz1_currentCombination[0], gameState.puz1_currentCombination[1], gameState.puz1_currentCombination[2], gameState.puz1_currentCombination[3]);
		ImGui::Text("Correct combination: %d%d%d%d", gameState.puz1_correctCombination[0], gameState.puz1_correctCombination[1], gameState.puz1_correctCombination[2], gameState.puz1_correctCombination[3]);

		if (gameState.puz1_match)
			ImGui::Text("MATCHING COMBINATION");

		//elapsedTimer = 100000 - (time(NULL) - gameTimer);
		//elapsedM = (elapsedTimer / 60) % 60;
		//elapsedS = elapsedTimer % 60;
		//ImGui::Text("%02d:%02d\n", elapsedM, elapsedS);
		//if (elapsedM == 0 && elapsedS == 0) {
		//	gameScene = 3;
		//}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_PUZLE2:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (false) flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Game timer", nullptr, flags);

		ImGui::Text("Puzzle 2 - JOYA EN ESTATUA");
		ImGui::Text("Last click in world coords (x, y, z): %f, %f, %f", gameState.clickPosWorld_x, gameState.clickPosWorld_y, gameState.clickPosWorld_z);
		RenderUI();

		//elapsedTimer = 100000 - (time(NULL) - gameTimer);
		//elapsedM = (elapsedTimer / 60) % 60;
		//elapsedS = elapsedTimer % 60;
		//ImGui::Text("%02d:%02d\n", elapsedM, elapsedS);
		//if (elapsedM == 0 && elapsedS == 0) {
		//	gameScene = 3;
		//}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_PUZLE3:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (false) flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Game timer", nullptr, flags);

		ImGui::Text("Puzzle 3 - COMBINACIO NUMERICA");
		ImGui::Text("Current combination: %d%d%d%d", gameState.puz3_currentCombination[0], gameState.puz3_currentCombination[1], gameState.puz3_currentCombination[2], gameState.puz3_currentCombination[3]);
		ImGui::Text("Correct combination: %d%d%d%d", gameState.puz3_correctCombination[0], gameState.puz3_correctCombination[1], gameState.puz3_correctCombination[2], gameState.puz3_correctCombination[3]);

		if (gameState.puz3_match)
			ImGui::Text("MATCHING COMBINATION");


		//elapsedTimer = 100000 - (time(NULL) - gameTimer);
		//elapsedM = (elapsedTimer / 60) % 60;
		//elapsedS = elapsedTimer % 60;
		//ImGui::Text("%02d:%02d\n", elapsedM, elapsedS);
		//if (elapsedM == 0 && elapsedS == 0) {
		//	gameScene = 3;
		//}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_PUZLE4:
		break;
	case SCENE_WIN:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowSize(ImVec2(1200.0f, 1000.0f));
		ImGui::SetNextWindowPos(ImVec2(((ImGui::GetIO().DisplaySize.x / 2)-300), ((ImGui::GetIO().DisplaySize.y / 2)-200)));
		ImGui::Begin("Escape Room Ending", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
		currentTime = std::chrono::steady_clock::now();
		//ImGui::SetNextWindowPos(ImVec2(200, 300));
		;
		ImGui::PushFont(font2);

		for (int i = 0; i <= currentLine; ++i) {
			
			ImGui::Text("%s", textLines[i]);
			
		}

		if (duration.count() > 2000 * currentLine && currentLine < sizeof(textLines) / sizeof(textLines[0]) - 1) {
			currentLine++;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
			gameScene = 1;
		}

		ImGui::PopFont();
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_PUZLE6:
		break;
	case SCENE_ITEM_INSPECT:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (false) flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Game timer", nullptr, flags);

		//ImGui::Text("Puzzle 4: Clau amagada");

		//elapsedTimer = 100000 - (time(NULL) - gameTimer);
		//elapsedM = (elapsedTimer / 60) % 60;
		//elapsedS = elapsedTimer % 60;
		//ImGui::Text("%02d:%02d\n", elapsedM, elapsedS);
		//if (elapsedM == 0 && elapsedS == 0) {
		//	gameScene = 3;
		//}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		break;
	case SCENE_BACKGROUND:

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("OpenGL Texture Text", nullptr, flags);
		ImGui::Image((void*)(intptr_t)texturesID[11], ImVec2(900, 244));

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		break;
	default:
		break;
	}


}

// Barra_Estat: Actualitza la barra d'estat (Status Bar) de l'aplicació amb els
//      valors R,A,B,PVx,PVy,PVz en Visualització Interactiva.
void Barra_Estat()
{
}

void draw_Menu_ImGui()
{
}


void MostraEntornVGIWindow(bool* p_open)
{
}

void ShowArxiusOptions()
{

}


void ShowAboutWindow(bool* p_open)
{

}

// Entorn VGI: Funció que retorna opció de menú TIPUS CAMERA segons variable camera (si modificada per teclat)
int shortCut_Camera()
{
	return 0;
}


// Entorn VGI: Funció que retorna opció de menú TIPUS PROJECCIO segons variable projecte (si modificada per teclat)
int shortCut_Projeccio()
{
	return 0;
}

// Entorn VGI: Funció que retorna opció de menú Objecte segons variable objecte (si modificada per teclat)
int shortCut_Objecte()
{
	return 0;
}

// Entorn VGI: Funció que retorna opció de menú TIPUS ILUMINACIO segons variable ilumina (si modificada per teclat)
int shortCut_Iluminacio()
{
	return 0;
}


// Demonstrate most Dear ImGui features (this is big function!)
// You may execute this function to experiment with the UI and understand what it does.
// You may then seABP_CUSTOMh for keywords in the code when you are interested by a specific feature.
void ShowEntornVGIWindow(bool* p_open)
{

}

void Menu_Shaders_Opcio_CarregarVSFS()
{

}

void Menu_Shaders_Opcio_CarregarProgram()
{
}

void Menu_Shaders_Opcio_GravarProgram()
{

}

/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL TECLAT                              */
/* ------------------------------------------------------------------------- */

// OnKeyDown: Funció de tractament de teclat (funció que es crida quan es prem una tecla)
//   PARÀMETRES:
//    - key: Codi del caracter seleccionat
//    - scancode: Nombre de vegades que s'ha apretat la tecla (acceleració)
//    - action: Acció de la tecla: GLFW_PRESS (si s'ha apretat), GLFW_REPEAT, si s'ha repetit pressió i GL_RELEASE, si es deixa d'apretar.
//    - mods: Variable que identifica si la tecla s'ha pulsat directa (mods=0), juntament amb la tecla Shift (mods=1) o la tecla Ctrl (mods=2).
void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

	// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	const double incr = 0.025f;
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };

	// EntornVGI.ImGui: Test si events de mouse han sigut filtrats per ImGui (io.WantCaptureMouse)
// (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
	ImGuiIO& io = ImGui::GetIO();
	//io.AddMouseButtonEvent(button, true);

	// (2) ONLY forward mouse data to your underlying app/game.
	switch (gameScene) {
	case SCENE_MAIN:
		return;	// desactiva controles teclado en esta escena

	case SCENE_GAME:
	case SCENE_DEBUG_TEST:
	case SCENE_DEBUG_TEX:
	case SCENE_PUZLE1:
	case SCENE_BACKGROUND:
		break;	// activa controles teclado en estas escenas

	case SCENE_TIMER_GAMEOVER:
		return;
	case SCENE_SKYBOXES:
		return;

	default:
		break;
	}

	if (!io.WantCaptureKeyboard) { //<Tractament mouse de l'aplicació>}
		// ABP: pass input to game
		gameState.OnKeyDown(window, key, scancode, action, mods);

		// EntornVGI: Si tecla pulsada és ESCAPE, tancar finestres i aplicació.
		if (mods == 1 && key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			gameScene = 1;
		}
		else if (mods == 0 && key == GLFW_KEY_PRINT_SCREEN && action == GLFW_PRESS) statusB = !statusB;
		else if ((mods == GLFW_MOD_SHIFT) && (action == GLFW_PRESS)) Teclat_Shift(key, window);	// Shorcuts Shift Key
		else if ((mods == GLFW_MOD_CONTROL) && (action == GLFW_PRESS)) Teclat_Ctrl(key);	// Shortcuts Ctrl Key
		else if ((mods == GLFW_MOD_ALT) && (action == GLFW_PRESS)) Teclat_Alt(key);	// Shortcuts ALT Key
		else if ((sw_grid) && ((grid.x) || (grid.y) || (grid.z))) Teclat_Grid(key, action);
		else if (((key == GLFW_KEY_G) && (action == GLFW_PRESS)) && ((grid.x) || (grid.y) || (grid.z))) sw_grid = !sw_grid;
		else if ((key == GLFW_KEY_O) && (action == GLFW_PRESS)) sw_color = true; // Activació color objecte
		else if ((key == GLFW_KEY_F) && (action == GLFW_PRESS)) sw_color = false; // Activació color objecte
		else if (camera == CAM_NAVEGA) Teclat_Navega(key, action);
	}
}



void OnKeyUp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	gameState.OnKeyUp(window, key, scancode, action, mods);
}

void OnTextDown(GLFWwindow* window, unsigned int codepoint)
{
}

// Teclat_Shift: Shortcuts per Pop Ups Fitxer, Finestra, Vista, Projecció i Objecte
void Teclat_Shift(int key, GLFWwindow* window)
{
	//const char* nomfitxer;
	bool testA = false;
	int error = 0;

	nfdchar_t* nomFitxer = NULL;
	nfdresult_t result; // = NFD_OpenDialog(NULL, NULL, &nomFitxer);

	CColor color_Mar = { 0.0,0.0,0.0,1.0 };

	switch (key)
	{
		// ----------- POP UP Fitxers

				// Tecla Obrir Fitxer OBJ (ABP: no funciona, lo dejo para ver como lo podemos arreglar y usar para cargar modelos 3d)
	case GLFW_KEY_F2:
		// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.OBJ)
		nomFitxer = NULL;
		result = NFD_OpenDialog(NULL, NULL, &nomFitxer);

		if (result == NFD_OKAY) {
			puts("Success!");
			puts(nomFitxer);

			objecte = OBJOBJ;	textura = true;		tFlag_invert_Y = false;
			//char* nomfitx = nomFitxer;
			if (ObOBJ == NULL) ObOBJ = ::new COBJModel;
			else { // Si instància ja s'ha utilitzat en un objecte OBJ
				ObOBJ->netejaVAOList_OBJ();		// Netejar VAO, EBO i VBO
				ObOBJ->netejaTextures_OBJ();	// Netejar buffers de textures
			}

			//int error = ObOBJ->LoadModel(nomfitx);			// Carregar objecte OBJ amb textura com a varis VAO's
			int error = ObOBJ->LoadModel(nomFitxer);			// Carregar objecte OBJ amb textura com a varis VAO's

			//	Pas de paràmetres textura al shader
			if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "textur"), textura);
			if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "flag_invert_y"), tFlag_invert_Y);
			free(nomFitxer);
		}
		break;



		// ----------- POP UP Vista
		// 
				// Tecla Full Screen?
	case GLFW_KEY_F:
		fullscreen = !fullscreen;
		OnFull_Screen(primary, window);
		break;

	default:
		break;
	}
}


// Teclat_Ctrl: Shortcuts per Pop Ups Transforma, Iluminació, llums, Shaders
void Teclat_Ctrl(int key)
{
	//const char* nomfitxer;
	std::string nomVert, nomFrag;	// Nom de fitxer.

	nfdchar_t* nomFitxer = NULL;
	nfdresult_t result; // = NFD_OpenDialog(NULL, NULL, &nomFitxer);

	switch (key)
	{

		// ----------- POP UP ILUMINACIÓ
			// Tecla Llum Fixe? (opció booleana).
	case GLFW_KEY_F:
		ifixe = !ifixe;
		break;

		// Tecla Iluminació --> Punts
	case GLFW_KEY_F1:
		ilumina = PUNTS;
		test_vis = false;		oculta = false;
		break;

		// Tecla Iluminació --> Filferros
	case GLFW_KEY_F2:
		if (ilumina != FILFERROS) {
			ilumina = FILFERROS;
			test_vis = false;		oculta = false;
		}
		break;

		// Tecla Iluminació --> Plana
	case GLFW_KEY_F3:
		if (ilumina != PLANA) {
			ilumina = PLANA;
			test_vis = false;		oculta = true;
			/*			// Elimina shader anterior
						shaderLighting.DeleteProgram();
						// Càrrega Flat shader
						shader_programID = shaderLighting.loadFileShaders(".\\shaders\\flat_shdrML.vert", ".\\shaders\\flat_shdrML.frag");
			*/
		}
		break;

		// Tecla Iluminació --> Suau
	case GLFW_KEY_F4:
		if (ilumina != SUAU) {
			ilumina = SUAU;
			test_vis = false;		oculta = true;
		}
		break;

		// Tecla Iluminació --> Reflexió Material --> Emissió?
	case GLFW_KEY_F6:
		sw_material[0] = !sw_material[0];
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[0]"), sw_material[0]); // Pas màscara llums al shader
		break;

		// Tecla Iluminació --> Reflexió Material -> Ambient?
	case GLFW_KEY_F7:
		sw_material[1] = !sw_material[1];
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[1]"), sw_material[1]); // Pas màscara llums al shader
		break;

		// Tecla Iluminació --> Reflexió Material -> Difusa?
	case GLFW_KEY_F8:
		sw_material[2] = !sw_material[2];
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[2]"), sw_material[2]); // Pas màscara llums al shader
		break;

		// Tecla Iluminació --> Reflexió Material -> Especular?
	case GLFW_KEY_F9:
		sw_material[3] = !sw_material[3];
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[3]"), sw_material[3]); // Pas màscara llums al shader
		break;

		// Tecla Iluminació --> Reflexió Material -> Especular?
	case GLFW_KEY_F10:
		sw_material[4] = !sw_material[4];
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[4]"), sw_material[4]); // Pas màscara llums al shader
		break;

		// Tecla Iluminació --> Textura?.
	case GLFW_KEY_I:
		textura = !textura;
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "texture"), textura); //	Pas de textura al shader
		break;



		// ----------- POP UP Llums
			// Tecla Llums --> Llum Ambient?.
	case GLFW_KEY_A:
		llum_ambient = !llum_ambient;
		sw_il = true;
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[1]"), (llum_ambient && sw_material[1])); // Pas màscara llums al shader
		break;

		// Tecla Llums --> Llum #0? (+Z)
	case GLFW_KEY_0:
		llumGL[0].encesa = !llumGL[0].encesa;
		sw_il = true;
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[0]"), llumGL[0].encesa); // Pas màscara Llum #0 al shader
		break;

		// Tecla Llums --> Llum #1? (+X)
	case GLFW_KEY_1:
		llumGL[1].encesa = !llumGL[1].encesa;
		sw_il = true;
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[1]"), llumGL[1].encesa);
		break;

		// Tecla Llums --> Llum #2? (+Y)
	case GLFW_KEY_2:
		llumGL[2].encesa = !llumGL[2].encesa;
		sw_il = true;
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[2]"), llumGL[2].encesa);
		break;

		// Tecla Llums --> Llum #3? (Z=Y=X)
	case GLFW_KEY_3:
		llumGL[3].encesa = !llumGL[3].encesa;
		sw_il = true;

		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[3]"), llumGL[3].encesa);
		break;

		// Tecla Llums --> Llum #4? (-Z)
	case GLFW_KEY_4:
		llumGL[4].encesa = !llumGL[4].encesa;
		sw_il = true;
		if (!shader_programID) glUniform1i(glGetUniformLocation(shader_programID, "sw_lights[4]"), llumGL[4].encesa);
		break;

		// Tecla Shaders --> Fitxers Shaders
	case GLFW_KEY_F12:

		break;

	default:
		break;
	}
}

void Teclat_Alt(int key)
{
	switch (key)
	{
		// Tecla Full Screen?
	case GLFW_KEY_ENTER:
		fullscreen = !fullscreen;
		OnFull_Screen(primary, window);
		break;
	}
}

// Teclat_ColorObjecte: Teclat pels canvis de color de l'objecte per teclat.
void Teclat_ColorObjecte(int key, int action)
{

}


// Teclat_ColorFons: Teclat pels canvis del color de fons.
void Teclat_ColorFons(int key, int action)
{		}

// Teclat_Navega: Teclat pels moviments de navegació.
void Teclat_Navega(int key, int action)
{
	GLdouble vdir[3] = { 0, 0, 0 };
	double modul = 0;

	// Entorn VGI: Controls de moviment de navegació
	vdir[0] = n[0] - opvN.x;
	vdir[1] = n[1] - opvN.y;
	vdir[2] = n[2] - opvN.z;
	modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
	vdir[0] = vdir[0] / modul;
	vdir[1] = vdir[1] / modul;
	vdir[2] = vdir[2] / modul;

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
			// Tecla cursor amunt
		case GLFW_KEY_UP:
			if (Vis_Polar == POLARZ) {  // (X,Y,Z)
				opvN.x += fact_pan * vdir[0];
				opvN.y += fact_pan * vdir[1];
				n[0] += fact_pan * vdir[0];
				n[1] += fact_pan * vdir[1];
			}
			else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
				opvN.x += fact_pan * vdir[0];
				opvN.z += fact_pan * vdir[2];
				n[0] += fact_pan * vdir[0];
				n[2] += fact_pan * vdir[2];
			}
			else if (Vis_Polar == POLARX) {	//(X,Y,Z) --> (Y,Z,X)
				opvN.y += fact_pan * vdir[1];
				opvN.z += fact_pan * vdir[2];
				n[1] += fact_pan * vdir[1];
				n[2] += fact_pan * vdir[2];
			}
			break;

			// Tecla cursor avall
		case GLFW_KEY_DOWN:
			if (Vis_Polar == POLARZ) { // (X,Y,Z)
				opvN.x -= fact_pan * vdir[0];
				opvN.y -= fact_pan * vdir[1];
				n[0] -= fact_pan * vdir[0];
				n[1] -= fact_pan * vdir[1];
			}
			else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
				opvN.x -= fact_pan * vdir[0];
				opvN.z -= fact_pan * vdir[2];
				n[0] -= fact_pan * vdir[0];
				n[2] -= fact_pan * vdir[2];
			}
			else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
				opvN.y -= fact_pan * vdir[1];
				opvN.z -= fact_pan * vdir[2];
				n[1] -= fact_pan * vdir[1];
				n[2] -= fact_pan * vdir[2];
			}
			break;

			// Tecla cursor esquerra
		case GLFW_KEY_LEFT:
			angleZ += fact_pan;
			if (Vis_Polar == POLARZ) { // (X,Y,Z)
				n[0] = vdir[0]; // n[0] - opvN.x;
				n[1] = vdir[1]; // n[1] - opvN.y;
				n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
				n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
				n[0] = n[0] + opvN.x;
				n[1] = n[1] + opvN.y;
			}
			else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
				n[2] = vdir[2]; // n[2] - opvN.z;
				n[0] = vdir[0]; // n[0] - opvN.x;
				n[2] = n[2] * cos(angleZ * PI / 180) - n[0] * sin(angleZ * PI / 180);
				n[0] = n[2] * sin(angleZ * PI / 180) + n[0] * cos(angleZ * PI / 180);
				n[2] = n[2] + opvN.z;
				n[0] = n[0] + opvN.x;
			}
			else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
				n[1] = vdir[1]; // n[1] - opvN.y;
				n[2] = vdir[2]; // n[2] - opvN.z;
				n[1] = n[1] * cos(angleZ * PI / 180) - n[2] * sin(angleZ * PI / 180);
				n[2] = n[1] * sin(angleZ * PI / 180) + n[2] * cos(angleZ * PI / 180);
				n[1] = n[1] + opvN.y;
				n[2] = n[2] + opvN.z;
			}
			break;

			// Tecla cursor dret
		case GLFW_KEY_RIGHT:
			angleZ = 360 - fact_pan;
			if (Vis_Polar == POLARZ) { // (X,Y,Z)
				n[0] = vdir[0]; // n[0] - opvN.x;
				n[1] = vdir[1]; // n[1] - opvN.y;
				n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
				n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
				n[0] = n[0] + opvN.x;
				n[1] = n[1] + opvN.y;
			}
			else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
				n[2] = vdir[2]; // n[2] - opvN.z;
				n[0] = vdir[0]; // n[0] - opvN.x;
				n[2] = n[2] * cos(angleZ * PI / 180) - n[0] * sin(angleZ * PI / 180);
				n[0] = n[2] * sin(angleZ * PI / 180) + n[0] * cos(angleZ * PI / 180);
				n[2] = n[2] + opvN.z;
				n[0] = n[0] + opvN.x;
			}
			else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
				n[1] = vdir[1]; // n[1] - opvN.y;
				n[2] = vdir[2]; // n[2] - opvN.z;
				n[1] = n[1] * cos(angleZ * PI / 180) - n[2] * sin(angleZ * PI / 180);
				n[2] = n[1] * sin(angleZ * PI / 180) + n[2] * cos(angleZ * PI / 180);
				n[1] = n[1] + opvN.y;
				n[2] = n[2] + opvN.z;
			}
			break;

			// Tecla Inicio
		case GLFW_KEY_HOME:
			if (Vis_Polar == POLARZ) {
				opvN.z += fact_pan;
				n[2] += fact_pan;
			}
			else if (Vis_Polar == POLARY) {
				opvN.y += fact_pan;
				n[1] += fact_pan;
			}
			else if (Vis_Polar == POLARX) {
				opvN.x += fact_pan;
				n[0] += fact_pan;
			}
			break;

			// Tecla Fin
		case GLFW_KEY_END:
			if (Vis_Polar == POLARZ) {
				opvN.z -= fact_pan;
				n[2] -= fact_pan;
			}
			else if (Vis_Polar == POLARY) {
				opvN.y -= fact_pan;
				n[1] -= fact_pan;
			}
			else if (Vis_Polar == POLARX) {
				opvN.x -= fact_pan;
				n[0] -= fact_pan;
			}
			break;

			// Tecla PgUp
		case GLFW_KEY_PAGE_UP:
			fact_pan /= 2;
			if (fact_pan < 0.125) fact_pan = 0.125;
			break;

			// Tecla PgDown
		case GLFW_KEY_PAGE_DOWN:
			fact_pan *= 2;
			if (fact_pan > 2048) fact_pan = 2048;
			break;

		default:
			break;
		}
	}
}


// Teclat_Pan: Teclat pels moviments de Pan.
void Teclat_Pan(int key, int action)
{

}

// Teclat_TransEscala: Teclat pels canvis del valor d'escala per X,Y,Z.
void Teclat_TransEscala(int key, int action)
{

}

// Teclat_TransRota: Teclat pels canvis del valor del vector de l'angle de rotació per X,Y,Z.
void Teclat_TransRota(int key, int action)
{

}


// Teclat_TransTraslada: Teclat pels canvis del valor de traslació per X,Y,Z.
void Teclat_TransTraslada(int key, int action)
{

}


// Teclat_Grid: Teclat pels desplaçaments dels gridXY, gridXZ i gridYZ.
void Teclat_Grid(int key, int action)
{

}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL RATOLI                              */
/* ------------------------------------------------------------------------- */

// OnMouseButton: Funció que es crida quan s'apreta algun botó (esquerra o dreta) del mouse.
//      PARAMETRES: - window: Finestra activa
//					- button: Botó seleccionat (GLFW_MOUSE_BUTTON_LEFT o GLFW_MOUSE_BUTTON_RIGHT)
//					- action: Acció de la tecla: GLFW_PRESS (si s'ha apretat), GLFW_REPEAT, si s'ha repetit pressió i GL_RELEASE, si es deixa d'apretar.
void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	// Get the cursor position when the mouse key has been pressed or released.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);



	// EntornVGI.ImGui: Test si events de mouse han sigut filtrats per ImGui (io.WantCaptureMouse)
	// (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(button, action);

	// (2) ONLY forward mouse data to your underlying app/game.
	if (!io.WantCaptureMouse) { //<Tractament mouse de l'aplicació>}
		// OnLButtonDown
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			// ABP DEBUG
			gameState.OnMouseButton(window, button, action, mods);


			// Entorn VGI: Detectem en quina posició s'ha apretat el botó esquerra del
			//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoEAvall
			m_ButoEAvall = true;
			m_PosEAvall.x = xpos;	m_PosEAvall.y = ypos;
			m_EsfeEAvall = OPV;
		}
		// OnLButtonUp: Funció que es crida quan deixem d'apretar el botó esquerra del mouse.
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			gameState.OnMouseButtonRelease(window, button, action, mods);

			// Entorn VGI: Desactivem flag m_ButoEAvall quan deixem d'apretar botó esquerra del mouse.
			m_ButoEAvall = false;

			// OPCIÓ VISTA-->SATÈLIT: Càlcul increment desplaçament del Punt de Vista
			if ((satelit) && (projeccio != ORTO))
			{	//m_EsfeIncEAvall.R = m_EsfeEAvall.R - OPV.R;
				m_EsfeIncEAvall.alfa = 0.01f * (OPV.alfa - m_EsfeEAvall.alfa); //if (abs(m_EsfeIncEAvall.alfa)<0.01) { if ((m_EsfeIncEAvall.alfa)>0.0) m_EsfeIncEAvall.alfa = 0.01 else m_EsfeIncEAvall.alfa=0.01}
				m_EsfeIncEAvall.beta = 0.01f * (OPV.beta - m_EsfeEAvall.beta);
				if (abs(m_EsfeIncEAvall.beta) < 0.01)
				{
					if ((m_EsfeIncEAvall.beta) > 0.0) m_EsfeIncEAvall.beta = 0.01;
					else m_EsfeIncEAvall.beta = 0.01;
				}
				//if ((m_EsfeIncEAvall.R == 0.0) && (m_EsfeIncEAvall.alfa == 0.0) && (m_EsfeIncEAvall.beta == 0.0)) KillTimer(WM_TIMER);
				//else SetTimer(WM_TIMER, 10, NULL);
			}
		}
		// OnRButtonDown
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{	// Entorn VGI: Detectem en quina posició s'ha apretat el botó esquerra del
			//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoEAvall
			m_ButoDAvall = true;
			//m_PosDAvall = point;
			m_PosDAvall.x = xpos;	m_PosDAvall.y = ypos;
		}
		// OnLButtonUp: Funció que es crida quan deixem d'apretar el botó esquerra del mouse.
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		{	// Entorn VGI: Desactivem flag m_ButoEAvall quan deixem d'apretar botó esquerra del mouse.
			m_ButoDAvall = false;
		}
	}
}

// OnMouseMove: Funció que es crida quan es mou el mouse. La utilitzem per la 
//				  Visualització Interactiva amb les tecles del mouse apretades per 
//				  modificar els paràmetres de P.V. (R,angleh,anglev) segons els 
//				  moviments del mouse.
//      PARAMETRES: - window: Finestra activa
//					- xpos: Posició X del cursor del mouse (coord. pantalla) quan el botó s'ha apretat.
//					- ypos: Posició Y del cursor del mouse(coord.pantalla) quan el botó s'ha apretat.
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	gameState.OnMouseMove(window, xpos, ypos);
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };
	CSize gir = { 0,0 }, girn = { 0,0 }, girT = { 0,0 }, zoomincr = { 0,0 };

	// TODO: Add your message handler code here and/or call default
	if (m_ButoEAvall && mobil && projeccio != CAP)
	{
		// Entorn VGI: Determinació dels angles (en graus) segons l'increment
		//				horitzontal i vertical de la posició del mouse.
		gir.cx = m_PosEAvall.x - xpos;		gir.cy = m_PosEAvall.y - ypos;
		m_PosEAvall.x = xpos;				m_PosEAvall.y = ypos;
		if (camera == CAM_ESFERICA)
		{	// Càmera Esfèrica
			OPV.beta = OPV.beta - gir.cx / 2.0;
			OPV.alfa = OPV.alfa + gir.cy / 2.0;

			// Entorn VGI: Control per evitar el creixement desmesurat dels angles.
			if (OPV.alfa >= 360)	OPV.alfa = OPV.alfa - 360.0;
			if (OPV.alfa < 0)		OPV.alfa = OPV.alfa + 360.0;
			if (OPV.beta >= 360)	OPV.beta = OPV.beta - 360.0;
			if (OPV.beta < 0)		OPV.beta = OPV.beta + 360.0;
		}
		else { // Càmera Geode
			OPV_G.beta = OPV_G.beta + gir.cx / 2;
			OPV_G.alfa = OPV_G.alfa + gir.cy / 2;
			// Entorn VGI: Control per evitar el creixement desmesurat dels angles
			if (OPV_G.alfa >= 360.0f)	OPV_G.alfa = OPV_G.alfa - 360.0;
			if (OPV_G.alfa < 0.0f)		OPV_G.alfa = OPV_G.alfa + 360.0;
			if (OPV_G.beta >= 360.f)	OPV_G.beta = OPV_G.beta - 360.0;
			if (OPV_G.beta < 0.0f)		OPV_G.beta = OPV_G.beta + 360.0;
		}
		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint(window);
	}
	else if (m_ButoEAvall && (camera == CAM_NAVEGA) && (projeccio != CAP && projeccio != ORTO)) // Opció Navegació
	{
		//// Entorn VGI: Canviar orientaci� en opci� de Navegaci�
		//girn.cx = m_PosEAvall.x - xpos;		girn.cy = m_PosEAvall.y - ypos;
		//angleZ = girn.cx / 2.0;
		//// Entorn VGI: Control per evitar el creixement desmesurat dels angles.
		//if (angleZ >= 360) angleZ = angleZ - 360;
		//if (angleZ < 0)	angleZ = angleZ + 360;

		//// Entorn VGI: Segons orientaci� dels eixos Polars (Vis_Polar)
		//if (Vis_Polar == POLARZ) { // (X,Y,Z)
		//	n[0] = n[0] - opvN.x;
		//	n[1] = n[1] - opvN.y;
		//	n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
		//	n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
		//	n[0] = n[0] + opvN.x;
		//	n[1] = n[1] + opvN.y;
		//}
		//else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
		//	n[2] = n[2] - opvN.z;
		//	n[0] = n[0] - opvN.x;
		//	n[2] = n[2] * cos(angleZ * PI / 180) - n[0] * sin(angleZ * PI / 180);
		//	n[0] = n[2] * sin(angleZ * PI / 180) + n[0] * cos(angleZ * PI / 180);
		//	n[2] = n[2] + opvN.z;
		//	n[0] = n[0] + opvN.x;
		//}
		//else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
		//	n[1] = n[1] - opvN.y;
		//	n[2] = n[2] - opvN.z;
		//	n[1] = n[1] * cos(angleZ * PI / 180) - n[2] * sin(angleZ * PI / 180);
		//	n[2] = n[1] * sin(angleZ * PI / 180) + n[2] * cos(angleZ * PI / 180);
		//	n[1] = n[1] + opvN.y;
		//	n[2] = n[2] + opvN.z;
		//}

		//m_PosEAvall.x = xpos;		m_PosEAvall.y = ypos;
		//// Crida a OnPaint() per redibuixar l'escena
		////OnPaint(window);

	}

	// Entorn VGI: Transformació Geomètrica interactiva pels eixos X,Y boto esquerra del mouse.
	else {
		bool transE = transX || transY;
		if (m_ButoEAvall && transE && transf)
		{
			// Calcular increment
			girT.cx = m_PosEAvall.x - xpos;		girT.cy = m_PosEAvall.y - ypos;
			if (transX)
			{
				long int incrT = girT.cx;
				if (trasl)
				{
					TG.VTras.x += incrT * fact_Tras;
					if (TG.VTras.x < -100000) TG.VTras.x = 100000;
					if (TG.VTras.x > 100000) TG.VTras.x = 100000;
				}
				else if (rota)
				{
					TG.VRota.x += incrT * fact_Rota;
					while (TG.VRota.x >= 360) TG.VRota.x -= 360;
					while (TG.VRota.x < 0) TG.VRota.x += 360;
				}
				else if (escal)
				{
					if (incrT < 0) incrT = -1 / incrT;
					TG.VScal.x = TG.VScal.x * incrT;
					if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
					if (TG.VScal.x > 8192) TG.VScal.x = 8192;
				}
			}
			if (transY)
			{
				long int incrT = girT.cy;
				if (trasl)
				{
					TG.VTras.y += incrT * fact_Tras;
					if (TG.VTras.y < -100000) TG.VTras.y = 100000;
					if (TG.VTras.y > 100000) TG.VTras.y = 100000;
				}
				else if (rota)
				{
					TG.VRota.y += incrT * fact_Rota;
					while (TG.VRota.y >= 360) TG.VRota.y -= 360;
					while (TG.VRota.y < 0) TG.VRota.y += 360;
				}
				else if (escal)
				{
					if (incrT <= 0) {
						if (incrT >= -2) incrT = -2;
						incrT = 1 / Log2(-incrT);
					}
					else incrT = Log2(incrT);
					TG.VScal.y = TG.VScal.y * incrT;
					if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
					if (TG.VScal.y > 8192) TG.VScal.y = 8192;
				}
			}
			m_PosEAvall.x = xpos;	m_PosEAvall.y = ypos;
			// Crida a OnPaint() per redibuixar l'escena
			//InvalidateRect(NULL, false);
			//OnPaint(windows);
		}
	}

	// Entorn VGI: Determinació del desplaçament del pan segons l'increment
	//				vertical de la posició del mouse (tecla dreta apretada).
	if (m_ButoDAvall && pan && (projeccio != CAP && projeccio != ORTO))
	{
		//CSize zoomincr = m_PosDAvall - point;
		zoomincr.cx = m_PosDAvall.x - xpos;		zoomincr.cy = m_PosDAvall.y - ypos;
		long int incrx = zoomincr.cx;
		long int incry = zoomincr.cy;

		// Desplaçament pan vertical
		tr_cpv.y -= incry * fact_pan;
		if (tr_cpv.y > 100000) tr_cpv.y = 100000;
		else if (tr_cpv.y < -100000) tr_cpv.y = -100000;

		// Desplaçament pan horitzontal
		tr_cpv.x += incrx * fact_pan;
		if (tr_cpv.x > 100000) tr_cpv.x = 100000;
		else if (tr_cpv.x < -100000) tr_cpv.x = -100000;

		//m_PosDAvall = point;
		m_PosDAvall.x = xpos;	m_PosDAvall.y = ypos;
		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint(window);
	}
	// Determinació del paràmetre R segons l'increment
	//   vertical de la posició del mouse (tecla dreta apretada)
		//else if (m_ButoDAvall && zzoom && (projeccio!=CAP && projeccio!=ORTO))
	else if (m_ButoDAvall && zzoom && (projeccio != CAP))
	{
		//CSize zoomincr = m_PosDAvall - point;
		zoomincr.cx = m_PosDAvall.x - xpos;		zoomincr.cy = m_PosDAvall.y - ypos;
		long int incr = zoomincr.cy / 1.0;

		//		zoom=zoom+incr;
		OPV.R = OPV.R + incr;
		if (OPV.R < p_near) OPV.R = p_near;
		if (OPV.R > p_far) OPV.R = p_far;
		//m_PosDAvall = point;
		m_PosDAvall.x = xpos;				m_PosDAvall.y = ypos;
		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint(window);
	}
	else if (m_ButoDAvall && (camera == CAM_NAVEGA) && (projeccio != CAP && projeccio != ORTO))
	{	// Avançar en opció de Navegació
		if ((m_PosDAvall.x != xpos) && (m_PosDAvall.y != ypos))
		{
			//CSize zoomincr = m_PosDAvall - point;
			zoomincr.cx = m_PosDAvall.x - xpos;		zoomincr.cy = m_PosDAvall.y - ypos;
			double incr = zoomincr.cy / 2.0;
			//	long int incr=zoomincr.cy/2.0;  // Causa assertion en "afx.inl" lin 169
			vdir[0] = n[0] - opvN.x;
			vdir[1] = n[1] - opvN.y;
			vdir[2] = n[2] - opvN.z;
			modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
			vdir[0] = vdir[0] / modul;
			vdir[1] = vdir[1] / modul;
			vdir[2] = vdir[2] / modul;
			opvN.x += incr * vdir[0];
			opvN.y += incr * vdir[1];
			n[0] += incr * vdir[0];
			n[1] += incr * vdir[1];
			//m_PosDAvall = point;
			m_PosDAvall.x = xpos;				m_PosDAvall.y = ypos;
			// Crida a OnPaint() per redibuixar l'escena
			//OnPaint(window);
		}
	}

	// Entorn VGI: Transformació Geomètrica interactiva per l'eix Z amb boto dret del mouse.
	else if (m_ButoDAvall && transZ && transf)
	{
		// Calcular increment
		girT.cx = m_PosDAvall.x - xpos;		girT.cy = m_PosDAvall.y - ypos;
		long int incrT = girT.cy;
		if (trasl)
		{
			TG.VTras.z += incrT * fact_Tras;
			if (TG.VTras.z < -100000) TG.VTras.z = 100000;
			if (TG.VTras.z > 100000) TG.VTras.z = 100000;
		}
		else if (rota)
		{
			incrT = girT.cx;
			TG.VRota.z += incrT * fact_Rota;
			while (TG.VRota.z >= 360) TG.VRota.z -= 360;
			while (TG.VRota.z < 0) TG.VRota.z += 360;
		}
		else if (escal)
		{
			if (incrT <= 0) {
				if (incrT >= -2) incrT = -2;
				incrT = 1 / Log2(-incrT);
			}
			else incrT = Log2(incrT);
			TG.VScal.z = TG.VScal.z * incrT;
			if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
			if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		}
		m_PosDAvall.x = xpos;	m_PosDAvall.y = ypos;
	}
}

// OnMouseWheel: Funció que es crida quan es mou el rodet del mouse. La utilitzem per la 
//				  Visualització Interactiva per modificar el paràmetre R de P.V. (R,angleh,anglev) 
//				  segons el moviment del rodet del mouse.
//      PARAMETRES: -  (xoffset,yoffset): Estructura (x,y) que dóna la posició del mouse 
//							 (coord. pantalla) quan el botó s'ha apretat.
void OnMouseWheel(GLFWwindow* window, double xoffset, double yoffset)
{
	gameState.OnMouseWheel(window, xoffset, yoffset);

	// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };

	// EntornVGI.ImGui: Test si events de mouse han sigut filtrats per ImGui (io.WantCaptureMouse)
	// (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
	ImGuiIO& io = ImGui::GetIO();
	//io.AddMouseButtonEvent(button, true);

// (2) ONLY forward mouse data to your underlying app/game.
	if (!io.WantCaptureMouse) { // <Tractament mouse de l'aplicació>}
		// Funció de zoom quan està activada la funció pan o les T. Geomètriques
		if ((zzoom) || (transX) || (transY) || (transZ))
		{
			OPV.R = OPV.R + yoffset / 4;
			if (OPV.R < 1) OPV.R = 1;
		}
		else if (camera == CAM_NAVEGA && !io.WantCaptureMouse)
		{
			vdir[0] = n[0] - opvN.x;
			vdir[1] = n[1] - opvN.y;
			vdir[2] = n[2] - opvN.z;
			modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
			vdir[0] = vdir[0] / modul;
			vdir[1] = vdir[1] / modul;
			vdir[2] = vdir[2] / modul;
			opvN.x += (yoffset / 4) * vdir[0];		//opvN.x += (zDelta / 4) * vdir[0];
			opvN.y += (yoffset / 4) * vdir[1];		//opvN.y += (zDelta / 4) * vdir[1];
			n[0] += (yoffset / 4) * vdir[0];		//n[0] += (zDelta / 4) * vdir[0];
			n[1] += (yoffset / 4) * vdir[1];		//n[1] += (zDelta / 4) * vdir[1];
		}
	}
}


/* ------------------------------------------------------------------------- */
/*					     TIMER (ANIMACIÓ)									 */
/* ------------------------------------------------------------------------- */
void OnTimer()
{
	// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	if (anima) {
		// Codi de tractament de l'animació quan transcorren els ms. del crono.

		// Crida a OnPaint() per redibuixar l'escena
		//InvalidateRect(NULL, false);
	}
	else if (satelit) {	// OPCIÓ SATÈLIT: Increment OPV segons moviments mouse.
		//OPV.R = OPV.R + m_EsfeIncEAvall.R;
		OPV.alfa = OPV.alfa + m_EsfeIncEAvall.alfa;
		while (OPV.alfa > 360) OPV.alfa = OPV.alfa - 360;	while (OPV.alfa < 0) OPV.alfa = OPV.alfa + 360;
		OPV.beta = OPV.beta + m_EsfeIncEAvall.beta;
		while (OPV.beta > 360) OPV.beta = OPV.beta - 360;	while (OPV.beta < 0) OPV.beta = OPV.beta + 360;

		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint();
	}
}

// ---------------- Entorn VGI: Funcions locals a main.cpp

// Log2: Càlcul del log base 2 de num
int Log2(int num)
{
	int tlog;

	if (num >= 8192) tlog = 13;
	else if (num >= 4096) tlog = 12;
	else if (num >= 2048) tlog = 11;
	else if (num >= 1024) tlog = 10;
	else if (num >= 512) tlog = 9;
	else if (num >= 256) tlog = 8;
	else if (num >= 128) tlog = 7;
	else if (num >= 64) tlog = 6;
	else if (num >= 32) tlog = 5;
	else if (num >= 16) tlog = 4;
	else if (num >= 8) tlog = 3;
	else if (num >= 4) tlog = 2;
	else if (num >= 2) tlog = 1;
	else tlog = 0;

	return tlog;
}


// -------------------- FUNCIONS CORBES SPLINE i BEZIER

// llegir_ptsC: Llegir punts de control de corba (spline o Bezier) d'un fitxer .crv. 
//				Retorna el nombre de punts llegits en el fitxer.
//int llegir_pts(CString nomf)
int llegir_ptsC(const char* nomf)
{
	int i, j;
	FILE* fd;

	// Inicialitzar vector punts de control de la corba spline
	for (i = 0; i < MAX_PATCH_CORBA; i = i++)
	{
		PC_t[i].x = 0.0;
		PC_t[i].y = 0.0;
		PC_t[i].z = 0.0;
	}

	//	ifstream f("altinicials.dat",ios::in);
	//    f>>i; f>>j;
	if ((fd = fopen(nomf, "rt")) == NULL)
	{
		//LPCWSTR texte1 = reinterpret_cast<LPCWSTR> ("ERROR:");
		//LPCWSTR texte2 = reinterpret_cast<LPCWSTR> ("File .crv was not opened");
		//MessageBox(texte1, texte2, MB_OK);
		fprintf(stderr, "ERROR: File .crv was not opened");
		return false;
	}
	fscanf(fd, "%d \n", &i);
	if (i == 0) return false;
	else {
		for (j = 0; j < i; j = j++)
		{	//fscanf(fd, "%f", &corbaSpline[j].x);
			//fscanf(fd, "%f", &corbaSpline[j].y);
			//fscanf(fd, "%f \n", &corbaSpline[j].z);
			fscanf(fd, "%lf %lf %lf \n", &PC_t[j].x, &PC_t[j].y, &PC_t[j].z);

		}
	}
	fclose(fd);

	return i;
}


// Entorn VGI. OnFull_Screen: Funció per a pantalla completa
void OnFull_Screen(GLFWmonitor* monitor, GLFWwindow* window)
{
	//int winPosX, winPosY;
	//winPosX = 0;	winPosY = 0;

	//fullscreen = !fullscreen;

	if (fullscreen) {	// backup window position and window size
		glfwGetWindowPos(window, &winPosX_old, &winPosY_old);
		glfwGetWindowSize(window, &width_old, &height_old);

		// Get resolution of monitor
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		w = mode->width;	h = mode->height;
		// Switch to full screen
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else {	// Restore last window size and position
		glfwSetWindowMonitor(window, nullptr, winPosX_old, winPosY_old, width_old, height_old, mode->refreshRate);
	}
}

// -------------------- TRACTAMENT ERRORS
// error_callback: Displaia error que es pugui produir
void error_callback(int code, const char* description)
{
	//const char* descripcio;
	//int codi = glfwGetError(&descripcio);

 //   display_error_message(code, description);
	fprintf(stderr, "Codi Error: %i", code);
	fprintf(stderr, "Descripcio: %s\n", description);
}


GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		fprintf(stderr, "ERROR %s | File: %s | Line ( %3i ) \n", error.c_str(), file, line);
		//fprintf(stderr, "ERROR %s | ", error.c_str());
		//fprintf(stderr, "File: %s | ", file);
		//fprintf(stderr, "Line ( %3i ) \n", line);
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	fprintf(stderr, "---------------\n");
	fprintf(stderr, "Debug message ( %3i %s \n", id, message);

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             fprintf(stderr, "Source: API \n"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   fprintf(stderr, "Source: Window System \n"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "Source: Shader Compiler \n"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     fprintf(stderr, "Source: Third Party \n"); break;
	case GL_DEBUG_SOURCE_APPLICATION:     fprintf(stderr, "Source: Application \n"); break;
	case GL_DEBUG_SOURCE_OTHER:           fprintf(stderr, "Source: Other \n"); break;
	} //std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               fprintf(stderr, "Type: Error\n"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "Type: Deprecated Behaviour\n"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  fprintf(stderr, "Type: Undefined Behaviour\n"); break;
	case GL_DEBUG_TYPE_PORTABILITY:         fprintf(stderr, "Type: Portability\n"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:         fprintf(stderr, "Type: Performance\n"); break;
	case GL_DEBUG_TYPE_MARKER:              fprintf(stderr, "Type: Marker\n"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          fprintf(stderr, "Type: Push Group\n"); break;
	case GL_DEBUG_TYPE_POP_GROUP:           fprintf(stderr, "Type: Pop Group\n"); break;
	case GL_DEBUG_TYPE_OTHER:               fprintf(stderr, "Type: Other\n"); break;
	} //std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         fprintf(stderr, "Severity: high\n"); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       fprintf(stderr, "Severity: medium\n"); break;
	case GL_DEBUG_SEVERITY_LOW:          fprintf(stderr, "Severity: low\n"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity: notification\n"); break;
	} //std::cout << std::endl;
	//std::cout << std::endl;
	fprintf(stderr, "\n");
}

void LoadVAOsAPB()
{
	// Càrrega dels VAO's per a construir objecte ABP_CUSTOM
	netejaVAOList();						// Neteja Llista VAO.

	// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

	//if (Get_VAOId(GLUT_CUBE) != 0) deleteVAOList(GLUT_CUBE);
	Set_VAOList(GLUT_CUBE, loadglutSolidCube_EBO(1.0));		// Càrrega Cub de costat 1 com a EBO a la posició GLUT_CUBE.

	//if (Get_VAOId(GLU_SPHERE) != 0) deleteVAOList(GLU_SPHERE);
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(0.5, 20, 20));	// Càrrega Esfera a la posició GLU_SPHERE.

	//if (Get_VAOId(GLUT_TEAPOT) != 0) deleteVAOList(GLUT_TEAPOT);
	Set_VAOList(GLUT_TEAPOT, loadglutSolidTeapot_VAO());		// Carrega Tetera a la posició GLUT_TEAPOT.

	//if (Get_VAOId(GLU_CYLINDER) != 0) deleteVAOList(GLU_CYLINDER);
	Set_VAOList(GLUT_CYLINDER, loadgluCylinder_EBO(5.0f, 5.0f, 0.5f, 6, 1));// Càrrega cilindre com a VAO.

	//if (Get_VAOId(GLU_DISK) != 0)deleteVAOList(GLU_DISK);
	Set_VAOList(GLU_DISK, loadgluDisk_EBO(0.0f, 5.0f, 20, 5));	// Càrrega disc com a VAO

	//if (Get_VAOId(GLUT_USER1) != 0)deleteVAOList(GLUT_USER1);
	Set_VAOList(GLUT_USER1, loadgluCylinder_EBO(5.0f, 5.0f, 2.0f, 6, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_TORUS) != 0)deleteVAOList(GLUT_TORUS);
	Set_VAOList(GLUT_TORUS, loadglutSolidTorus_EBO(1.0, 5.0, 20, 20));

	//if (Get_VAOId(GLUT_USER2) != 0)deleteVAOList(GLUT_USER2);	
	Set_VAOList(GLUT_USER2, loadgluCylinder_EBO(1.0f, 0.5f, 5.0f, 60, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_USER3) != 0)deleteVAOList(GLUT_USER3);
	Set_VAOList(GLUT_USER3, loadgluCylinder_EBO(0.35f, 0.35f, 5.0f, 80, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_USER4) != 0)deleteVAOList(GLUT_USER4);
	Set_VAOList(GLUT_USER4, loadgluCylinder_EBO(4.0f, 2.0f, 10.25f, 40, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_USER5) != 0) deleteVAOList(GLUT_USER5);
	Set_VAOList(GLUT_USER5, loadgluCylinder_EBO(1.5f, 4.5f, 2.0f, 8, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_USER6) != 0) deleteVAOList(GLUT_USER6);
	Set_VAOList(GLUT_USER6, loadgluDisk_EBO(0.0f, 1.5f, 8, 1)); // Càrrega disk com a VAO


	// Skybox
			// Càrrega Shader Skybox
	if (!skC_programID) skC_programID = shader_SkyBoxC.loadFileShaders(".\\shaders\\skybox.VERT", ".\\shaders\\skybox.FRAG");

	// Càrrega VAO Skybox Cube
	if (skC_VAOID.vaoId == 0) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);

	if (!cubemapTexture)
	{	// load Skybox textures
		// -------------
		std::vector<std::string> faces;
		faces =
		{ ".\\textures\\skybox\\right.jpg",
			".\\textures\\skybox\\left.jpg",
			".\\textures\\skybox\\top.jpg",
			".\\textures\\skybox\\bottom.jpg",
			".\\textures\\skybox\\front.jpg",
			".\\textures\\skybox\\back.jpg"
		};
		cubemapTexture[SkyBox] = loadCubemap(faces);
	}

}

void LoadTexturesABP()
{
	texturesID[0] = loadIMA_SOIL(".\\textures\\nota.png");
	texturesID[1] = loadIMA_SOIL(".\\textures\\brick.jpg");
	texturesID[2] = loadIMA_SOIL(".\\textures\\sac.JPG");
	texturesID[3] = loadIMA_SOIL(".\\textures\\sorra.jpg");
	texturesID[4] = loadIMA_SOIL(".\\textures\\totxana.jpg");
	texturesID[5] = loadIMA_SOIL(".\\textures\\Vent.bmp");
	texturesID[6] = loadIMA_SOIL(".\\textures\\wood.jpg");

	// Textures cadenat
	texturesID[7] = loadIMA_SOIL(".\\textures\\cadenat\\red.bmp");
	texturesID[8] = loadIMA_SOIL(".\\textures\\cadenat\\green.bmp");
	texturesID[9] = loadIMA_SOIL(".\\textures\\cadenat\\blue.bmp");
	texturesID[10] = loadIMA_SOIL(".\\textures\\cadenat\\black.bmp");

	//Textures menu
	texturesID[11] = loadIMA_SOIL(".\\textures\\menu\\menu_bar.png");

	// for models
	texturesID[20] = loadIMA_SOIL(".\\textures\\furniturebits_texture.png");


	// Textures cadenat numeros
	texturesID[30] = loadIMA_SOIL(".\\textures\\cadenat\\0.bmp");
	texturesID[31] = loadIMA_SOIL(".\\textures\\cadenat\\1.bmp");
	texturesID[32] = loadIMA_SOIL(".\\textures\\cadenat\\2.bmp");
	texturesID[33] = loadIMA_SOIL(".\\textures\\cadenat\\3.bmp");
	texturesID[34] = loadIMA_SOIL(".\\textures\\cadenat\\4.bmp");
	texturesID[35] = loadIMA_SOIL(".\\textures\\cadenat\\5.bmp");
	texturesID[36] = loadIMA_SOIL(".\\textures\\cadenat\\6.bmp");
	texturesID[37] = loadIMA_SOIL(".\\textures\\cadenat\\7.bmp");
	texturesID[38] = loadIMA_SOIL(".\\textures\\cadenat\\8.bmp");
	texturesID[39] = loadIMA_SOIL(".\\textures\\cadenat\\9.bmp");

	// Puzzle 4
	//texturesID[40] = loadIMA_SOIL(".\\textures\\cadenat\\9.bmp");


	//Textures habitació
	texturesID[49] = loadIMA_SOIL(".\\models\\wallpaper2.tga.png");		// Paredes
	texturesID[50] = loadIMA_SOIL(".\\textures\\brick2.bmp");			// Paredes
	texturesID[51] = loadIMA_SOIL(".\\textures\\habitacio\\techo.jpg"); // Techo
	texturesID[52] = loadIMA_SOIL(".\\textures\\habitacio\\suelo.jpg"); // Suelo

	// inventario
	texturesID[61] = loadIMA_SOIL(".\\textures\\inventoryItems\\gema.png");
	//texturesID[62] = loadIMA_SOIL(".\\textures\\inventoryItems\\potion.png");
	//texturesID[63] = loadIMA_SOIL(".\\textures\\inventoryItems\\sword.png");
	texturesID[64] = loadIMA_SOIL(".\\textures\\inventoryItems\\llave.png");
	texturesID[65] = loadIMA_SOIL(".\\textures\\inventoryItems\\nota.png");

	//menu
	texturesID[80] = loadIMA_SOIL(".\\textures\\menu\\start-scene.png");
	texturesID[81] = loadIMA_SOIL(".\\textures\\menu\\main-menu.png");
	texturesID[82] = loadIMA_SOIL(".\\textures\\menu\\game-over.png"); // inventory branch

	//game over
	texturesID[100] = loadIMA_SOIL(".\\textures\\menu\\game_over.png"); // main branch

}

void LoadModelsABP()
{
	std::tuple<int, char*> models[] = {
		std::make_tuple(0, (char*)".\\models\\cactus_small_A.obj"),
		std::make_tuple(1, (char*)".\\models\\bed_double_A.obj"),
		std::make_tuple(2, (char*)".\\models\\book_set.obj"),
		std::make_tuple(3, (char*)".\\models\\chair_A.obj"),
		std::make_tuple(4, (char*)".\\models\\pictureframe_large_B.obj"),
		std::make_tuple(5, (char*)".\\models\\chair_stool.obj"),
		std::make_tuple(6, (char*)".\\models\\couch.obj"),
		std::make_tuple(7, (char*)".\\models\\lamp_standing.obj"),
		std::make_tuple(8, (char*)".\\models\\shelf_A_big.obj"),
		std::make_tuple(9, (char*)".\\models\\table_medium_long.obj"),
		std::make_tuple(10, (char*)".\\models\\gema.obj"),
		//std::make_tuple(11, (char*)".\\models\\lock.obj"),
		std::make_tuple(11, (char*)".\\models\\candado_num.obj"),
		std::make_tuple(111, (char*)".\\models\\candado_simb.obj"),
		std::make_tuple(12, (char*)".\\models\\cuadro.obj"),
		std::make_tuple(13, (char*)".\\models\\estatua.obj"),
		std::make_tuple(14, (char*)".\\models\\ps1_antique_radio.obj"),
		// PSX Gothic Furniture Pack
		std::make_tuple(MODEL_GOTHIC_BED, (char*)".\\models\\bed.obj"),
		std::make_tuple(MODEL_GOTHIC_BENCH1, (char*)".\\models\\bench1.obj"),
		std::make_tuple(MODEL_GOTHIC_BENCH2, (char*)".\\models\\bench2.obj"),
		std::make_tuple(MODEL_GOTHIC_CANDLESTICK, (char*)".\\models\\candlestick.obj"),
		std::make_tuple(MODEL_GOTHIC_CHAIR, (char*)".\\models\\chair.obj"),
		std::make_tuple(MODEL_GOTHIC_DESK, (char*)".\\models\\desk.obj"),
		std::make_tuple(MODEL_GOTHIC_DINNERTABLE, (char*)".\\models\\dinnertable.obj"),
		std::make_tuple(MODEL_GOTHIC_DRESSER, (char*)".\\models\\dresser.obj"),

		// SPOOKY HOUSE PIECES
		std::make_tuple(MODEL_SPOOKY_CABINET, (char*)".\\models\\cabinet.obj"),
		std::make_tuple(MODEL_SPOOKY_CROSS, (char*)".\\models\\cross.obj"),
		std::make_tuple(MODEL_SPOOKY_SIDETABLE, (char*)".\\models\\sidetable.obj"),
		std::make_tuple(MODEL_SPOOKY_PORTRAIT, (char*)".\\models\\portrait.obj"),
		std::make_tuple(MODEL_SPOOKY_RUG, (char*)".\\models\\rug.obj"),
		std::make_tuple(MODEL_SPOOKY_PIANO, (char*)".\\models\\piano.obj"),
		std::make_tuple(MODEL_SPOOKY_PIANO_STOOL, (char*)".\\models\\pianostool.obj"),
		std::make_tuple(MODEL_SPOOKY_OPEN_BOOK, (char*)".\\models\\openbook.obj"),
		std::make_tuple(MODEL_SPOOKY_MUSIC_SHEET, (char*)".\\models\\musicsheet.obj"),
		std::make_tuple(MODEL_SPOOKY_LARGETABLE, (char*)".\\models\\largetable.obj"),
		std::make_tuple(MODEL_SPOOKY_OILYLAMP, (char*)".\\models\\oilylamp.obj"),
		std::make_tuple(MODEL_SPOOKY_DINNER_CHAIR, (char*)".\\models\\dinnerchair.obj"),
		std::make_tuple(MODEL_SPOOKY_CHAIR, (char*)".\\models\\chair.obj"),
		std::make_tuple(MODEL_SPOOKY_CANDLE, (char*)".\\models\\candle.obj"),
		std::make_tuple(MODEL_SPOOKY_BOOK1, (char*)".\\models\\book1.obj"),
		std::make_tuple(MODEL_SPOOKY_BOOK2, (char*)".\\models\\book2.obj"),

		// Habitació
		std::make_tuple(39, (char*)".\\models\\habitacio.obj"),
		std::make_tuple(40, (char*)".\\models\\pared.obj"),
		std::make_tuple(41, (char*)".\\models\\manivela.obj"),
		std::make_tuple(42, (char*)".\\models\\llave.obj"),
		std::make_tuple(43, (char*)".\\models\\historia.obj"),
		std::make_tuple(44, (char*)".\\models\\estatua.obj"),
		std::make_tuple(45, (char*)".\\models\\posters-2.obj")

	};

	printf("Loading OBJ models...\n");
	for (std::tuple<int, char*> model : models)
	{
		printf("Loading model %s ...\n", std::get<1>(model));
		modelos[std::get<0>(model)].LoadModel(std::get<1>(model));
	}
	printf("Finished loading models.\n");

}

int main(void)
{
	//    GLFWwindow* window;
	// Entorn VGI. Timer: Variables
	float time = elapsedTime;
	float now;
	float delta;

	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	// Retrieving main monitor
	primary = glfwGetPrimaryMonitor();

	// To get current video mode of a monitor
	mode = glfwGetVideoMode(primary);

	// Retrieving monito++rs
	//    int countM;
	//   GLFWmonitor** monitors = glfwGetMonitors(&countM);

	// Retrieving video modes of the monitor
	int countVM;
	const GLFWvidmode* modes = glfwGetVideoModes(primary, &countVM);

	const unsigned char* version = (unsigned char*)glGetString(GL_VERSION);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_SAMPLES, antialiasing);	// enable AA
	window = glfwCreateWindow(1280, 720, "Escape room - Projecte ABP VGI 2023", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}


	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Llegir resolució actual de pantalla
	glfwGetWindowSize(window, &width_old, &height_old);

	// Initialize GLEW
	if (GLEW_VERSION_3_3) glewExperimental = GL_TRUE; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		glGetError();	// Esborrar codi error generat per bug a llibreria GLEW
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	int major, minor;
	GetGLVersion(&major, &minor);

	// ------------- Entorn VGI: Configure OpenGL context	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor); // GL4.3

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Si funcions deprecades són eliminades (no ARB_COMPATIBILITY)
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);  // Si funcions deprecades NO són eliminades (Si ARB_COMPATIBILITY)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Creació contexte CORE
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);	// Creació contexte ARB_COMPATIBILITY
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // comment this line in a release build! 


	// ------------ - Entorn VGI : Enable OpenGL debug context if context allows for DEBUG CONTEXT (GL4.3)
	if (GLEW_VERSION_4_3)
	{
		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Initialize Application control varibles
	InitGL();

	// Make the game fullscreen if the variable is enabled :) (ABP)
	OnFull_Screen(primary, window);

	// ------------- Entorn VGI: Callbacks
	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetWindowSizeCallback(window, OnSize);										// - Windows Size in screen Coordinates
	glfwSetFramebufferSizeCallback(window, OnSize);									// - Windows Size in Pixel Coordinates
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)OnMouseButton);			// - Directly redirect GLFW mouse button events
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)OnMouseMove);				// - Directly redirect GLFW mouse position events
	glfwSetScrollCallback(window, (GLFWscrollfun)OnMouseWheel);						// - Directly redirect GLFW mouse wheel events
	glfwSetKeyCallback(window, (GLFWkeyfun)OnKeyDown);								// - Directly redirect GLFW key events
	//glfwSetCharCallback(window, OnTextDown);										// - Directly redirect GLFW char events
	glfwSetErrorCallback(error_callback);											// Error callback
	glfwSetWindowRefreshCallback(window, (GLFWwindowrefreshfun)OnPaint);			// - Callback to refresh the screen

	// Entorn VGI; Timer: Lectura temps
	float previous = glfwGetTime();

	// Entorn VGI.ImGui: Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//Imgui Fonts
	font1 = io.Fonts->AddFontDefault();
	font2 = io.Fonts->AddFontFromFileTTF(".\\fonts\\Creepster-Regular.ttf", 30.0f);
	fontMenu = io.Fonts->AddFontFromFileTTF(".\\fonts\\Crang.ttf", 30.0f);

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

// Entorn VGI.ImGui: Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

// Entorn VGI.ImGui: Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Entorn VGI.ImGui: End Setup Dear ImGui context


	// -- Custom for ABP --
	LoadVAOsAPB();	// Load Object VAOs
	LoadTexturesABP();
	LoadModelsABP();
	c_fons.r = 0;	c_fons.g = 0;	c_fons.b = 0;	c_fons.a = 0;	// Set black background color
	wglSwapIntervalEXT(vsync);	// Enable or disable vsync

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Entorn VGI. Timer: common part, do this only once
		now = glfwGetTime();
		delta = now - previous;
		previous = now;

		// Entorn VGI. Timer: for each timer do this
		time -= delta;
		if ((time <= 0.0) && (satelit || anima)) OnTimer();

		// Poll for and process events
		glfwPollEvents();

		// Entorn VGI.ImGui: Dibuixa menú ImGui
				//draw_Menu_ImGui();

		// ABP: Game update
		gameState.UpdateGame(delta);

		// Crida a OnPaint() per redibuixar l'escena
		OnPaint(window);

		// Entorn VGI.ImGui: Capta dades del menú InGui
				//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Entorn VGI: Activa la finestra actual
		glfwMakeContextCurrent(window);

		// Entorn VGI: Transferència del buffer OpenGL a buffer de pantalla
		glfwSwapBuffers(window);
	}

	// Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Entorn VGI.ImGui: Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);

	// Terminating GLFW: Destroy the windows, monitors and cursor objects
	glfwTerminate();

	if (shaderLighting.getProgramID() != -1) shaderLighting.DeleteProgram();
	if (shaderSkyBox.getProgramID() != -1) shaderSkyBox.DeleteProgram();
	return 0;
}