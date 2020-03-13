#pragma once

#include "component.h"
#include "game_object.h"
#include "player.h"
#include "floor.h"
#include "avancezlib.h"
#include "vector2D.h"
#include <vector>

/*An elevator. If a secret time has passed, the elevator is slow.*/

class Elevator : public GameObject
{

public:
	Floor * floor;
	Player * player;
	bool slow;

	virtual void Create(Floor * floor, Player * player) {
		this->floor = floor;
		this->player = player;
		slow = false;
	}

	virtual void Receive(Message m) {
		if (m == TIMES_UP) {
			slow = true;
		}
	}
};

/*The elevator behaviour.*/

class ElevatorBehaviourComponent : public Component
{
	Elevator * elevator;
	float move_speed = 60.f;
	//Slow speed if the secret time has passed
	const float slow_speed = 40.f;
	bool moveUp;
	bool moveDown;
	int current_elevator_level = 0;
	//The y-coordinates for the floors in the world
	std::vector<int> levels;
	//Should just the elevator move or should the player move as well
	bool movePlayer;
	const int checkStep = 100;

public:
	virtual void Create(AvancezLib* engine, Elevator * elevator, std::set<GameObject*> * game_objects, std::vector<int> levels)
	{
		Component::Create(engine, elevator, game_objects);
		this->elevator = elevator;
		moveUp = false;
		moveDown = false;
		movePlayer = false;
		this->levels = levels;
	}

	/*Move the elevator (elevator and its floor) to the requested level.*/

	virtual void Move(float step)
	{
		elevator->currentPosition.y += step;
		elevator->floor->currentPosition.y += step;
		if(movePlayer)
		{
			elevator->player->currentPosition.y += step;
		}
		if (current_elevator_level >= 0 && current_elevator_level < levels.size()) {
			if (moveDown && elevator->floor->currentPosition.y >= levels[current_elevator_level]) {
				moveDown = false;
			}
			else if (moveUp && elevator->floor->currentPosition.y <= levels[current_elevator_level]) {
				moveUp = false;
			}
		}
	}

	/*If the elevator is supposed to be slow, set the move speed to slow.
	
	Check whether the player is inside the elevator. If it is, and the elevator isn't currently 
	moving up or down, check if the up or down key is pressed. If the up key is pressed, the current level
	is decreased and move up bool is set to true. If down is pressed, the same is done but the opposite. The
	move player bool is also set to true.

	If the elevator isnt moving, check if the elevator is on the level that the player is. If it isn't,
	move the elevator towards the player's level.
	*/

	virtual void Update(float dt)
	{
		if (elevator->slow) {
			move_speed = slow_speed;
		}

		PlayerBehaviourComponent * player_behaviour = elevator->player->GetComponent<PlayerBehaviourComponent*>();
		if (player_behaviour != nullptr) {
			if (go->IsInside(elevator->player)) {
				player_behaviour->insideElevator = true;
				if(!(moveDown || moveUp)){
					AvancezLib::KeyStatus keys;
					engine->getKeyStatus(keys);
					if (keys.duck && current_elevator_level < levels.size() - 1) {
						current_elevator_level++;
						moveDown = true;
					}
					else if (keys.jump && current_elevator_level > 0) {
						current_elevator_level--;
						moveUp = true;
					}
					movePlayer = true;
				}
			}
			else {
				player_behaviour->insideElevator = false;
				movePlayer = false;
			}
		}
		if (current_elevator_level >= 0 && current_elevator_level <= levels.size() - 1 && !(moveUp||moveDown)) {
			int playerBottomY = (int)elevator->player->currentPosition.y + elevator->player->height;
			if (playerBottomY < levels[current_elevator_level] - checkStep) {
				moveUp = true;
				current_elevator_level--;
			}
			else if (playerBottomY > levels[current_elevator_level] + checkStep) {
				moveDown = true;
				current_elevator_level++;
			}
		}
	
		if (moveUp) {
			Move(-move_speed * dt);
		}
		else if (moveDown) {
			Move(move_speed*dt);
		}
	}
};