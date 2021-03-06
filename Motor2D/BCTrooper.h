#pragma once
#ifndef __BCTROOPER_H_
#define __BCTROOPER_H_

#include "NPC.h"

#define NUM_POINTS_CIRCLE 90
#define MULTI_P 4

enum BCTrooperState { BC_IDLE = 0, BC_WALKING, BC_HIT, BC_DYING, BC_DEFEND };

class P_Follow;

struct Bole
{
	iPoint position;
	int pos_in_vect = 1;
	bool active = false;
	Collider* collision_maze = nullptr;
	P_Follow* particle_maze = nullptr;
};

class BCTrooper : public NPC
{
public:
	BCTrooper();

	~BCTrooper();

	// Called before render is available
	bool Awake(pugi::xml_node&, uint);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	//bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	//bool PostUpdate();

	void Draw();

	// Called before quitting
	bool CleanUp();

	// STATE MACHINE ----------------
	void Idle();
	void Walk(float dt);
	bool Move(float dt);
	void Hit();
	void Defend();
	void Death();
	// -----------------------------
	//Change Radius
	bool ChangeRadius_degrade(int radius_to_stop, bool increment);
	bool ChangeRadius_insta(int radius_to_go, bool increment);

	BCTrooperState GetState() const;
	Collider* GetColliderMaze(uint pos);

	bool Movebyhit(int speed);

	uint GetMazeSize() const;

	//Collision----------------------------------
	void OnCollision(Collider*, Collider*);

private:
	//Animation
	AnimationStruct animation;
	SDL_Rect anim_rect;
	//state
	BCTrooperState state = BC_IDLE;
	SDL_Texture* texture = nullptr;
	SDL_Texture* death_graphics = nullptr;
	std::vector<iPoint> points;
	std::vector<Bole> boles;
	bool next_phase = false;
	int radius = 10;
	int speed_bole = 1;
	int save_speed = 0;
	int canmove = 0;
	//Timers
	j1Timer Change_State;
	j1Timer Wait_attack;
	bool stunned = false;

	bool reset_time = true;

	Collider* collision_maze_2 = nullptr;
	Collider* collision_maze_3 = nullptr;
	Collider* collision_maze_4 = nullptr;
};

#endif//__BCTROOPER_H_
