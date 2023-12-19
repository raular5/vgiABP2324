#include "game.h";
#include <iostream>

GameState::GameState()
{
	// Init debug cube variables
	debug_cube_pos = vec3(0.0f, 0.0f, 0.0f);
	debug_cube_rotation = vec3(90.0f, 0.0f, 0.0f);
	debug_cube_scale = vec3(0.5f, 0.5f, 0.5f);
	cube_color.r = 1.0;
	cube_color.g = 0.5;
	cube_color.b = 0.0;
	cube_color.a = 0.5f;

	boundaries.push_back(ObjectBoundaries(vec3(10.2f, 8.0f, -1.3f), 1.0f, (char*)"Puzle 1")); // candado simbolos
	boundaries.push_back(ObjectBoundaries(vec3(0.0f, -15.0f, 0.0f), 1.0f, (char*)"Puzle 2")); // estatua
	boundaries.push_back(ObjectBoundaries(vec3(0.0f, 0.0f, 0.0f),   1.0f, (char*)"Puzle 3")); // candado numerico
	boundaries.push_back(ObjectBoundaries(vec3(18.0f, 6.0f, 1.0f),  1.5f, (char*)"Puzle 4")); // cuadro
	boundaries.push_back(ObjectBoundaries(vec3(13.5f, -8.0f, 0.5f), 1.5f, (char*)"nota"));    // ver nota

	inventory.push_back(InventorySlot("Note", 65, 1, 43));

}

bool GameState::IsGemInInventory() {
	// Utilizar std::find_if para buscar una instancia de InventorySlot con itemName igual a "Gem"
	auto gemIterator = std::find_if(inventory.begin(), inventory.end(),
		[](const InventorySlot& slot) { return slot.itemName == "Gem"; });

	// Devolver true si se encontró la gema, false en caso contrario
	return gemIterator != inventory.end();
}

bool GameState::IsKeyInInventory() {
	// Utilizar std::find_if para buscar una instancia de InventorySlot con itemName igual a "Gem"
	auto gemIterator = std::find_if(inventory.begin(), inventory.end(),
		[](const InventorySlot& slot) { return slot.itemName == "Key"; });

	// Devolver true si se encontró la gema, false en caso contrario
	return gemIterator != inventory.end();
}

void GameState::RemoveGemFromInventory() {
	auto it = std::find_if(inventory.begin(), inventory.end(),
		[](const InventorySlot& slot) { return slot.itemName == "Gem"; });

	if (it != inventory.end()) {
		inventory.erase(it);
	}
}

bool GameState::puz1_checkMatch()
{
	return checkMatch(puz1_currentCombination, puz1_correctCombination, puz1_n_Symbols);
}

bool GameState::checkMatch(int* currentCombination, int* correctCombination, int n_Symbols)
{
	for (int i = 0; i < n_Symbols; i++)
	{
		if (currentCombination[i] != correctCombination[i])
			return false;
	}
	return true;
}

void GameState::ChangeDebugCubePos(vec3 pos)
{
	debug_cube_pos = pos;
}

void GameState::UpdateGame(float delta)
{
	switch (*gameScene)
	{
	case SCENE_DEBUG_TEST:
		if (gameOver)
			return;	// No actualizar juego si has perdido

		timeAcumm += delta;
		gameOverCountdownInSeconds -= delta;
		if (false)//Cambiar a  (gameOverCountdownInSeconds <= 0)	 para que funcione
		{
			printf("Game over!\n");
			gameOver = true;
			return;
		}

		debug_cube_rotation.y += 20.0 * delta;
		break;
	default:
		break;
	}

}

void GameState::OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) {
		OnKeyUp(window, key, scancode, action, mods);
		return;
	}

	if (action != GLFW_PRESS) {
		return;
	}
	
	switch (*gameScene) {
	case SCENE_GAME:
		if (key == GLFW_KEY_TAB)
		{
			// Reset state of the scene
			item_inspect_currentItem = 0;
			item_inspect_currentModel = inventory[item_inspect_currentItem].modelID;
			item_inspect_rotation = vec3(0.0f, 0.0f, 0.0f);
			item_inspect_scale = vec3(1.0f, 1.0f, 1.0f);
			ChangeScene(SCENE_ITEM_INSPECT);
			break;
		}
	case SCENE_ITEM_INSPECT:
		switch (key) {
// Move item
		case GLFW_KEY_D:
			item_inspect_rotation.z += 1;
			break;
		case GLFW_KEY_A:
			item_inspect_rotation.z -= 1;
			break;
		case GLFW_KEY_W:
			item_inspect_rotation.y += 1;
			break;
		case GLFW_KEY_S:
			item_inspect_rotation.y -= 1;
			break;
		case GLFW_KEY_Q:
			item_inspect_scale += (0.5f, 0.5f, 0.5f);
			break;
		case GLFW_KEY_E:
			item_inspect_scale -= (0.5f, 0.5f, 0.5f);
			break;
// Change item in inspector
		case GLFW_KEY_RIGHT:
			item_inspect_currentItem = (item_inspect_currentItem + 1) % inventory.size();
			item_inspect_currentModel = inventory[item_inspect_currentItem].modelID;
			break;
		case GLFW_KEY_LEFT:
			item_inspect_currentItem = (item_inspect_currentItem - 1) % inventory.size();
			item_inspect_currentModel = inventory[item_inspect_currentItem].modelID;
			break;
		
		case GLFW_KEY_TAB:
			ChangeScene(SCENE_GAME);
			break;
		default:
			break;

		}


	}
	

	//// Puzle 1:	
	//if (*gameScene == SCENE_PUZLE1)
	//{
	//	switch (key)
	//	{
	//	case GLFW_KEY_1:
	//		puz1_currentCombination[0] = (puz1_currentCombination[0] + 1) % puz1_n_Symbols;
	//		break;
	//	case GLFW_KEY_2:
	//		puz1_currentCombination[1] = (puz1_currentCombination[1] + 1) % puz1_n_Symbols;
	//		break;
	//	case GLFW_KEY_3:
	//		puz1_currentCombination[2] = (puz1_currentCombination[2] + 1) % puz1_n_Symbols;
	//		break;
	//	case GLFW_KEY_4:
	//		puz1_currentCombination[3] = (puz1_currentCombination[3] + 1) % puz1_n_Symbols;
	//		break;
	//	default:
	//		break;
	//	}
	//	puz1_match = puz1_checkMatch();
	//}
	
}

void GameState::OnKeyUp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	

}

void GameState::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	isMouseDown = true;
	

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Actualiza la última posición del cursor
	previousMouse_xpos = xpos;
	previousClick_xpos = xpos;
	previousMouse_ypos = ypos;
	previousClick_ypos = ypos;

	//printf("Click pos : %f, %f\n", xpos, ypos);

	// Convert coords
	vec3 ndc = screenToNDC(xpos, ypos, width, height);
	//printf("NDC : %f, %f\n", ndc.x, ndc.y);
	vec3 viewSpace = NDCToViewSpace(ndc, *m_ProjectionMatrix);
	//printf("View Space : %f, %f\n", viewSpace.x, viewSpace.y);
	vec3 worldPos = ViewSpaceToWorld(viewSpace, *m_ViewMatrix);
	
	worldPos.x = 0;
	worldPos.y *= 500;
	worldPos.z *= 500;
	//printf("World pos : %f, %f, %f\n", worldPos.x, worldPos.y, worldPos.z);

	// Debug
	clickPosWorld_x = worldPos.x;
	clickPosWorld_y = worldPos.y;
	clickPosWorld_z = worldPos.z;

	

	
	

	switch (*gameScene)
	{
	case SCENE_DEBUG_TEST:
	{
		
	}
		break;
	case SCENE_GAME:
	{
		
	}
	break;

	case SCENE_PUZLE1:
		if (worldPos.z > -0.5 && worldPos.z < 0.5)
		{
			if (worldPos.y > -2 && worldPos.y < -1) {
				puz1_currentCombination[0] = (puz1_currentCombination[0] + 1) % puz1_n_Symbols;
				if (!audioEngine) {
					std::cout << "ERROR" << std::endl;
				}
				irrklang::ISound* mySound = audioEngine->play2D("media\\Sonido de Mover Herramientas Efecto de Sonido.ogg", false, false, true);
			}
			else if (worldPos.y > -1 && worldPos.y < 0) {
				puz1_currentCombination[1] = (puz1_currentCombination[1] + 1) % puz1_n_Symbols;
				
				if (!audioEngine) {
					std::cout << "ERROR" << std::endl;
				}
				irrklang::ISound* mySound = audioEngine->play2D("media\\Sonido de Mover Herramientas Efecto de Sonido.ogg", false, false, true);
			}
			else if (worldPos.y > 0 && worldPos.y < 1) {
				puz1_currentCombination[2] = (puz1_currentCombination[2] + 1) % puz1_n_Symbols;
				
				if (!audioEngine) {
					std::cout << "ERROR" << std::endl;
				}
				irrklang::ISound* mySound = audioEngine->play2D("media\\Sonido de Mover Herramientas Efecto de Sonido.ogg", false, false, true);
			}
			else if (worldPos.y > 1 && worldPos.y < 2) {
				puz1_currentCombination[3] = (puz1_currentCombination[3] + 1) % puz1_n_Symbols;
				if (!audioEngine) {
					std::cout << "ERROR" << std::endl;
				}
				irrklang::ISound* mySound = audioEngine->play2D("media\\Sonido de Mover Herramientas Efecto de Sonido.ogg", false, false, true);

			}
		
	


			puz1_match = puz1_checkMatch();
			if (puz1_match) 
			{ 
				inventory.push_back(InventorySlot("Gem", 61, 1, 10));
				puz1_complete = true;
				//audioEngine->drop();
				ChangeScene(SCENE_GAME); 
			}
		}
		break;
	case SCENE_PUZLE2:
		if (!IsGemInInventory()) ChangeScene(SCENE_GAME);
		else {
		//else if (puz2_hasPickedGem && worldPos.y > -0.5 && worldPos.y < 0.5 && worldPos.z < 0.5 && worldPos.z > -0.5){ // Click on statue
		//else if (worldPos.y > -0.5 && worldPos.y < 0.5 && worldPos.z < 0.5 && worldPos.z > -0.5){ // Click on statue

			if (!audioEngine) {
				std::cout << "ERROR" << std::endl;
			}
			irrklang::ISound* mySound = audioEngine->play2D("media\\MUSICA DE TERROR.ogg", false, false, true);
			
			RemoveGemFromInventory();
			puz2_complete = true;
			//audioEngine->drop();
			

		}
		break;
			
	case SCENE_PUZLE3:
		if (worldPos.z > -0.5 && worldPos.z < 0.5)
		{
			if (worldPos.y > -2 && worldPos.y < -1)
			{
				puz3_currentCombination[0] = (puz3_currentCombination[0] + 1) % 10;
				irrklang::ISound* mySound = audioEngine->play2D("media\\Sonido de Mover Herramientas Efecto de Sonido.ogg", false, false, true);
			}
			else if (worldPos.y > -1 && worldPos.y < 0)
			{
				puz3_currentCombination[1] = (puz3_currentCombination[1] + 1) % 10;
				irrklang::ISound* mySound = audioEngine->play2D("media\\Sonido de Mover Herramientas Efecto de Sonido.ogg", false, false, true);
			}
			else if (worldPos.y > 0 && worldPos.y < 1)
			{
				puz3_currentCombination[2] = (puz3_currentCombination[2] + 1) % 10;
				irrklang::ISound* mySound = audioEngine->play2D("media\\Sonido de Mover Herramientas Efecto de Sonido.ogg", false, false, true);
			}
			else if (worldPos.y > 1 && worldPos.y < 2)
			{
				puz3_currentCombination[3] = (puz3_currentCombination[3] + 1) % 10;
				irrklang::ISound* mySound = audioEngine->play2D("media\\Sonido de Mover Herramientas Efecto de Sonido.ogg", false, false, true);
			}

			puz3_match = checkMatch(puz3_currentCombination, puz3_correctCombination, puz3_n_Symbols);
			if (puz3_match) { 
				puz3_complete = true;
				//audioEngine->drop();
				//drawParetFalsa = false;
				ChangeScene(SCENE_PUZLE6);
			}
		}
		break;
	case SCENE_PUZLE4:
		if (puz4_hasMovedFrame && worldPos.y > 0 && worldPos.y < 1.25 && worldPos.z < -1.5 && worldPos.z > -2.5) {// Click on key
			puz4_hasPickedKey = true;
			//audioEngine->drop();
			inventory.push_back(InventorySlot("Key", 64, 1, 42));
		}
		else if (worldPos.y > -2.5 && worldPos.y < 2.5 && worldPos.z < 2.5 && worldPos.z > -2.5) // Click on frame
		{
			if (puz4_hasPickedKey) { ChangeScene(SCENE_GAME); }	// Go back to the game
			else { puz4_hasMovedFrame = !puz4_hasMovedFrame; }
			
		}
			
	}

}

void GameState::OnMouseButtonRelease(GLFWwindow* window, int button, int action, int mods)
{
	isMouseDown = false;

	

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (xpos != previousClick_xpos || ypos != previousClick_ypos) return;

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// RAYCAST
	glm::vec3 rayDirection = getRayDirection(xpos, ypos, width, height, *m_ViewMatrix, *m_ProjectionMatrix);

	switch (*gameScene)
	{
	case SCENE_DEBUG_TEST:
	{
		ObjectBoundaries boundariesTestScene[] = {
			ObjectBoundaries(vec3(0.0f, -4.0f, 0.0f), 1.0f, (char*)"left"),
			ObjectBoundaries(vec3(0.0f,  0.0f, 0.0f), 1.0f, (char*)"center"),
			ObjectBoundaries(vec3(0.0f,  4.0f, 0.0f), 1.0f, (char*)"right"),

		};

		for (const ObjectBoundaries& b : boundariesTestScene)
		{
			if (checkRayIntersection(glm::vec3(opvN->x, opvN->y, opvN->z), rayDirection, b.position, b.radius)) {
				printf("Clicked on object %s \n", b.name);
			}
		}
	}
	break;
	case SCENE_GAME:
	{
		char* hit = nullptr;
		for (const ObjectBoundaries& b : boundaries)
		{
			if (checkRayIntersection(glm::vec3(opvN->x, opvN->y, opvN->z), rayDirection, b.position, b.radius)) {
				printf("Clicked on object %s \n", b.name);
				hit = b.name;
				break;
			}
		}

		if (hit == nullptr)	break;
		else if (hit == "Puzle 1" && !puz1_complete) ChangeScene(SCENE_PUZLE1);
		else if (hit == "Puzle 2") ChangeScene(SCENE_PUZLE2);
		else if (hit == "Puzle 3" && !puz3_complete) ChangeScene(SCENE_PUZLE3);
		else if (hit == "Puzle 4") ChangeScene(SCENE_PUZLE4);
		//else if (hit == "Puzle 5") ChangeScene(SCENE_PUZLE5);
		else if (hit == "Puzle 6") ChangeScene(SCENE_PUZLE6);
		else if (hit == "Win" && IsKeyInInventory()) ChangeScene(SCENE_WIN);
	}
	break;
	}
}

void GameState::OnMouseWheel(GLFWwindow* window, double xoffset, double yoffset)
{
	//printf("x: %f, y:%f\n", xoffset, yoffset);
	if (*gameScene == SCENE_ITEM_INSPECT)
	{
		if(yoffset > 0)
			item_inspect_scale *= 1.1f;
		else
			item_inspect_scale *= 0.9f;
	}
}

void GameState::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	if (!isMouseDown) {
		return;
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Calcula el desplazamiento del cursor desde la última posición
	double xoffset = xpos - previousMouse_xpos;
	double yoffset = previousMouse_ypos - ypos;

	// Actualiza la última posición del cursor
	previousMouse_xpos = xpos;
	previousMouse_ypos = ypos;

	// Inspect item with mouse
	if (*gameScene == SCENE_ITEM_INSPECT)
	{
		item_inspect_rotation.z += 360 * xoffset / width;
		item_inspect_rotation.y += 360 * yoffset / height;
		return;
	}

	// GIRO CAMARA PRIMERA PERSONA CON MOUSE
	if (enableCameraRotation)
	{
		GLdouble vdir[3] = { 0, 0, 0 };
		double modul = 0;

		// Entorn VGI: Controls de moviment de navegació
		vdir[0] = n[0] - opvN->x;
		vdir[1] = n[1] - opvN->y;
		vdir[2] = n[2] - opvN->z;
		modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
		vdir[0] = vdir[0] / modul;
		vdir[1] = vdir[1] / modul;
		vdir[2] = vdir[2] / modul;


		//angleZ += fact_pan;
		(*angleZ) = xoffset * 0.05;
		//printf("%f\n", *angleZ);

		//(*angleZ) = (*angleZ) % 360;
		n[0] = vdir[0]; // n[0] - opvN.x;
		n[1] = vdir[1]; // n[1] - opvN.y;
		n[0] = n[0] * cos((*angleZ) * PI / 180) - n[1] * sin((*angleZ) * PI / 180);
		n[1] = n[0] * sin((*angleZ) * PI / 180) + n[1] * cos((*angleZ) * PI / 180);
		n[0] = n[0] + opvN->x;
		n[1] = n[1] + opvN->y;
	}
	
	if (*gameScene == SCENE_PUZLE6) {
		float normalized_x = (2 * xpos / width)  -1.0f;
		float normalized_y = 1.0f - (2 * ypos / height);
		vec2 normalized = glm::normalize(vec2(normalized_x, normalized_y));
		float rot = acos(normalized.x);
		if (normalized.y < 0)
			rot = -rot;
		//printf("Rotation mouse: %f\n", rot);
		puz6_progress += abs(rot - puz6_rotation);
		printf("Progress: %f\n", puz6_progress);
		puz6_rotation = rot;

		if (puz6_progress > 120.0f) {
			drawParetFalsa = false;
			boundaries.push_back(ObjectBoundaries(vec3(0.0f, -2.0f, 0.0f), 1.0f, (char*)"Win"));
			ChangeScene(SCENE_GAME);
			
		}
	}
}

void GameState::ChangeScene(int scene)
{
	switch (scene) {
	case SCENE_ITEM_INSPECT:
	case SCENE_PUZLE1:
	case SCENE_PUZLE2:
	case SCENE_PUZLE3:
	case SCENE_PUZLE4:
	case SCENE_WIN:
	case SCENE_PUZLE6:
		enableCameraRotation = false;
		break;
	case SCENE_GAME:
		enableCameraRotation = true;
		break;
	}
	(*gameScene) = scene;
}


// Coord
glm::vec3 screenToNDC(float screenX, float screenY, float screenWidth, float screenHeight)
{
	float ndcX = (2.0f * screenX / screenWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * screenY / screenHeight);
	return glm::vec3(ndcX, ndcY, 0.0f);
}

glm::vec3 NDCToViewSpace(vec3 ndcCoords, mat4 ProjectionMatrix)
{
	glm::mat4 inverseProjectionMatrix = glm::inverse(ProjectionMatrix);
	glm::vec4 viewCoords = inverseProjectionMatrix * glm::vec4(ndcCoords, 1.0);
	glm::vec3 viewSpaceCoords = viewCoords / viewCoords.w;
	return viewSpaceCoords;
}

glm::vec3 ViewSpaceToWorld(vec3 viewSpaceCoords, mat4 ViewMatrix)
{
	glm::mat4 inverseViewMatrix = glm::inverse(ViewMatrix);
	glm::vec4 worldCoords = inverseViewMatrix * glm::vec4(viewSpaceCoords, 1.0);
	glm::vec3 worldSpaceCoords = glm::vec3(worldCoords);
	return worldSpaceCoords;
}

// Raycast
glm::vec3 getRayDirection(float mouseX, float mouseY, int screenWidth, int screenHeight, mat4 viewMatrix, mat4 projectionMatrix) {
	// Convert mouse coordinates to NDC
	float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY) / screenHeight;

	// Create ray in clip space
	glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

	// Inverse projection matrix
	glm::mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);

	// Inverse view matrix
	glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);

	// Transform ray to world space
	glm::vec4 rayEye = inverseProjectionMatrix * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

	glm::vec4 rayWorld = inverseViewMatrix * rayEye;
	glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

	return rayDirection;
}

bool checkRayIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 objectPosition, float objectRadius) {
	// Perform intersection test based on your object's bounding volume (e.g., sphere)

	// Example: Check intersection with a sphere
	glm::vec3 toObject = objectPosition - rayOrigin;
	float distance = glm::dot(toObject, rayDirection);

	if (distance < 0) {
		// The object is behind the ray
		return false;
	}

	glm::vec3 pointOnRay = rayOrigin + distance * rayDirection;
	float distanceToCenter = glm::distance(pointOnRay, objectPosition);

	return distanceToCenter <= objectRadius;
}
