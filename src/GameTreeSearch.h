#ifndef GAME_TREE_SEARCH_H
#define GAME_TREE_SEARCH_H

#include "Application.h"
#include "Camera.h"
#include "AIBase.h"
#include "ConnectFour.h"

class GameTreeSearch : public Application
{
public:

	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void UpdateGame();

	vec3	m_pick_Pos;

	Game*	m_game;
	AI*		m_ai;

private:

	Camera*	m_camera;

	float	m_dt;

};

class MCTS : public AI
{
public:

	MCTS(int _playouts) : m_playouts(_playouts)
	{

	}

	virtual ~MCTS() {}

	virtual int makeDecision(const Game& game) { return 1; }

private:

	// The number of times it will simulate a game.
	int m_playouts;
	
};

#endif // !GAME_TREE_SEARCH_H