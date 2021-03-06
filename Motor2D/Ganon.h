#pragma once
#ifndef __GANON_H_
#define __GANON_H_

#include "NPC.h"

#define NUM_POINTS_CIRCLE 180
#define MULTI_P 2

enum GanonPhase { SLEEP = 0, INITIAL, INVINCIBLE, RAGE, DEATH };
enum GanonState { G_WALKING = 0, G_HIT, G_MELEE, G_SPECIAL_1/*Jump&Fire*/, G_SPECIAL_2/*FireBats*/, G_SHIELD, G_DYING, G_ATTACKING, G_IDLE};
enum SpawnType { GREEN, RED };


class Ganon : public NPC
{
public:
	Ganon();

	~Ganon();

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

	// PHASE UPDATES MACHINE -------------
	bool InitialUpdate(float dt);
	bool InvincibleUpdate(float dt);
	bool RageUpdate(float dt);
	bool DeathUpdate(float dt);
	bool InitCombat();
	// ----------------------------------

	// STATE MACHINE ----------------
	void Idle();
	void Walk(float dt);
	void IdleRage();
	void StartMovement();
	void Reorientate();
	void MeleeAttack();
	void SpecialAttack();
	void FireBats();
	void ResetFireBats();
	void StartJump();
	void FireJump();
	void DoJump();
	void ResetJump();
	void Hit();
	void HitRage();
	void Spawn();
	bool Die();
	// -----------------------------

	// Called before quitting
	bool CleanUp();

	void OnCollision(Collider*, Collider*);

	//UTILITY FUNCTIONS -----
	GanonState GetState();
	void SetState(GanonState state);
	void SetAnimState(GanonState state);
	iPoint GetPosinVect(int index);
	bool ChangeRadius_degrade(int radius_to_stop, bool increment);
	void IncreaseDeadMinions();
	void SetDestination();
	//-----------------------------

	bool start_augment = false;
	bool start_awake = false;
	int num_firebats = 0;
	int firebats_dead = 0;

private:
	GanonState state = G_WALKING;
	GanonState anim_state = G_WALKING;
	GanonPhase phase = SLEEP;
	Animation* current_animation = nullptr;

	//HIT TIMERS/VARIABLES ---
	j1Timer HitTime;
	// -------------------------

	//ATTACK VARIABLES ---
	bool StartAttack = false;
	Collider* collision_attack = nullptr;
	// ----------------------

	//MOVEMENT VARIABLES ---
	int canmove = 0;
	// -------------------

	//SLEEP VARIABLES ---
	bool start_dialogue = false;
	//-------------------

	//1ST PHASE VARIABLES --
	j1Timer mov_timer;
	iPoint mov_origin;
	iPoint mov_dest;
	float mov_time;
	bool mov_finished = true;
	// -----------------

	//2ND PHASE VARIABLES ---
	j1Timer spawn_timer;
	int spawn_rate = 2;
	int max_spawns = 6;
	int minions_killed = 0;
	int minions_spawned = 0;
	// -----------------------

	//3D PHASE VARIABLES -----
	j1Timer attack_timer;
	int radius = 30;
	int time_to_create = 0;
	int firebat_rate = 1;
	int new_fire_bat = 30;
	std::vector<iPoint> points;
	int augment_radius = 2;
	GanonState special_attack = G_SPECIAL_1;

	j1Timer jump_timer;
	float jump_time;
	iPoint jump_origin;
	iPoint jump_dest;
	int num_jumps = 0;
	int max_jumps = 3;
	bool jump_finished = false;
	j1Timer explosion_timer;
	j1Timer wait_time;
	// -----------------------

	//DEAD -------------------
	bool video = false;
	bool fade = false;
	bool now_switch = false;
	// -----------------

	bool test = true;

};

#endif //__GANON_H_