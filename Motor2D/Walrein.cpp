#include "Walrein.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Gui.h"
#include "j1GuiEntity.h"
#include "j1Audio.h"
#include "j1GuiElements.h"
#include "CombatManager.h"

Walrein::Walrein()
{
	type = CREATURE;
}

Walrein::~Walrein()
{
}


bool Walrein::Awake(pugi::xml_node &conf)
{
	std::string temp = conf.attribute("dir").as_string("");
	if (temp == "up")
		direction = UP;
	else if (temp == "down")
		direction = DOWN;
	else if (temp == "left")
		direction = LEFT;
	else
		direction = RIGHT;

	cooldown = conf.attribute("cooldown").as_int(0);
	hp = conf.attribute("hp").as_int(0);
	attack = conf.attribute("attack").as_int(0);
	speed = conf.attribute("speed").as_int(0);
	name = conf.attribute("name").as_string("");
	position.x = conf.attribute("pos_x").as_int(0);
	position.y = conf.attribute("pos_y").as_int(0);
	active = conf.attribute("active").as_bool(false);
	sp_damage = conf.attribute("special_attack").as_int(0);
	defense = conf.attribute("defense").as_int(0);
	passive = App->tex->Load("textures/AnimationsAndEffects.png"); //LOOK at this, not good to load the full texture (already loaded in sceptyle)
	return true;
}

bool Walrein::Start()
{
	state = PC_IDLE;
	anim_state = PC_IDLE;
	scale = App->win->GetScale();
	offset_x = 7;
	offset_y = 17;
	timetoplay = SDL_GetTicks();
	collision_feet = App->collision->AddCollider({ position.x - offset_x, position.y - offset_y, 15, 15 }, COLLIDER_POKECOMBAT, this);
	timetoplay = SDL_GetTicks();
	reset_distance = false;
	sp_attacking = false;
	reset_run = true;
	return true;
}

bool Walrein::Update(float dt)
{
	// STATE MACHINE ------------------
	if (App->scene->gamestate == INGAME)
	{
		//pokemon controlled by player
		switch (state)
		{
		case PC_IDLE:
		{
			Idle();
			break;
		}
		case PC_WALKING:
		{
			Walking(dt);
			break;
		}
		case PC_CHASING:
		{
			OrientatePokeLink();
			Chasing(dt);
			break;
		}
		case PC_SPECIAL:
		{
			Special_Attack();
			break;
		}
		case PC_HIT:
		{
			Movebyhit(4);
			break;
		}
		case PC_COLLISION:
		{
			Movebyhit(1);
			break;
		}
		default:
		{
			break;
		}

		}

	}
	else if (App->scene->gamestate == INMENU)
	{

	}

	if (CheckPlayerPos() < VIEWING_DISTANCE && state == PC_WALKING)
	{
		state = PC_CHASING;
	}

	if (CheckPlayerPos() < ATTACK_DISTANCE && (state == PC_WALKING || state == PC_CHASING) && wait_attack.ReadSec() > 0.5)
	{
		OrientatePokeLink();
		state = PC_SPECIAL;
		anim_state = PC_SPECIAL;
		current_animation = App->anim_manager->GetAnimation(anim_state, direction, WALREIN);
		current_animation->Reset();
	}
	
	//Check for passive draw
	switch (num_hits)
	{
	case 0:
		if (App->combatmanager->pokemon_active_link->GetState() == PC_STUNNED)
		{
			passive_rect = { 0,429,21,19 };
		}
		else
		{
			passive_rect = { 0,409,21,19 };
		}
		break;
	case 1:
		passive_rect = { 22,409,21,19 };
		break;
	case 2:
		passive_rect = { 43,409,21,19 };
		break;
	}
	//Collision follow the player
	collision_feet->SetPos(position.x - offset_x, position.y - offset_y);
	return true;
}

void Walrein::Draw()
{
	App->anim_manager->Drawing_Manager(anim_state, direction, position, WALREIN);
	App->render->Blit(passive, App->combatmanager->pokemon_active_link->position.x-10, App->combatmanager->pokemon_active_link->position.y , &passive_rect);
}

bool Walrein::CleanUp()
{
	return true;
}

void Walrein::OnCollision(Collider* c1, Collider* c2)
{
	if (c1 != nullptr && c2 != nullptr)
	{
		PokemonCombat* pokemon_1 = (PokemonCombat*)c1->callback;
		PokemonCombat* pokemon_2 = (PokemonCombat*)c2->callback;
		if (pokemon_1 != nullptr && pokemon_2 != nullptr)
		{
			if (pokemon_1->active && pokemon_2->active && pokemon_1 != pokemon_2)
			{
				if (c1 == sp_attack && c2->type == COLLIDER_POKECOMBAT && getdamage == false)
				{
					num_hits++;
					if (num_hits < 3)
					{
						pokemon_2->knockback_time.Start();
						pokemon_2->hp -= sp_damage;
						getdamage = true;
						App->scene->pokecombat->GetDamage(sp_damage, true);
						pokemon_2->SetState(PC_HIT);
						pokemon_2->SetAnimState(PC_HIT);
						pokemon_2->dir_hit = c1->callback->direction;
						pokemon_2->prev_position = pokemon_2->position;
					}
					else
					{
						num_hits = 0;
						pokemon_2->time_stunned.Start();
						pokemon_2->hp -= sp_damage;
						getdamage = true;
						App->scene->pokecombat->GetDamage(sp_damage, true);
						pokemon_2->SetState(PC_STUNNED);
						pokemon_2->SetAnimState(PC_HIT);
						pokemon_2->dir_hit = c1->callback->direction;
						pokemon_2->prev_position = pokemon_2->position;
					}

				}
			}
		}
	}
}

bool Walrein::Idle()
{
	if (reset_run)
	{
		timetorun = SDL_GetTicks();
		reset_run = false;
	}
	else
	{
		if (SDL_GetTicks() - timetorun > 200)
		{
			int direc_select = rand() % 4 + 1;
			if (direc_select == 1)
			{
				direction = UP;
			}
			else if (direc_select == 2)
			{
				direction = DOWN;
			}
			else if (direc_select == 3)
			{
				direction = LEFT;
			}
			else if (direc_select == 4)
			{
				direction = RIGHT;
			}
			state = PC_WALKING;
			anim_state = PC_WALKING;
			orient_time.Start();
			reset_distance = true;
		}
	}
	return true;
}

bool Walrein::Walking(float dt)
{
	walking = false;
	if (reset_distance)
	{
		distance = rand() % 60 + 20;
		dis_moved = 0;
		reset_distance = false;
	}
	if (canmove % 3 != 0)
	{
		Move(dt);
	}
	else
	{
		walking = true;
	}
	if (canmove > 500)
	{
		canmove = 0;
	}
	canmove++;


	if (dis_moved >= distance)
	{
		walking = false;
		reset_run = true;
	}


	if (walking == false)
	{
		state = PC_IDLE;
		anim_state = PC_IDLE;
	}

	else
	{
		state = PC_WALKING;
		anim_state = PC_WALKING;
	}
	return true;
}

bool Walrein::Move(float dt)
{
	//App->pathfinding->CreatePath(position, target->Getposition());
	if (direction == LEFT)
	{
		if (App->map->MovementCost(collision_feet->rect.x - speed, collision_feet->rect.y, collision_feet->rect.w, collision_feet->rect.h, LEFT) == 0)
		{
			position.x -= ceil(speed * dt);
			dis_moved++;
		}
		else
		{
			//Function to change direction
			dis_moved++;
		}
		walking = true;
	}

	if (direction == RIGHT)
	{
		if (App->map->MovementCost(collision_feet->rect.x + collision_feet->rect.w + speed, collision_feet->rect.y, collision_feet->rect.w, collision_feet->rect.h, RIGHT) == 0)
		{
			position.x += ceil(speed * dt);
			dis_moved++;
		}
		else
		{
			dis_moved++;
		}
		walking = true;
	}
	if (direction == UP)
	{
		if (App->map->MovementCost(collision_feet->rect.x, collision_feet->rect.y - speed, collision_feet->rect.w, collision_feet->rect.h, UP) == 0)
		{
			position.y -= ceil(speed * dt);
			dis_moved++;
		}
		else
		{
			dis_moved++;
		}
		walking = true;
	}
	if (direction == DOWN)
	{
		if (App->map->MovementCost(collision_feet->rect.x, collision_feet->rect.y + collision_feet->rect.h + speed, collision_feet->rect.w, collision_feet->rect.h, DOWN) == 0)
		{
			position.y += ceil(speed * dt);
			dis_moved++;
		}
		else
		{
			dis_moved++;
		}
		walking = true;
	}
	return true;
}

void Walrein::Special_Attack()
{
	if (attacker)
	{
		if (current_animation->Finished())
		{
			App->collision->EraseCollider(sp_attack);
			sp_attack = nullptr;
			attacker = false;
			current_animation->Reset();
			current_animation = nullptr;
			state = PC_IDLE;
			anim_state = PC_IDLE;
			getdamage = false;
			wait_attack.Start();
		}
	}
	else
	{
		attacker = true;
		App->audio->PlayFx(31);
		if (direction == UP)
		{
			sp_attack = App->collision->AddCollider({ position.x - 11, position.y - 35, 22, 8 }, COLLIDER_POKEMON_SPECIAL_ATTACK, this);
			App->audio->PlayFx(10);
		}
		else if (direction == RIGHT)
		{
			sp_attack = App->collision->AddCollider({ position.x + 14, position.y - 26, 8, 22 }, COLLIDER_POKEMON_SPECIAL_ATTACK, this);
			App->audio->PlayFx(10);
		}
		else if (direction == DOWN)
		{
			sp_attack = App->collision->AddCollider({ position.x - 10, position.y + 2, 22, 8 }, COLLIDER_POKEMON_SPECIAL_ATTACK, this);
			App->audio->PlayFx(10);
		}
		else if (direction == LEFT)
		{
			sp_attack = App->collision->AddCollider({ position.x - 22, position.y - 26, 8, 22 }, COLLIDER_POKEMON_SPECIAL_ATTACK, this);
			App->audio->PlayFx(10);
		}
	}
}

bool Walrein::Chasing(float dt)
{
	walking = false;
	if (reset_distance)
	{
		distance = rand() % 75 + 50;
		dis_moved = 0;
		reset_distance = false;
	}
	if (canmove % 3 != 0)
	{
		Move(dt);
	}
	else
	{
		walking = true;
	}
	if (canmove > 500)
	{
		canmove = 0;
	}
	canmove++;


	if (dis_moved >= distance)
	{
		walking = false;
		reset_run = true;
	}


	if (walking == false)
	{
		state = PC_IDLE;
		anim_state = PC_IDLE;
	}
	else
	{
		state = PC_CHASING;
		anim_state = PC_WALKING;
	}
	return true;
}

bool Walrein::Movebyhit(int speed)
{
	if (hp <= 0)
	{
		state = PC_DYING;
		anim_state = PC_DYING;
		return true;
	}

	if (knockback_time.ReadSec() >= 0.2)
	{
		state = PC_IDLE;
		anim_state = PC_IDLE;
		return true;
	}

	if (dir_hit == UP)
	{
		if (App->map->MovementCost(collision_feet->rect.x, collision_feet->rect.y - 4, collision_feet->rect.w, collision_feet->rect.h, UP) == 0)
		{
			position.y -= speed;
		}
	}
	else if (dir_hit == DOWN)
	{
		if (App->map->MovementCost(collision_feet->rect.x, collision_feet->rect.y + collision_feet->rect.h + 4, collision_feet->rect.w, collision_feet->rect.h, DOWN) == 0)
		{
			position.y += speed;
		}
	}
	else if (dir_hit == LEFT)
	{
		if (App->map->MovementCost(collision_feet->rect.x - 4, collision_feet->rect.y, collision_feet->rect.w, collision_feet->rect.h, LEFT) == 0)
		{
			position.x -= speed;
		}
	}
	else if (dir_hit == RIGHT)
	{
		if (App->map->MovementCost(collision_feet->rect.x + collision_feet->rect.w + 4, collision_feet->rect.y, collision_feet->rect.w, collision_feet->rect.h, RIGHT) == 0)
		{
			position.x += speed;
		}
	}
	return true;
}

int Walrein::CheckPlayerPos()
{
	return App->combatmanager->pokemon_active_link->position.DistanceTo(position);
}