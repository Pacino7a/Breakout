#include "utils.h"
//#include "texture.h"
//#include "resourceManager.h"
#include <learnGL/config.h>

constexpr int MAJOR_VERSION{ 3 };
constexpr int MINOR_VERSION{ 3 };
constexpr int SCR_WIDTH{ 800 };
constexpr int SCR_HEIGHT{ 600 };
constexpr bool MSAA{ true };

auto g_lastX{ static_cast<double>(SCR_WIDTH) / 2 };
auto g_lastY{ static_cast<double>(SCR_HEIGHT) / 2 };
auto g_deltaTime{ 0.0f };
auto g_firstMoveMouse{ true };
auto g_framebuffer_width{ SCR_WIDTH };
auto g_framebuffer_height{ SCR_HEIGHT };

auto g_keyPressd{ false };
auto g_keyResetPressd{ false };
auto g_keyMenuPressd{ false };

#ifdef __MULTISAMPLE__
auto MULTISAMPLER{ 8 };
#endif // __MULTISAMPLE__

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) // automatically triggered
{
	if (key >= 0 && key < 1024)
	{
		Game* breakout = static_cast<Game*>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS)
			breakout->m_keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			breakout->m_keys[key] = false;
			breakout->m_keyPressed[key] = false;
		}
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // activate DEBUG-OUTPUT mode
	/* MSAA */ // when you use G-Buffer, please turn OFF MSAA!
	glfwWindowHint(GLFW_SAMPLES, MULTISAMPLER);

	GLFWwindow* window{ glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Breakout", NULL, NULL) };
	if (!window)
	{
		std::cerr << "Sorry, we failed to create a GLFW window.\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Sorry, we failed to initialize GLAD.\n";
		return -1;
	}
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glfwSetFramebufferSizeCallback(window, GameUtils::framebufferSizeCallBack);

	double currentFrame{};
	double lastFrame{};
	double lastSecond{};

	Game game{ SCR_WIDTH, SCR_HEIGHT };
	glfwSetWindowUserPointer(window, &game);
	glfwSetKeyCallback(window, key_callback);

	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime(); // currentFrameTime
		g_deltaTime = static_cast<float>(currentFrame - lastFrame); // time elapsed in the last frame
		lastFrame = currentFrame; // save current time for the next calculation

		if (currentFrame - lastSecond > 1)
		{
			lastSecond = currentFrame;
			auto fps{ 1 / g_deltaTime };
			std::ostringstream strStream;
			strStream << "Breakout, Current FPS:" << fps;
			glfwSetWindowTitle(window, strStream.str().c_str());
		}
		
		/* Because we use keyCallBack, so these judges have no meaning for keeping use*/

		//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//{
		//	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//	{
		//		game.m_keys[GLFW_KEY_A] = false;
		//		game.m_keys[GLFW_KEY_D] = false;
		//	}
		//	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//	{
		//		game.m_keys[GLFW_KEY_A] = true;
		//		game.m_keys[GLFW_KEY_D] = false;
		//	}
		//	else
		//	{
		//		game.m_keys[GLFW_KEY_A] = false;
		//		game.m_keys[GLFW_KEY_D] = true;
		//	}
		//}
		//else
		//{
		//	game.m_keys[GLFW_KEY_A] = false;
		//	game.m_keys[GLFW_KEY_D] = false;
		//}

		//if (!g_keyPressd && glfwGetKey(window, GLFW_KEY_SPACE))
		//{
		//	game.m_keys[GLFW_KEY_SPACE] = true;
		//	g_keyPressd = true;
		//}
		//else
		//{
		//	game.m_keys[GLFW_KEY_SPACE] = false;
		//	g_keyPressd = false;
		//}

		//if (!g_keyResetPressd && glfwGetKey(window, GLFW_KEY_R))
		//{
		//	game.m_keys[GLFW_KEY_R] = true;
		//	g_keyResetPressd = true;
		//}
		//else
		//{
		//	game.m_keys[GLFW_KEY_R] = false;
		//	g_keyResetPressd = false;
		//}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		game.processInput(g_deltaTime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		game.render();
		game.update(g_deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}