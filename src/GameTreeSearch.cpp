#include "GameTreeSearch.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>

bool GameTreeSearch::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	m_game = new ConnectFour();
	m_ai = new RandomAI();

	Gizmos::create();

	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	return true;
}

void GameTreeSearch::ShutDown()
{
	delete m_ai;
	delete m_game;

	Gizmos::destroy();

	Application::ShutDown();
}

bool GameTreeSearch::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	m_dt = glfwGetTime();
	glfwSetTime(0.0);
	m_camera->Update(m_dt);

	UpdateGame();

	return true;
}

void GameTreeSearch::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_game->draw();

	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void GameTreeSearch::UpdateGame()
{
	// keep track of if the mouse has been clicked.
	static bool move_Made = false;

	// Get the mouse position within the game grid.
	double x = 0, y = 0;
	glfwGetCursorPos(m_window, &x, &y);
	m_pick_Pos = m_camera->PickAgainstPlane((float)x, (float)y, vec4(0, 1, 0, 0));

	printf("x %f, y %f\n", x, y);

	// If the game isn't over...
	if (m_game->getCurrentGameState() == Game::UNKNOWN)
	{
		// If it is the user's turn (Player 1).
		if (m_game->getCurrentPlayer() == Game::PLAYER_ONE)
		{
			// If the mouse button is down.
			if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS)
			{
				// If the mouse JUST went down then make a choice.
				if (move_Made == false)
				{
					move_Made = true;

					// Determine which colum was chosen.
					int column = (int)((m_pick_Pos.z + ConnectFour::COLUMNS) / 2);

					// If we clicked in the area for valid column then perform that action.
					if (m_game->isActionValid(column))
					{
						m_game->performAction(column);
					}
				}
				else
				{
					move_Made = false;
				}
			}
		}
		else
		{
			// It is the opponent's turn (Player 2).
			// Use the A.I. to make a turn.
			m_game->performAction(m_ai->makeDecision(*m_game));
		}
	}
}