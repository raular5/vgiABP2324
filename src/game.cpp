#include "game.h";

GameState::GameState()
{
	// Init debug cube variables
	debug_cube_pos = vec3(0.0f, 0.0f, 0.0f);
	debug_cube_rotation = vec3(0.0f, 0.0f, 0.0f);
	debug_cube_scale = vec3(1.0f, 1.0f, 1.0f);
	cube_color.r = 1.0;
	cube_color.g = 0.5;
	cube_color.b = 0.0;
	cube_color.a = 0.5f;

}


void GameState::ChangeDebugCubePos(vec3 pos)
{
	debug_cube_pos = pos;
	/*cube_color.r = fmod(cube_color.r * 1.2f, 1.0f);
	cube_color.g = fmod(cube_color.g * 1.1f, 1.0f);
	cube_color.b = fmod(cube_color.b * 1.3f, 1.0f);*/
}

void GameState::UpdateGame(float delta)
{
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
		

	//printf("delta: %f \n", delta);
	//debug_cube_rotation.x += 5.0 * delta;
	debug_cube_rotation.y += 20.0 * delta;
	//debug_cube_pos.z = 2.0f*sin(timeAcumm);

	

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

	// Move item inspector
	switch (key) {	// Mejorable, poner en el update
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

	}
	
	// Change item in inspector
	switch (key)
	{
	case GLFW_KEY_TAB: // show inspector
		showItemInspector = !showItemInspector;
		break;

	case GLFW_KEY_0:
		currentItem = ITEM_NONE;
		break;
	case GLFW_KEY_1:
		currentItem = ITEM_KEY;
		break;
	case GLFW_KEY_2:
		currentItem = ITEM_NOTE;
		break;
	case GLFW_KEY_3:
		currentItem = ITEM_CANDLE;
		break;
	default:
		break;
	}
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

	printf("Click pos : %f, %f\n", xpos, ypos);

	// Convert coords
	vec3 ndc = screenToNDC(xpos, ypos, width, height);
	printf("NDC : %f, %f\n", ndc.x, ndc.y);
	vec3 viewSpace = NDCToViewSpace(ndc, *m_ProjectionMatrix);
	printf("View Space : %f, %f\n", viewSpace.x, viewSpace.y);
	vec3 worldPos = ViewSpaceToWorld(viewSpace, *m_ViewMatrix);
	printf("World pos : %f, %f, %f\n", worldPos.x, worldPos.y, worldPos.z);
	worldPos.x = 0;
	worldPos.y *= 500;
	worldPos.z *= 500;
	ChangeDebugCubePos(worldPos);
}

void GameState::OnMouseButtonRelease(GLFWwindow* window, int button, int action, int mods)
{
	isMouseDown = false;
}

void GameState::OnMouseWheel(GLFWwindow* window, double xoffset, double yoffset)
{
	//printf("x: %f, y:%f\n", xoffset, yoffset);
	if (showItemInspector)
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

	// Calcula el desplazamiento del cursor desde la �ltima posici�n
	double xoffset = xpos - previousMouse_xpos;
	double yoffset = previousMouse_ypos - ypos;

	// Actualiza la �ltima posici�n del cursor
	previousMouse_xpos = xpos;
	previousMouse_ypos = ypos;

	const float sensitivity = 0.1f; 
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// Actualiza los �ngulos de rotaci�n
	*angleZ += xoffset; // Rotaci�n horizontal
	double pitch = yoffset;  // Rotaci�n vertical

	// Mant�n la distancia original al origen constante
	float originalDistance = sqrt(opvN->x * opvN->x + opvN->z * opvN->z);

	// Calcular la nueva orientaci�n de la c�mara
	glm::vec3 direction;
	direction.x = cos(glm::radians(*angleZ)) * originalDistance;
	direction.z = sin(glm::radians(*angleZ)) * originalDistance;
	direction.y = opvN->y + sin(glm::radians(pitch)) * originalDistance;

	// Actualizar opvN con la nueva direcci�n
	opvN->x = direction.x;
	opvN->y = direction.y;
	opvN->z = direction.z;

	// Actualizar la matriz de vista
	*m_ViewMatrix = glm::lookAt(glm::vec3(opvN->x, opvN->y, opvN->z), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
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


