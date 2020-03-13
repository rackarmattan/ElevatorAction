#pragma once

#include "component.h"
#include "game_object.h"
#include "bullet.h"
#include "player.h"
#include "avancezlib.h"
#include "vector2D.h"

#include <cstdlib>

/*The evil agents.*/

class EvilAgent : public GameObject
{

public:
	//If the secret time is up, the evil agents should change their behaviour.
	bool times_up;

	virtual void Init()
	{
		SDL_Log("EvilAgent::Init");
		GameObject::Init();
		times_up = false;
	}

	virtual void Receive(Message m) 
	{
		if (m == HIT_BULLET) {
			enabled = false;
		}
		if (m == TIMES_UP) {
			times_up = true;
		}
	}

	virtual void Destroy() {
		SDL_Log("Destroy evil agent");
		delete this;
	}
};

/*The evil agents behaviour.*/

class EvilAgentBehaviourComponent : public Component
{
	const float walk_speed = 60.0f;
	Vector2D velocity;
	float time_fire_pressed;
	float time_went_inside_door;
	float time_turned;
	ObjectPool<Bullet> * bullets_pool;
	float chance_to_shoot = 10;
	const float chance_to_turn = 5;
	const float increased_chance_to_shoot = 60;
	const int leftEdge = 384;
	double rightEdge = 448;
	EvilAgent * evil_agent;

public:
	bool isLeft;
	bool isInsideDoor;

	virtual void Create(AvancezLib* engine, EvilAgent * go, std::set<GameObject*> * game_objects, ObjectPool<Bullet> * rockets_pool)
	{
		Component::Create(engine, go, game_objects);
		this->evil_agent = go;
		this->bullets_pool = rockets_pool;
		velocity.x = 0.0;
		velocity.y = 0.0;
	}

	virtual void Init()
	{
		time_fire_pressed = -10000.f;
		time_turned = -10000.f;
		time_went_inside_door = -10000.f;
		velocity.x = walk_speed;
		isLeft = false;
	}

	/*The agents can shoot if 1.5 seconds has passed.*/

	bool CanFire()
	{
		if (engine->getElapsedTime() - time_fire_pressed > 1.5) {
			time_fire_pressed = engine->getElapsedTime();
			return true;
		}
		return false;
	}

	/*If the next step is inside the screen and the sides, the agent can move.*/

	bool CanMove() {
		double x = go->currentPosition.x + velocity.x;
		return (x > 0 && x < leftEdge) || (x > rightEdge && x < 832);
	}

	/*If the secret time has passed, the evil agents' chance to shoot is increased to 60%.
	
	If two seconds since the last time the agent turned, there's a 10% chance that the agent turns. If the
	agent turned, the time when it did is saved.

	If enough time has passed, there's a (in the beginning) 10 % chance that the agent shoots. If it does,
	the agent requests a bullet from the bullet pool and if there's a bullet available, it inits that 
	bullet in the direction that the agent is facing.
	*/
	virtual void Update(float dt)
	{
		if (evil_agent->times_up) {
			chance_to_shoot = increased_chance_to_shoot;
		}

		if (rand() % 100 < chance_to_turn && (engine->getElapsedTime() - time_turned > 2.0))
		{
			velocity.x *= -1;
			isLeft = !isLeft;
			time_turned = engine->getElapsedTime();
		}

		if (CanFire() && (rand() % 100 < chance_to_shoot))
		{
			// fetches a rocket from the pool and use it in game_objects
			Bullet * bullet = bullets_pool->FirstAvailable();
			if (bullet != NULL)	// rocket is NULL is the object pool can not provide an object
			{
				if (isLeft)
				{
					bullet->Init(go->currentPosition.x, go->currentPosition.y + go->height / 2 - 10, isLeft);
				}
				else
				{
					bullet->Init(go->currentPosition.x + go->width, go->currentPosition.y + go->height / 2 - 10, isLeft);
				}

				game_objects->insert(bullet);
			}
		}

		if (CanMove()) {
			go->currentPosition.x += velocity.x*dt;
		}
	}
};
