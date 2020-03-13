#pragma once

#include "component.h"
#include "game_object.h"

/*Bullets that evil agents and the players can fire*/

class Bullet : public GameObject
{

public:
	bool left;

	virtual void Init(double xPos, double yPos, bool left)
	{
		SDL_Log("Bullet::Init");
		this->left = left;
		currentPosition.x = xPos;
		currentPosition.y = yPos;
		GameObject::Init();

	}

	/*Disable the bullet if it hit something*/
	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT_BULLET)
		{
			enabled = false;
			SDL_Log("Bullet::Hit");
		}
	}
};

/*The bullets' behaviour*/

class BulletBehaviourComponent : public Component
{
	Bullet *bullet;
	const unsigned int speed = 200;

public:

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects) {
		bullet = (Bullet*)go;
		SDL_Log("Create bullet");
		Component::Create(engine, go, game_objects);
	}

	/*If the bullet's direction is left (if it's launched to the left), it moves to the left*/
	void Update(float dt)
	{
		if (bullet->left) {
			bullet->currentPosition.x -= speed * dt;
		}
		else {
			bullet->currentPosition.x += speed * dt;
		}
		if (bullet->currentPosition.x > 832 || bullet->currentPosition.x < 0) {
			bullet->enabled = false;
		}
	}
};

/*Bullet collision*/
class BulletCollideComponent : public BoxCollideComponent
{
public:

	/*Traverse all the collision objects and check if there is an intersection between any of them.*/
	virtual void Update(float dt) {
		for (int i = 0; i < this->coll_objects->pool.size(); i++) {
			GameObject * otherGo = coll_objects->pool[i];
			if (otherGo != go && otherGo->enabled) {
				if (BoxCollideComponent::Intersects(otherGo)) {
					go->Receive(HIT_BULLET);
					otherGo->Receive(HIT_BULLET);
					break;
				}
			}
		}
	}

};