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
	cube_color.r = fmod(cube_color.r * 1.2f, 1.0f);
	cube_color.g = fmod(cube_color.g * 1.1f, 1.0f);
	cube_color.b = fmod(cube_color.b * 1.3f, 1.0f);
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

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	printf("Click pos : %f, %f\n", xpos, ypos);
	ChangeDebugCubePos(vec3(2.5f, 3 * (xpos / 1280), 1 - 3 * (ypos / 720)));
}

void GameState::OnMouseButtonRelease(GLFWwindow* window, int button, int action, int mods)
{
	isMouseDown = false;
}

void GameState::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	// Inspect item with mouse
	if (showItemInspector && isMouseDown)
	{
		item_inspect_rotation.z = 360 - 360*(xpos / width);
		item_inspect_rotation.y = 360*(ypos / height);
	}
}



