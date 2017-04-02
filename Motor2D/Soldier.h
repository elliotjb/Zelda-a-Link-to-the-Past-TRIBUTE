#pragma once
#ifndef __SOLDIER_H_
#define __SOLDIER_H_

#include "NPC.h"
enum SoldierType {AGGRESSIVE, PASSIVE};
class Soldier : public NPC
{
public:
	Soldier();

	~Soldier();

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

	void AddItem(Item*);

	void Drop_item();

	bool Idle();

	bool Walking();

	bool Move();

	bool Attack();

	bool Die();

	//hit enemy
	bool Movebyhit();
	Direction dir_hit;
	iPoint previus_position;

private:

	SoldierType soldier_type;
	uint id = 0;

	//Soldier Test for Release 0.1
	AnimationStruct animation;
	SDL_Rect anim_rect;
	iPoint pivot;
	//Item* drop;
	bool stunned = false;

	int item_id = 0;
	Item* item_inside = nullptr;
	SDL_Texture* texture = nullptr;

	int timetoplay;

	int timetorun;
	bool reset_run;
	int distance;
	int dis_moved;
	bool reset_distance;
};

#endif //__SOLDIER_H_