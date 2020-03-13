#pragma once

#include "component.h"
#include "game_object.h"
#include "avancezlib.h"

/*A timer that has a time that passes before the game gets harder.*/

class Timer : public GameObject
{

};

class TimerBehaviourComponent : public Component
{
	const float time_before_chaos = 120.f;

public:

	/*If the time has passed, send a message to the objects that are supposed to change behaviour.*/

	virtual void Update(float dt) {
		if (engine->getElapsedTime() > time_before_chaos) {
			go->Send(TIMES_UP);
			go->enabled = false;
		}
	}
};