#pragma once

#include "component.h"
#include "game_object.h"
#include "bullet.h"
#include "avancezlib.h"
#include "vector2D.h"

/*The player's behaviour*/

class PlayerBehaviourComponent : public Component
{
	float time_fire_pressed;	
	float time_jumped;
	ObjectPool<Bullet> * bullets_pool;
	const float walk_speed = 80.f;
	const float jump_speed = -0.7f;
	const float gravity = 1.7f;

public:
	Vector2D velocity;
	bool isJumping;
	bool isFalling;
	bool isLeft;
	bool insideElevator;

	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Bullet> * rockets_pool)
	{
		Component::Create(engine, go, game_objects);
		this->bullets_pool = rockets_pool;
		velocity.x = 0.0;
		velocity.y = 0.0;
		isJumping = false;
		isLeft = false;
		isFalling = false;
		insideElevator = false;
	}

	virtual void Init()
	{
		time_fire_pressed = -10000.f;
		time_jumped = -10000.f;
	}

	/*If 0.5 seconds has passed since the player shot, the player can shoot again.*/

	bool CanFire()
	{
		if (engine->getElapsedTime() - time_fire_pressed > 0.5) {
			time_fire_pressed = engine->getElapsedTime();
			return true;
		}
		return false;
	}

	/*If 1 second has passed since the player previously jummped, they can jump again.*/

	bool CanJump() {
		if (engine->getElapsedTime() - time_jumped > 1.0) {
			time_jumped = engine->getElapsedTime();
			return true;
		}
		return false;
	}

	/*If the left key is pressed, the velocity in x-direction is the positive walk speed times delta t. If
	the right key is pressed, the opposite happens. If none of them is pressed, the velocity changes to 0.

	If the fire key is pressed, it's checked whether enough time has passed since the last bullet was fired and
	then a bullet from the bullet pool is fetched. If there's a bullet in the pool available, it inits that 
	bullet in the direction that the player is facing.

	If the player isn't inside the elevator, check if the jump keys is pressed. If enough time has passed since
	the last jump, the velocity in the y-direction is set to the jump speed. 

	If the duck key just been pressed, half the height of the player and change the y-coordinate accordingly.
	If the duck key just been released, double the player's height and again change the y-coordinate accordingly.

	If the player is jumping or falling, apply gravity to the velocity.

	Mo´ve the player based on it's velocity. If it has reached the bottom, send a message that it has.
	*/

	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		engine->getKeyStatus(keys);
		if (keys.right) {
			velocity.x = walk_speed*dt;
			isLeft = false;
		}
		else if (keys.left) {
			velocity.x = -walk_speed*dt;
			isLeft = true;
		}
		else {
			velocity.x = 0;
		}
		
		if (keys.fire)
		{
			if (CanFire())
			{
				Bullet * bullet = bullets_pool->FirstAvailable();
				if (bullet != NULL)
				{
					if (isLeft)
					{
						bullet->Init(go->currentPosition.x, go->currentPosition.y + go->height/2 - 10, isLeft);
					}
					else
					{
						bullet->Init(go->currentPosition.x + go->width, go->currentPosition.y + go->height/2 - 10, isLeft);
					}
					
					game_objects->insert(bullet);
				}
			}
		}

		if(!insideElevator)
		{
			if (keys.jump)
			{
				if (CanJump())
				{
					isJumping = true;
					velocity.y = jump_speed;
				}
			}
			engine->getJustPressed(keys);
			if (keys.duck) {
				go->height /= 2;
				go->currentPosition.y += go->height;
			}
			engine->getJustReleased(keys);
			if (keys.duck) {
				go->currentPosition.y -= go->height;
				go->height *= 2;
			}
		}


		if (isJumping || isFalling) {
			velocity.y += dt*gravity;
		}

		Move();

		if (go->currentPosition.y > 1000 - go->height - 50) {
			go->Send(REACHED_BOTTOM);
		}
	}


	void Move()
	{
		go->currentPosition = go->currentPosition.operator+(velocity);
	}

	virtual void Destroy() {
		SDL_Log("Destroy player behaviour");
		delete this;
	}
};


/*The player.*/

class Player : public GameObject
{
public:

	Vector2D initialPosition;

	virtual ~Player() { SDL_Log("Player::~Player"); }

	void Restart() {
		currentPosition = initialPosition;
	}

	virtual void Init()
	{
		SDL_Log("Player::Init");
		GameObject::Init();
		initialPosition = currentPosition;
	}

	virtual void Receive(Message m)
	{
		if(m == HIT_BULLET)
		{
			Send(GAME_OVER);
		}
	}

	virtual void Destroy() {
		GameObject::Destroy();
		SDL_Log("Destroy player");
		delete this;
	}
};

/*The collision detection between the player and the floors.*/

class PlayerCollideComponent : public BoxCollideComponent
{
	Player* player;

public:
	virtual void Create(AvancezLib * engine, Player * player, std::set<GameObject*>* game_objects, ObjectPool<GameObject>* coll_objects)
	{
		CollideComponent::Create(engine, player, game_objects, coll_objects);
		this->player = player;
	}

	/*Check for collision against all the objects (floors). If there's an intersection, check if the 
	player was above, to the left, to the right or beneath the object that it collided with. Correct the 
	player's position based on how it collided with the object.

	If theres no collision at all, the player is falling.	
	*/

	virtual void Update(float dt) {
		PlayerBehaviourComponent * player_behaviour = player->GetComponent<PlayerBehaviourComponent*>();
		if (player_behaviour != nullptr) {
			for (int i = 0; i < coll_objects->pool.size(); i++) {
				GameObject * otherGo = coll_objects->pool[i];
				if (otherGo != go && otherGo->enabled) {
					if (BoxCollideComponent::Intersects(otherGo)) {
						double otherRightEdge = otherGo->currentPosition.x + otherGo->width;
						double otherBottom = otherGo->currentPosition.y + otherGo->height;
						double thisRightEdge = go->currentPosition.x + go->width;
						double thisBottom = go->currentPosition.y + go->height;

						fromTop = thisBottom - 5 < otherGo->currentPosition.y;
						fromLeft = thisRightEdge - 3 < otherGo->currentPosition.x && !fromTop;
						fromRight = go->currentPosition.x + 3 > otherRightEdge && !fromTop;
						fromBottom = go->currentPosition.y + 3 > otherBottom;

						if (fromLeft) {
							player_behaviour->velocity.x = 0;
							player->currentPosition.x = otherGo->currentPosition.x - player->width;
						}
						if (fromRight) {
							player_behaviour->velocity.x = 0;
							player->currentPosition.x = otherGo->currentPosition.x + otherGo->width;
						}
						if (fromBottom) {
							player_behaviour->velocity.y = 0;
							player->currentPosition.y = otherGo->currentPosition.y + otherGo->height;
						}
						else if(fromTop) {
							player_behaviour->velocity.y = 0;
							player->currentPosition.y = otherGo->currentPosition.y - player->height;
							player_behaviour->isJumping = false;
							player_behaviour->isFalling = false;
						}
					}
					else {
						player_behaviour->isFalling = true;
					}
				}
			}
		}
	}

	virtual void Destroy() {
		SDL_Log("Destroy player collide");
		delete this;
	}
};

