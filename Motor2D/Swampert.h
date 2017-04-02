#pragma once
#ifndef __SWAMPERT_H_
#define __SWAMPERT_H_

#include "Pokemon.h"

class Swampert : public Pokemon
{
public:
	Swampert();

	~Swampert();

	// Called before render is available
	bool Awake(pugi::xml_node&, uint);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	//bool PreUpdate();

	// Called each loop iteration
	bool Update();

	// Called before all Updates
	//bool PostUpdate();

	void Draw();

	// Called before quitting
	bool CleanUp();

	void OnCollision(Collider*, Collider*);

	bool Idle();

	bool Walking();

	bool Move();

	bool Attack();

	bool Idle_IA();

	bool Walking_IA();

	bool Move_IA();

	bool Attack_IA();

	bool CheckOrientation();

	void OnInputCallback(INPUTEVENT, EVENTSTATE);

private:

	//Timer
	int timetoplay;
	int dis_moved = 0;
	int distance = 0;
	bool reset_run;
	bool reset_distance;
	int timetorun = 0;
};

#endif //__SWAMPERT_H_