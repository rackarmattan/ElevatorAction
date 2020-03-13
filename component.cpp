#pragma once

#include "component.h"
#include "game_object.h"
#include "avancezlib.h"
#include "evil_agent.h"
#include "door.h"
#include "car.h"
#include "player.h"

/////////////////////////////////////TAKEN FROM LABS////////////////////////////////////////////////////

void Component::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->engine = engine;
	this->game_objects = game_objects;
}

void RenderComponent::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(engine, go, game_objects);

	currentSprite = engine->createSprite(sprite_name);
}

void RenderComponent::Update(float dt)
{
	if (!go->enabled)
		return;

	if (currentSprite)
		currentSprite->draw(int(go->currentPosition.x), int(go->currentPosition.y), go->width, go->height);
}

void RenderComponent::Destroy()
{
	if (currentSprite != NULL)
		currentSprite->destroy();
	currentSprite = NULL;
}

/////////////////////////////////////BELOW IS MADE BY ME////////////////////////////////////////////////////

void AgentRenderComponent::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_left, const char * sprite_right)
{
	RenderComponent::Create(engine, go, game_objects, sprite_left);
	this->left = currentSprite;
	this->right = engine->createSprite(sprite_right);
}


/*Taked the EvilAgentBehaviourComponent from the game object to check in which direction
it's facing.*/
void AgentRenderComponent::Update(float dt)
{
	EvilAgentBehaviourComponent * agent = go->GetComponent<EvilAgentBehaviourComponent*>();
	if (agent == nullptr) return;
	if (agent->isLeft) {
		currentSprite = left;
	}
	else {
		currentSprite = right;
	}
	RenderComponent::Update(dt);
}

void AgentRenderComponent::Destroy() {
	RenderComponent::Destroy();
	if (left != NULL)
		left->destroy();
	if (right != NULL)
		right->destroy();
	left = NULL;
	right = NULL;
}

void PlayerRenderComponent::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_left, const char * sprite_right, const char * sprite_duck, const char* sprite_idle, const char* sprite_left_shoot, const char* sprite_right_shoot)
{
	AgentRenderComponent::Create(engine, go, game_objects, sprite_left, sprite_right);
	this->duck = engine->createSprite(sprite_duck);
	this->idle = engine->createSprite(sprite_idle);
	this->left_shoot = engine->createSprite(sprite_left_shoot);
	this->right_shoot = engine->createSprite(sprite_right_shoot);
}

/*Get the keystatuses from the engine as well as the PlayerBehaviourComponent from the game object to 
see whether it's inside an elevator, ducking, which direction it's facing and if it's shooting.*/

void PlayerRenderComponent::Update(float dt)
{
	AvancezLib::KeyStatus keys;
	engine->getKeyStatus(keys);
	PlayerBehaviourComponent * player_behaviour = go->GetComponent<PlayerBehaviourComponent*>();
	if (player_behaviour == nullptr) return;
	if (keys.duck && !player_behaviour->insideElevator) {
		currentSprite = duck;
	}
	else if (keys.fire && player_behaviour->isLeft) {
		currentSprite = left_shoot;
	}
	else if (keys.fire && !player_behaviour->isLeft) {
		currentSprite = right_shoot;
	}
	else if (keys.right) {
		currentSprite = right;
	}
	else if (keys.left) {
		currentSprite = left;
	}
	else if(!keys.duck && currentSprite == duck) {
		currentSprite = idle;
	}
	RenderComponent::Update(dt);
}

void PlayerRenderComponent::Destroy() {
	AgentRenderComponent::Destroy();
	if (duck != NULL)
		duck->destroy();
	if (idle != NULL)
		idle->destroy();
	if (left_shoot != NULL)
		left_shoot->destroy();
	if (right_shoot != NULL)
		right_shoot->destroy();
	left_shoot = NULL;
	right_shoot = NULL;
	duck = NULL;
	idle = NULL;
	delete this;
}

void DoorRenderComponent::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_closed, const char* sprite_closed_red, const char * sprite_open)
{
	RenderComponent::Create(engine, go, game_objects, sprite_closed);
	this->closed = currentSprite;
	this->open = engine->createSprite(sprite_open);
	this->red_closed = engine->createSprite(sprite_closed_red);
}

/*Get the DoorBehaviourComponent from the game object to check whether it's open, closed and if it has documents
or not.*/

void DoorRenderComponent::Update(float dt)
{
	DoorBehaviourComponent * door_behaviour = go->GetComponent<DoorBehaviourComponent*>();
	Door * door = (Door*)go;
	if (door_behaviour == nullptr) return;
	if (!door_behaviour->isOpen && door->hasDocuments) {
		currentSprite = red_closed;
	}
	else if (!door_behaviour->isOpen) {
		currentSprite = closed;
	}
	else {
		currentSprite = open;
	}
	RenderComponent::Update(dt);
}

void DoorRenderComponent::Destroy() 
{
	RenderComponent::Destroy();
	if (open != NULL)
		open->destroy();
	if (red_closed != NULL)
		red_closed->destroy();
	if (closed != NULL)
		closed->destroy();
	open = NULL;
	red_closed = NULL;
	closed = NULL;
}


void CarRenderComponent::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_normal, const char * sprite_standing)
{
	RenderComponent::Create(engine, go, game_objects, sprite_normal);
	normal = currentSprite;
	standing = engine->createSprite(sprite_standing);
}

/*Get the CarBehaviourComponent from the game object to see if the car is moving or not.*/

void CarRenderComponent::Update(float dt)
{
	CarBehaviourComponent * car_behaviour = go->GetComponent<CarBehaviourComponent*>();
	if (car_behaviour == nullptr) return;
	if (car_behaviour->move) currentSprite = standing;
	else currentSprite = normal;
	RenderComponent::Update(dt);
}

void CarRenderComponent::Destroy()
{
	RenderComponent::Destroy();
	if (normal != NULL)
		normal->destroy();
	if (standing != NULL)
		standing->destroy();
	normal = NULL;
	standing = NULL;

}

/*Set the collision objects for a CollideComponent.*/

void CollideComponent::Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(engine, go, game_objects);
	this->coll_objects = coll_objects;
}

/*Check where this game objects edges are with respect to the checked objects edges based on width and height.
Return true if there's an intersection somewhere.*/

bool BoxCollideComponent::Intersects(GameObject * otherObject)
{
	double otherMaxX = otherObject->currentPosition.x + otherObject->width;
	double otherMaxY = otherObject->currentPosition.y + otherObject->height;
	double thisMaxX = go->currentPosition.x + go->width;
	double thisMaxY = go->currentPosition.y + go->height;
	bool left = otherMaxX < go->currentPosition.x;
	bool right = otherObject->currentPosition.x > thisMaxX;
	bool above = otherMaxY < go->currentPosition.y;
	bool under = otherObject->currentPosition.y > thisMaxY;

	return !(left || right || above || under);
}


