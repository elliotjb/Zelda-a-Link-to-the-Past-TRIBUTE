#pragma once
#ifndef __POKEMONCOMBAT_H_
#define __POKEMONCOMBAT_H_

#include "NPC.h"
#include "j1Window.h"
#include "j1Collision.h"
#include "CombatManager.h"

#define VIEWING_DISTANCE 120
#define ATTACK_DISTANCE 30

enum PokemonCombatState { PC_IDLE = 0, PC_WALKING, PC_ATTACKING, PC_SPECIAL, PC_HIT, PC_DYING, PC_STATIC, PC_STUNNED, PC_COLLISION, PC_CHASING };
enum PokemonParticles { BUBBLE, LEAF };

class PokemonCombat : public NPC
{
public:
	PokemonCombat()
	{}

	virtual ~PokemonCombat()
	{}

	iPoint Getposition() const
	{
		return position;
	}

	PokemonCombatState GetState() const
	{
		return state;
	}
	void SetState(PokemonCombatState s_state)
	{
		state = s_state;
	}
	void SetAnimState(PokemonCombatState a_state)
	{
		anim_state = a_state;
	}

	bool Special_inUse()
	{
		return use_special;
	}

	void ModifyStats(const Item_pokeCombat* items)
	{
		if (items->def_protein > 0)
		{
			defense += items->def_protein * potion_defense;
		}
		if (items->hp_up > 0)
		{
			hp += items->hp_up * potion_hp;
		}
		if (items->x_attack > 0)
		{
			attack += items->x_attack * potion_attack;
			sp_damage += items->x_attack * potion_attack;
		}
	}

	void CheckDeleteColliders()
	{
		if (sp_attack != nullptr && state != PC_SPECIAL)
		{
			sp_attack->to_delete = true;
		}
		if (collision_attack != nullptr && state != PC_ATTACKING)
		{
			collision_attack->to_delete = true;
		}
	}

	bool OrientatePokeLink()
	{
		if (change_dir.ReadSec() >= 0.5)
		{
			iPoint pokemon = App->combatmanager->pokemon_active_link->position;

			Direction prev_dir = direction;
			if (abs(pokemon.x - position.x) < abs(pokemon.y - position.y))
			{
				if (pokemon.y > position.y)
				{
					direction = DOWN;
				}

				else
				{
					direction = UP;
				}
			}

			else
			{
				if (pokemon.x < position.x)
				{
					direction = LEFT;
				}

				else
				{
					direction = RIGHT;
				}
			}
			change_dir.Start();
		}

		return true;
	}

public:

	bool active = false;
	bool dead = false;
	uint cooldown = 0;
	int canmove = 0;

	//target
	PokemonCombat* target = nullptr;

	//special attack
	iPoint range; //.X shows max range, .Y shows distance moved
	//special attack stats
	iPoint sp_start;
	bool sp_attacking = false;
	int sp_direction = 0;
	int sp_speed = 0;
	uint sp_damage = 0;

	uint defense = 0;

	Collider* sp_attack = nullptr;
	Collider* collision_attack = nullptr;
	j1Timer time_stunned;
	j1Timer wait_attack;

	bool use_special = false;

protected:

	PokemonCombatState state = PC_IDLE;
	PokemonCombatState anim_state = PC_IDLE;

	Animation* current_animation = nullptr;

	bool attacker = false;
	bool getdamage = false;

	j1Timer change_dir;
	
private:

	int potion_hp = 20;
	int potion_attack = 4;
	int potion_defense = 2;

};

#endif //__POKEMONCOMBAT_H_
