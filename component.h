#pragma once

#include "vector2D.h"
#include <set>
#include "object_pool.h"
#include "game_object.h"

////////////////////////BELOW IS TAKEN FROM THE LABS///////////////////////////////////////////////

class GameObject;
class AvancezLib;
class Sprite;

class Component
{
protected:
	AvancezLib * engine;	// used to access the engine
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void Receive(Message m) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
protected:

	Sprite* currentSprite;

public:

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name);
	virtual void Update(float dt);
	virtual void Destroy();

	Sprite * GetSprite() { return currentSprite; }
};

class CollideComponent : public Component
{
protected:
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

public:
	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt) = 0;
};

////////////////////////BELOW IS FROM ME///////////////////////////////////////////////

/*Basic agents have only a left and a right sprite*/
class AgentRenderComponent : public RenderComponent
{
protected:
	Sprite * left;
	Sprite * right;

public: 
	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, const char* sprite_left, const char * sprite_right);
	virtual void Update(float dt);
	virtual void Destroy();
};

/*The player agent has four more sprites than the evil agents*/
class PlayerRenderComponent : public AgentRenderComponent
{
	Sprite * duck;
	Sprite * idle;
	Sprite* left_shoot;
	Sprite* right_shoot;

public:
	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, const char* sprite_left, const char * sprite_right, const char * sprite_duck, const char* sprite_idle, const char* sprite_left_shoot, const char* sprite_right_shoot);
	virtual void Update(float dt);
	virtual void Destroy();
};

/*The doors have three different sprites*/
class DoorRenderComponent : public RenderComponent
{
	Sprite * open;
	Sprite * closed;
	Sprite * red_closed;

public: 
	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, const char* sprite_closed, const char* sprite_closed_red, const char * sprite_open);
	virtual void Update(float dt);
	virtual void Destroy();
};

/*The car has two different sprites*/
class CarRenderComponent : public RenderComponent
{
	Sprite * standing;
	Sprite * normal;

public:
	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, const char* sprite_normal, const char* sprite_standing);
	virtual void Update(float dt);
	virtual void Destroy();
};

/*Box collision that has an intersection method that checks wether a collision between two game objects based on width and height happens.*/
class BoxCollideComponent : public CollideComponent
{
protected:
	bool fromTop;
	bool fromLeft;
	bool fromRight;
	bool fromBottom;

public:
	virtual bool Intersects(GameObject * otherObject);
};
