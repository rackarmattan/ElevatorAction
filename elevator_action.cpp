#pragma once
#define _CRT_SECURE_NO_WARNINGS

const int SCREEN_WIDTH = 832;
const int SCREEN_HEIGHT = 1000;

#include "avancezlib.h"
#include "game.h"


int main(int argc, char* argv[])
{
	AvancezLib engine;

	engine.init(832, 1000);

	Game game;
	game.Create(&engine);
	game.Init();
	float lastTime = engine.getElapsedTime();
	while (true) {
		float newTime = engine.getElapsedTime();
		float dt = newTime - lastTime;
		lastTime = newTime;

		engine.processInput();
		game.Update(dt);
		game.Draw();
	}

	game.Destroy();
	engine.destroy();
	

	return 0;
}