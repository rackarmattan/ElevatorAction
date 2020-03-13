#pragma once
#include "vector2D.h"
#include <vector>

enum Message { HIT, TAKEN_DOCUMENTS, REACHED_BOTTOM, TIMES_UP, HIT_BULLET, GAME_OVER, LEVEL_WIN, NO_MSG, QUIT };

class Component;

class GameObject
{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;

public:
	/*Everything except from these three and the method below is from the labs.*/
	Vector2D currentPosition;
	double width;
	double height;


	bool enabled;

	virtual ~GameObject();

	virtual void Create();
	virtual void AddComponent(Component * component);

	virtual void Init();
	virtual void Update(float dt);
	virtual void Destroy();
	virtual void AddReceiver(GameObject *go);
	virtual void Receive(Message m) {}
	void Send(Message m);

	/*Everything except this is from the labs.*/
	virtual bool IsInside(GameObject * otherGo);

	template<typename T>
	T GetComponent() {
		for (Component * c : components) {
			T t = dynamic_cast<T>(c);  //ugly but it works...
			if (t != nullptr) {
				return t;
			}
		}

		return nullptr;
	}
};