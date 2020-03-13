#pragma once

#include "component.h"
#include "game_object.h"
#include "player.h"
#include "avancezlib.h"
#include "vector2D.h"
#include <vector>

/*The escape car*/

class Car : public GameObject
{
	
public:

	Player* player;

	virtual void Create(Player * player) {
		this->player = player;
	}


};

/*The escape car's behaviour*/

class CarBehaviourComponent : public Component
{
	Car * car;
	const float move_speed = 200.f;

public:

	bool move;

	virtual void Create(AvancezLib* engine, Car * car, std::set<GameObject*> * game_objects) {
		Component::Create(engine, car, game_objects);

		this->car = car;
	}

	/*If the player is inside this car, the car drives away from the game. When it's outside the screen, 
	a message is sent that the player has won. 
	*/

	virtual void Update(float dt) {
		if (car->IsInside(car->player)&& !move) {
			car->player->enabled = false;
			move = true;
			go->currentPosition.y -= 15;
			go->width = 105;
			go->height = 103;
		}
		if (move) {
			car->currentPosition.x += move_speed * dt;
			if (car->currentPosition.x > 832) {
				car->Send(LEVEL_WIN);
			}
		}
	}

	virtual void Destroy() {
		delete this;
	}
};