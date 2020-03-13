#pragma once

#include "component.h"
#include "game_object.h"
#include "player.h"
#include "evil_agent.h"
#include "avancezlib.h"
#include "vector2D.h"
#include <vector>
#include <cstdlib>


/*The doors, their colour is based on whether they have documents inside or not.*/
class Door : public GameObject
{

public: 

	Player * player;
	bool hasDocuments;

	virtual void Create(Player * player, bool hasDocuments) {
		this->player = player;
		this->hasDocuments = hasDocuments;
	}

};

/*The door behaviour*/
class DoorBehaviourComponent : public Component
{
	Door * door;

public: 

	bool isOpen;
	bool playerIsInside;
	bool evilAgentIsInside;
	float time_opened_door;

	virtual void Create(AvancezLib* engine, Door * door, std::set<GameObject*> * game_objects)
	{
		Component::Create(engine, door, game_objects);

		this->door = door;
		time_opened_door = -10000.f;
		isOpen = false;
		playerIsInside = false;
		evilAgentIsInside = false;
	}

	/*If the player is inside the door and the door has documents, check if the player is pressing the 
	jump key and if they aren't in the door already. If so, the player goes inside and a message is sent
	that the documents are taken. Save the time the door was opened.

	If the player is inside the door and is pressing the left or right key and 1.5 seconds have passed since
	the player went inside, the door opens and the player can go out. Now there's no documents left inside the door.

	To make it look like the door opens and closes, the door is open for 0.5 seconds each time it opens.
	*/
	virtual void Update(float dt) 
	{
		AvancezLib::KeyStatus keys;
		engine->getKeyStatus(keys);
		if (door->IsInside(door->player) && door->hasDocuments) {
			if (keys.jump && !playerIsInside) {
				isOpen = true;
				playerIsInside = true;
				door->player->enabled = false;
				door->hasDocuments = false;
				go->Send(TAKEN_DOCUMENTS);
				time_opened_door = engine->getElapsedTime();
			}
		}

		if (playerIsInside && (keys.left || keys.right)) {
			if (engine->getElapsedTime() - time_opened_door > 1.5) {
				door->player->enabled = true;
				isOpen = true;
				playerIsInside = false;
				door->player->enabled = true;
				time_opened_door = engine->getElapsedTime();
			}
		}
		if (isOpen && (engine->getElapsedTime() - time_opened_door > 0.5)) {
			isOpen = false;
		}
	}
};

/*The door collide component, that mainly handles the interaction between the evil agents and the doors.*/

class DoorCollideComponent : public CollideComponent
{
	EvilAgent * agentInside;
	bool isAgentInside;
	Door * door;
	//Chance for the evil agent to go inside this door
	int chance_to_go_in;
	float time_went_inside;
	float time_went_outside;

public:

	virtual void Create(AvancezLib * engine, Door * door, std::set<GameObject*>* game_objects, ObjectPool<GameObject>* coll_objects) {
		CollideComponent::Create(engine, door, game_objects, coll_objects);
		this->door = door;
		time_went_inside = 10000.f;
		time_went_outside = -10000.f;
		chance_to_go_in = 30;
		isAgentInside = false;
	}

	/*If no one already is in this door, the door has no documents and 5 seconds have passed since the
	last agent went outside, check whether any agents are inside the door.

	If an agent is positioned inside the door, there's a 30% chance that the agent goes in. When the agent is inside, it's disabled. The door
	is open and the time when the agent went inside is saved. If this happens, the loop is stopped (as many
	agents can't go inside the same door.

	If an agent is inside the door and has been there for at least three seconds, there's a 30% chance that the agent
	goes outside again. If so, the agent is enabled gain and the door opens. The time when the agent went outside is saved. 
	*/

	virtual void Update(float dt) {
		if(!isAgentInside && !door->hasDocuments && (engine->getElapsedTime() - time_went_outside > 5.0))
		{
			for (auto agent = coll_objects->pool.begin(); agent != coll_objects->pool.end(); agent++) {
				if (door->IsInside(*agent) && rand() % 100 < chance_to_go_in) {
					agentInside = dynamic_cast<EvilAgent*>(*agent);
					DoorBehaviourComponent * door_behaviour = door->GetComponent<DoorBehaviourComponent*>();
					if (agentInside != nullptr && door_behaviour != nullptr) {
						agentInside->currentPosition.x = go->currentPosition.x;
						isAgentInside = true;
						agentInside->enabled = false;
						door_behaviour->isOpen = true;
						door_behaviour->time_opened_door = engine->getElapsedTime();
						time_went_inside = engine->getElapsedTime();
					}
					break;
				}
			}
		}
		else if (isAgentInside && engine->getElapsedTime() - time_went_inside > 3.0f && rand() % 100 < chance_to_go_in ) {
			DoorBehaviourComponent * door_behaviour = door->GetComponent<DoorBehaviourComponent*>();
			if (door_behaviour != nullptr) {
				door_behaviour->isOpen = true;
				door_behaviour->time_opened_door = engine->getElapsedTime();
			}
			isAgentInside = false;
			agentInside->enabled = true;
			time_went_outside = engine->getElapsedTime();
		}

	}

	virtual void Destroy() {
		delete this;
	}

};