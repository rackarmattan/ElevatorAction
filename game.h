#pragma once
#include <string>
#include "game_object.h"
#include "object_pool.h"
#include "component.h"
#include "avancezlib.h"
#include "floor.h"
#include "player.h"
#include "evil_agent.h"
#include "elevator.h"
#include "door.h"
#include "car.h"
#include "timer.h"

#include <iostream>

class Game : public GameObject
{
	//The game objects that is order dependent when rendering.
	std::set<GameObject*> game_objects;	

	//The game objects that have to be rendered in the back.
	std::set<GameObject*> background_objects;
	
	AvancezLib* engine;

	ObjectPool<Bullet> agents_bullet_pool;
	ObjectPool<Bullet> player_bullet_pool;

	ObjectPool<Player> players;

	ObjectPool<Floor> floors;

	ObjectPool<EvilAgent> evil_agents;

	ObjectPool<Door> doors;

	unsigned int score = 0;
	unsigned const int score_per_door = 500;
	const int start_level = 100;
	const int share_of_red_doors = 30;

	bool game_over = false;

	Player * player;

	Timer * timer;

	const int level_height = 64 * 2 + 10;

	std::vector<int> levels;

	bool game_won = false;

	const char* win_message = "YOU ESCAPED THE EVIL AGENTS!";
	const char* lose_message = "YOU GOT KILLED";

	/*Creates a player, gives it a render component, a behaviour component and a collision component.*/

	void CreatePlayer() {
		players.Create(1);
		
		PlayerRenderComponent * player_render = new PlayerRenderComponent();
		PlayerBehaviourComponent *player_behaviour = new PlayerBehaviourComponent();
		PlayerCollideComponent *player_collide = new PlayerCollideComponent();

		player = players.pool[0];
		player->Create();
		player->width = 32;
		player->height = 64;
		player->currentPosition.x = 260;
		player->currentPosition.y = 2;
		player_collide->Create(engine, player, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&floors));
		player_render->Create(engine, player, &game_objects, "sprites/left_player.bmp", "sprites/right_player.bmp", "sprites/duck_player.bmp", "sprites/idle_player.bmp", "sprites/left_shoot_player.bmp", "sprites/right_shoot_player.bmp");
		player_behaviour->Create(engine, player, &game_objects, &player_bullet_pool);		

		player->AddComponent(player_collide);
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);

		player->AddReceiver(this);
		game_objects.insert(player);
	}

	/*Creates bullets for the player and the evil agents. All bullets have render components, behaviour components
	and collision components.*/

	void CreateBullets(unsigned int nr_of_evil_bullets, unsigned int nr_of_player_bullets) {
		agents_bullet_pool.Create(nr_of_evil_bullets);
		//Evil agent bullets
		for (auto bullet = agents_bullet_pool.pool.begin(); bullet != agents_bullet_pool.pool.end(); bullet++) {
			
			(*bullet)->Create();
			(*bullet)->width = 10;
			(*bullet)->height = 10;
			
			BulletBehaviourComponent *bullet_behaviour = new BulletBehaviourComponent();
			bullet_behaviour->Create(engine, *bullet, &game_objects);

			RenderComponent *bullet_render = new RenderComponent();
			bullet_render->Create(engine, *bullet, &game_objects, "sprites/bullet.bmp");

			BulletCollideComponent * bullet_player_collide = new BulletCollideComponent();
			//The evil agent bullets can only hurt the player.
			bullet_player_collide->Create(engine, *bullet, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&players));

			(*bullet)->AddComponent(bullet_player_collide);
			(*bullet)->AddComponent(bullet_behaviour);
			(*bullet)->AddComponent(bullet_render);

		}

		player_bullet_pool.Create(nr_of_player_bullets);
		//Player bullets
		for (auto bullet = player_bullet_pool.pool.begin(); bullet != player_bullet_pool.pool.end(); bullet++) {
			(*bullet)->Create();
			(*bullet)->width = 10;
			(*bullet)->height = 10;

			BulletBehaviourComponent * bullet_behaviour = new BulletBehaviourComponent();
			bullet_behaviour->Create(engine, *bullet, &game_objects);

			RenderComponent * bullet_render = new RenderComponent();
			bullet_render->Create(engine, *bullet, &game_objects, "sprites/bullet.bmp");

			BulletCollideComponent * bullet_agent_collide = new BulletCollideComponent();
			//The player bullets can only hurt the evil agents.
			bullet_agent_collide->Create(engine, *bullet, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&evil_agents));

			(*bullet)->AddComponent(bullet_agent_collide);

			(*bullet)->AddComponent(bullet_behaviour);
			(*bullet)->AddComponent(bullet_render);

		}
	}

	/*Creates floor that places themselves as even levels over the screen. All the floors have a render component.*/

	void CreateFloor(unsigned int nr_of_floors) {
		floors.Create(nr_of_floors);

		int x = 0;
		int y = start_level;
		int width = 64;
		int height = 28;
		int countFloorsWidth = 0;
		int countFloorsHeight = 0;
		int levelWidth = 6;
		int maxFloorsOnLevel = 13;

		levels.push_back(y);
		for (int i = 1; i < floors.pool.size(); i++) {
			Floor * floor = floors.pool[i];
			RenderComponent * floor_render = new RenderComponent();

			floor->Create();
			floor->width = width;
			floor->height = height;

			if (countFloorsWidth == levelWidth) {
				x += width;
			}

			if (countFloorsWidth == maxFloorsOnLevel-1) {
				y += level_height;
				x = 0;
				countFloorsWidth = 0;
				if(!(y > 900))
					levels.push_back(y);
			}
			if (i >= floors.pool.size() - 12) {
				y = SCREEN_HEIGHT - height;
			}

			floor->currentPosition.x = x;
			floor->currentPosition.y = y;
			x += width;

			floor_render->Create(engine, floor, &game_objects, "sprites/floor.bmp");

			floor->AddComponent(floor_render);
			game_objects.insert(floor);

			countFloorsWidth++;
		}
		levels.push_back(SCREEN_HEIGHT - height);
	}

	/*Creates a number of evil agents. The agents are evenly placed at each floor level and there's currently 
	four evil agents per level. The evil agents get a render component and a behvaiour component.*/

	void CreateEvilAgents(unsigned int nr_of_evil_agents) {
		evil_agents.Create(nr_of_evil_agents);

		int width = 32;
		int height = 64;
		int x = 10;
		int y = start_level - height;
		int space_between_agents = 200;
		int agent_counter = 0;
		int agents_per_level = 4;

		for (auto agent = evil_agents.pool.begin(); agent != evil_agents.pool.end(); agent++) {
			AgentRenderComponent * agent_render = new AgentRenderComponent();
			EvilAgentBehaviourComponent *agent_behaviour = new EvilAgentBehaviourComponent();

			(*agent)->Create();
			(*agent)->width = width;
			(*agent)->height = height;
			(*agent)->currentPosition.x = x;
			(*agent)->currentPosition.y = y;

			agent_counter++;
			if (agent_counter == agents_per_level) {
				x = 10;
				agent_counter = 0;
				y += level_height;
			}
			else if (agent_counter == 2) {
				x += 2 * space_between_agents;
			}
			else {
				x += space_between_agents;
			}

			agent_render->Create(engine, *agent, &game_objects, "sprites/left_agent.bmp", "sprites/right_agent.bmp");
			agent_behaviour->Create(engine, *agent, &game_objects, &agents_bullet_pool);

			(*agent)->AddComponent(agent_behaviour);
			(*agent)->AddComponent(agent_render);

			timer->AddReceiver(*agent);

			game_objects.insert(*agent);
		}
	}

	/*Creates the elevator. The elevator gets placed at the top-middle of the screen and gets the first floor in the floor object pool.
	The elevator gets a render component and a behaviour component.
	*/

	void CreateElevator() {
		Elevator * elevator = new Elevator();
		ElevatorBehaviourComponent * elevator_behaviour = new ElevatorBehaviourComponent();
		RenderComponent * elevator_render = new RenderComponent();
		
		Floor * floor = floors.pool[0];
		RenderComponent * floor_render = new RenderComponent();
		floor->currentPosition.x = 384;
		floor->currentPosition.y = start_level;
		floor->width = 64;
		floor->height = 28;

		floor->Create();
		floor_render->Create(engine, floor, &game_objects, "sprites/floor.bmp");
		floor->AddComponent(floor_render);

		elevator->width = 64;
		elevator->height = level_height;
		elevator->currentPosition.y = start_level-level_height;
		elevator->currentPosition.x = 384;

		elevator->Create(floor, player);
		elevator_behaviour->Create(engine, elevator, &game_objects, levels);
		elevator_render->Create(engine, elevator, &game_objects, "sprites/elevator_background.bmp");

		elevator->AddComponent(elevator_behaviour);
		elevator->AddComponent(elevator_render);

		timer->AddReceiver(elevator);
		background_objects.insert(elevator);
		game_objects.insert(floor);	
	}

	/*Creates an amount of doors. There are four evenly spread doors at each level. The doors get a render component, 
	a behaviour component and a collision component. There's a 30 % chance that a door is red (i.e. has documents inside).*/

	void CreateDoors(unsigned int nr_of_doors) {
		doors.Create(nr_of_doors);

		const int width = 40;
		const int height = 70;
		int x = 128;
		int y = start_level-height;

		int space_between_doors = 128;
		int door_counter = 0;
		const int doors_per_level = 4;
		const int doors_per_side = 2;

		for(auto door= doors.pool.begin(); door != doors.pool.end(); door++)
		{
			DoorBehaviourComponent * door_behaviour = new DoorBehaviourComponent();
			DoorRenderComponent * door_render = new DoorRenderComponent();
			DoorCollideComponent * door_collide = new DoorCollideComponent();

			(*door)->width = width;
			(*door)->height = height;
			(*door)->currentPosition.y = y;
			(*door)->currentPosition.x = x;

			door_counter++;
			if (door_counter == doors_per_side) {
				x += space_between_doors;
			}
			if (door_counter == doors_per_level) {
				y += level_height;
				door_counter = 0;
				x = space_between_doors;
			}
			else {
				x += space_between_doors;
			}

			bool redDoor = rand() % 100 < share_of_red_doors;

			(*door)->Create(player, redDoor);
			door_render->Create(engine, *door, &game_objects, "sprites/door_closed.bmp", "sprites/red_door_closed.bmp", "sprites/door_open.bmp");
			door_behaviour->Create(engine, *door, &game_objects);
			door_collide->Create(engine, *door, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&evil_agents));
			
			(*door)->AddReceiver(this);
			(*door)->AddComponent(door_render);
			(*door)->AddComponent(door_behaviour);
			(*door)->AddComponent(door_collide);

			background_objects.insert(*door);
		}
	}

	/*Creates the escape car at the bottom of the game world. The car gets a render component and a behaviour component.*/

	void CreateCar() {
		Car *car = new Car();
		CarRenderComponent * car_render = new CarRenderComponent();
		CarBehaviourComponent * car_behaviour = new CarBehaviourComponent();

		car->Create(player);
		car->width = 120;
		car->height = 70;
		car->currentPosition.x = 50;
		car->currentPosition.y = SCREEN_HEIGHT - car->height -28;

		car_render->Create(engine, car, &game_objects, "sprites/car.bmp", "sprites/car_standing.bmp");
		car_behaviour->Create(engine, car, &game_objects);

		car->AddComponent(car_behaviour);
		car->AddComponent(car_render);

		car->AddReceiver(this);
		game_objects.insert(car);
	}


public:

	virtual void Create(AvancezLib* engine)
	{
		SDL_Log("Create game");
		this->engine = engine;
		timer = new Timer();
		timer->Create();
		TimerBehaviourComponent *timer_behaviour = new TimerBehaviourComponent();
		timer_behaviour->Create(engine, timer, &game_objects);
		timer->AddComponent(timer_behaviour);
		game_objects.insert(timer);

		//The player cannot shoot more than three bullets at once, according to the game rules.
		CreateBullets(50, 3);
		CreatePlayer();
		CreateFloor(85);
		CreateEvilAgents(24);
		CreateElevator();
		CreateDoors(24);

		CreateCar();
	}

	virtual void Init()
	{
		SDL_Log("Init game");
		enabled = true;

		for (auto go = background_objects.begin(); go != background_objects.end(); go++) {
			(*go)->Init();
		}

		for (auto go = game_objects.begin(); go != game_objects.end(); go++) {
			(*go)->Init();
		}
	}

	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		engine->getKeyStatus(keys);
		if (keys.esc) {
			Destroy();
			engine->quit();
			return;
		}
		if (keys.restart) {
			player->Restart();
		}

		if (game_over) return;
		if (game_won) return;

		//Update the background objects first so they are rendered before the other objects.

		for (auto go = background_objects.begin(); go != background_objects.end(); go++) {
			(*go)->Update(dt);
		}

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt);
	}

	/*If the game is won, request a fun background from the engine and draw a victory message. If the game
	is lost, request a red background from the engine and draw a game over message. Otherwise, just display
	the current score.*/

	virtual void Draw()
	{
		std::string sScore = "Score: " + std::to_string(score);
		const char* scoreMessage = sScore.c_str();

		engine->drawText(10, 0 ,scoreMessage);

		engine->swapBuffers();
		engine->clearWindow();
		if (game_won) {
			engine->displayMixedBackgroundColors();
			engine->drawText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, win_message);
		}

		if (game_over) {
			engine->setBackgroundColor(255, 0, 0);
			engine->drawText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, lose_message);
		}
	}

	/*If the player has reached the bottom, check if there are any doors with documents left. If there is,
	send the player to the door that has documents that is at the highest level. 
	
	When the car reaches the end of the screen, the game is won. Set the game_won variable to true.

	If the player is shot, set the game_over variable to true.
	*/

	virtual void Receive(Message m)
	{
		if (m == TAKEN_DOCUMENTS) {
			score += score_per_door;
		}
		else if (m == REACHED_BOTTOM) {
			int minY = 100000;
			int x = 10000;
			bool documentsLeft = false;
			for (auto door = doors.pool.begin(); door != doors.pool.end(); door++) {
				if ((*door)->hasDocuments && (*door)->currentPosition.y < minY) {
					minY = (*door)->currentPosition.y;
					x = (*door)->currentPosition.x;
					documentsLeft = true;
				}
			}
			if(documentsLeft) {
				player->currentPosition.y = minY;
				player->currentPosition.x = x;
			}
		}
		else if (m == LEVEL_WIN) {
			game_won = true;
		}
		else if (m == GAME_OVER) {
			game_over = true;
		}
	}

	/*Destroy all the game objects.*/

	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");
		for (auto go = background_objects.begin(); go != background_objects.end(); go++) {
			(*go)->Destroy();
		}

		for (auto go = game_objects.begin(); go != game_objects.end(); go++) {
			(*go)->Destroy();
		}

	}
};