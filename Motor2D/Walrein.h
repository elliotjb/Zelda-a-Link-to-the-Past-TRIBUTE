#pragma once
#ifndef __WALREIN_H_
#define __WALREIN_H_

#include "PokemonCombat.h"

class Walrein : public PokemonCombat
{
public:
	Walrein();

	~Walrein();

	// Called before render is available
	bool Awake(pugi::xml_node&);

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

	bool Idle();

	bool Walking(float dt);

	bool Move(float dt);

	void Special_Attack();
	//void ThrowSP(); **Only the special attack is launch.**

	bool Chasing(float dt);

	void OnCollision(Collider*, Collider*);

	bool Movebyhit(int speed);

	int CheckPlayerPos();

private:

	//Timer
	int timetoplay = 0;
	int dis_moved = 0;
	int distance = 0;
	bool reset_run = false;
	bool reset_distance = false;
	int timetorun = 0;
	int num_hits = 0;
	//bool drawThrowSP = false; **Only the special attack is launch.**
	SDL_Texture* passive = nullptr;
	SDL_Rect passive_rect = { 0,409,21,19 };
};

#endif //__WALREIN_H_
