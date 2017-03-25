#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Gui.h"
#include "j1GuiEntity.h"
#include "j1GuiElements.h"
#include "j1Player.h"
#include "j1FileSystem.h"
#include "j1AnimationManager.h"
#include "j1Collision.h"
#include "j1InputManager.h"
#include "j1Item.h"
#include "j1DynamicObjects.h"
#include "time.h"
#include "j1Creature.h"

//Constructor
Player::Player() : Creature()
{
	type = CREATURE;
	name = "Link";
}

// Destructor
Player::~Player()
{}

// Called before render is available
bool Player::Awake(pugi::xml_node& conf)
{
	LOG("Loading Texture Player");
	bool ret = true;

	tex_player_file_name = conf.child("atlas").attribute("file").as_string("");
	file_hit = conf.child("hit").attribute("file").as_string("");
	texmapfile_name = conf.child("day-night").attribute("file").as_string("");
	hp = conf.child("stats").attribute("hp").as_int(0);
	attack = conf.child("stats").attribute("attack").as_int(0);
	speed = conf.child("stats").attribute("speed").as_int(0);
	position.x = conf.child("stats").attribute("pos_x").as_int(0);
	position.y = conf.child("stats").attribute("pos_y").as_int(0);


	return ret;
}

// Called before the first frame
bool Player::Start()
{
	bool ret = true;
	Camera_follow_player = true;
	changeResolution = false;
	attacker = false;
	player_texture = App->tex->Load(tex_player_file_name.c_str());
	maptex = App->tex->Load(texmapfile_name.c_str());
	hit_tex = App->tex->Load(file_hit.c_str());
	direction = UP;
	state = IDLE;
	scale = App->win->GetScale();
	width = 15;
	height = 15;
	gamestate = TIMETOPLAY;
	timetoplay = SDL_GetTicks();
	//TEST TAKE STATS BY CONFIG.XML AND IMPLEMENTED IN GAME
	/*stats_temp.clear();
	stats_temp = std::to_string(hp);
	stats_temp.insert(0, "HP LINK -> ");
	hp_text = App->gui->CreateText(stats_temp.c_str(), { 300,150 }, 23);
	stats_temp.clear();//Delete string temp
	stats_temp = std::to_string(attack);
	stats_temp.insert(0, "ATTACK OF LINK -> ");
	attack_text = App->gui->CreateText(stats_temp.c_str(), { 300,200 }, 23);
	*/

	collision_player = App->collision->AddCollider({ position.x, position.y, 15, 15 }, COLLIDER_PLAYER, this);
	App->input_manager->AddListener(this);
	return ret;
}

bool Player::PreUpdate()
{
	bool ret = true;

	return ret;
}

bool Player::Update()//TODO HIGH -> I delete dt but i thing that we need.
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		Camera_follow_player = !Camera_follow_player;

	// STATE MACHINE ------------------
	if (gamestate == INGAME)
	{
		switch (state)
		{
		case IDLE:
		{
			Idle();
			break;
		}
		case WALKING:
		{
			Walking();
			break;
		}
		case ATTACKING:
		{
			Attack();
			break;
		}
		default:
		{
			break;
		}

		}
	}
	else if (gamestate == INMENU)
	{

	}
	else if (gamestate == TIMETOPLAY)
	{
		if (SDL_GetTicks() - timetoplay > 1000)
		{
			gamestate = INGAME;
		}
	}


	//Provisional gem provider
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && charge <= 34)
	{
		charge++;
	}
	else if (charge != 0)
	{
		charge--;
	}
	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT && gems<999)
	{
		gems++;
	}
	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT && bombs<99)
	{
		bombs++;
	}
	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_REPEAT && arrows<99)
	{
		arrows++;
	}
	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		//App->scene->dialog->PushLine(true);
	}
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		App->scene->switch_menu = true;
		gamestate = INMENU;
	}


	//Collision follow the player
	collision_player->SetPos(position.x, position.y);

	return ret;
}

void Player::Draw()
{
	App->anim_manager->Drawing_Manager(state, direction, position, name);
	//App->render->Blit(player_texture, position.x, position.y/*, &Rect_player);
}

bool Player::CleanUp()
{
	bool ret = true;

	return ret;
}


bool Player::Save()
{
	App->entity_elements->XML.child("config").child("Link").child("stats").attribute("hp").set_value(hp);
	App->entity_elements->XML.save_file("config.xml");
	return true;
}

void Player::OnCollision(Collider* c1, Collider* c2)
{
	if (c1 == collision_attack && c2->type == COLLIDER_DYNOBJECT)
	{
		iPoint pos_dyn = App->map->WorldToMap(c2->callback->position.x, c2->callback->position.y);
		//srand(time(NULL)); 		int canDrop = rand() % 5 + 1;
		int canDrop = 1;
		if (canDrop == 1)
		{
			iPoint position;
			position.x = c2->callback->position.x + 4;
			position.y = c2->callback->position.y;
			App->scene->items.push_back(App->entity_elements->CreateItem(1, position));

		}

		App->map->EditCost(pos_dyn.x, pos_dyn.y, App->map->data.tilesets.begin()._Ptr->_Next->_Myval->firstgid);
		App->map->EditCost(pos_dyn.x + 1, pos_dyn.y, App->map->data.tilesets.begin()._Ptr->_Next->_Myval->firstgid);
		App->map->EditCost(pos_dyn.x, pos_dyn.y + 1, App->map->data.tilesets.begin()._Ptr->_Next->_Myval->firstgid);
		App->map->EditCost(pos_dyn.x + 1, pos_dyn.y + 1, App->map->data.tilesets.begin()._Ptr->_Next->_Myval->firstgid);
		App->entity_elements->DeleteDynObject((DynamicObjects*)c2->callback);
		App->collision->EraseCollider(c2);
	}

	if (c1 == collision_player && c2->type == COLLIDER_ITEM)
	{
		if (c2->callback->name == "rupee")
		{
			gems++;
		}
		App->entity_elements->DeleteItem((Item*)c2->callback);
		App->collision->EraseCollider(c2);
	}



	/*if (c1 == collision_player && c2->type == COLLIDER_ENEMY)
	{
		if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y + 1 >= c2->rect.y + c2->rect.h)
		{
			App->render->camera.y -= speed * 2;
			position.y = position.y + 1;
		}
		else if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h - 1 <= c2->rect.y)
		{
			App->render->camera.y += speed * 2;
			position.y = position.y - 1;
		}
		else if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w - 1 <= c2->rect.x)
		{
			App->render->camera.x += speed * 2;
			position.x = position.x - 1;
		}
		else if (c1->rect.x <= c2->rect.x + c2->rect.w && c1->rect.x + 1 >= c2->rect.x + c2->rect.w)
		{
			App->render->camera.x -= speed * 2;
			position.x = position.x + 1;
		}
	}*/
}

bool Player::Camera_inside()
{
	//256x224
	iPoint temp = App->map->MapToWorld(App->map->data.width, App->map->data.height);
	if (direction == UP)
	{
		if (position.y < temp.y - (224 / 2))
		{
			if ((0 < -(App->render->camera.y + 2) && temp.y + 64 > -(App->render->camera.y + 2)) == false)
			{
				return false;
			}
		}
		else
			return false;
	}
	if (direction == DOWN)
	{
		if (position.y > 110)
		{
			if ((0 < -(App->render->camera.y - 2) && temp.y + 64 > -(App->render->camera.y - 2)) == false)
			{
				return false;
			}
		}
		else
			return false;

	}
	if (direction == LEFT)
	{
		if (position.x < temp.x - 130)
		{
			if ((0 < -(App->render->camera.x + 2) && temp.x > -(App->render->camera.x + 2)) == false)
			{
				return false;
			}
		}
		else
			return false;
	}
	if (direction == RIGHT)
	{
		if (position.x > 128)
		{
			if ((0 < -(App->render->camera.x - 2) && temp.x > -(App->render->camera.x - 2)) == false)
			{
				return false;
			}
		}
		else
			return false;
	}
	return true;
}

bool Player::Idle()
{
	//TEST MOVE LINK
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MLEFT) == EVENTSTATE::E_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MDOWN) == EVENTSTATE::E_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MRIGHT) == EVENTSTATE::E_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MUP) == EVENTSTATE::E_REPEAT)
	{
		state = WALKING;
		CheckOrientation();
	}

	else
	{
		state = IDLE;
	}

	return true;
}

bool Player::Walking()
{
	walking = false;
	Move();

	if (walking == false)
	{
		state = IDLE;
	}

	else
	{
		state = WALKING;
	}
	return false;
}

bool Player::Attack()
{
	if (attacker)
	{
		if (SDL_GetTicks() - attack_time > 200)
		{
			App->collision->EraseCollider(collision_attack);
			attacker = false;
			state = IDLE;
		}
	}
	else
	{
		attack_time = SDL_GetTicks();
		attacker = true;
		if (direction == UP)
		{
			collision_attack = App->collision->AddCollider({ position.x + 4, position.y - 10, 20, 12 }, COLLIDER_PLAYER, this);
		}
		else if (direction == RIGHT)
		{
			collision_attack = App->collision->AddCollider({ position.x + 8, position.y + 4, 20, 8 }, COLLIDER_PLAYER, this);
		}
		else if (direction == DOWN)
		{
			collision_attack = App->collision->AddCollider({ position.x + 4, position.y + 6, 20, 12 }, COLLIDER_PLAYER, this);
		}
		else if (direction == LEFT)
		{
			collision_attack = App->collision->AddCollider({ position.x - 8, position.y + 4, 20, 12 }, COLLIDER_PLAYER, this);
		}
		LOG("ATTACK!");
	}

	return true;
}

void Player::OnInputCallback(INPUTEVENT action, EVENTSTATE e_state)
{
	if (gamestate == INGAME)
	{
		switch (action)
		{
		case BUTTON_B:
			if (e_state == E_DOWN)
			{
				state = ATTACKING;
				attack_time = SDL_GetTicks();
			}
			break;
		}
	}
}

bool Player::Move()
{
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MLEFT) == EVENTSTATE::E_REPEAT)
	{
		direction = LEFT;
		if (App->map->MovementCost(position.x - speed, position.y, LEFT) == 0)
		{
			if (Camera_inside())
				App->render->camera.x += speed * scale;
			position.x -= speed;

		}
		walking = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MDOWN) == EVENTSTATE::E_REPEAT)
	{
		direction = DOWN;
		if (App->map->MovementCost(position.x, position.y + (speed + height), DOWN) == 0)
		{
			if (Camera_inside())
				App->render->camera.y -= speed * scale;
			position.y += speed;
		}
		walking = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MRIGHT) == EVENTSTATE::E_REPEAT)
	{
		direction = RIGHT;
		if (App->map->MovementCost(position.x + (speed + width), position.y, RIGHT) == 0)
		{
			if (Camera_inside())
				App->render->camera.x -= speed * scale;
			position.x += speed;
		}
		walking = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MUP) == EVENTSTATE::E_REPEAT)
	{
		direction = UP;
		if (App->map->MovementCost(position.x, position.y - speed, UP) == 0)
		{
			if (Camera_inside())
				App->render->camera.y += speed * scale;
			position.y -= speed;
		}
		walking = true;
	}
	return walking;
}

bool Player::CheckOrientation()
{
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MLEFT) == EVENTSTATE::E_REPEAT)
	{
		direction = LEFT;
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MRIGHT) == EVENTSTATE::E_REPEAT)
	{
		direction = RIGHT;
	}
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MUP) == EVENTSTATE::E_REPEAT)
	{
		direction = UP;
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input_manager->EventPressed(INPUTEVENT::MDOWN) == EVENTSTATE::E_REPEAT)
	{
		direction = DOWN;
	}
	return true;
}
